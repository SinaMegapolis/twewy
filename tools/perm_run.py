#!/usr/bin/env python3
"""perm_run.py -- a small decomp-permuter, driven by tools/perm_score.py.

decomp-permuter itself is not installed on this machine (its documented path is a
Linux checkout, and this box has no ARM assembler for the target object), so this
reimplements the part that matters: mutate the C, recompile, score against the
target's bytes, and hill-climb.

  python tools/perm_run.py score <workdir>            # score base.c as-is
  python tools/perm_run.py search <workdir> [iters]   # hill-climb from base.c

Scoring is strict differing-word count from perm_score.py; lower is better and 0
is a byte-exact match.  The masked count is tracked too and printed, because a
non-zero masked score means the opcode stream differs and no amount of
mutation will reach 0 -- useful for deciding when to stop.
"""

import random
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

from perm_score import compare, function_bytes  # noqa: E402

COMPILER = str(ROOT / "tools/mwccarm/2.0/sp1p5/mwccarm.exe")
CC_FLAGS = (
    "-O4,p -enum int -char signed -proc arm946e -gccext,on -fp soft "
    "-inline noauto -RTTI off -interworking -w off -sym on -gccinc -nolink "
    "-msgstyle gcc -enc SJIS -ipa file -str noreuse -Cpp_exceptions off"
)
CC_INCLUDES = (
    "-i include -i libs/include -i libs/c/include -i libs/cpp/include "
    "-i libs/nitro/include -i libs/runtime/include"
)


def compile_c(path, out):
    p = subprocess.run(
        [COMPILER, *CC_FLAGS.split(), *CC_INCLUDES.split(),
         "-lang=c99", "-d", "REGION_USA", "-c", str(path), "-o", str(out)],
        cwd=ROOT, capture_output=True, text=True,
    )
    return p.returncode == 0, p.stdout + p.stderr


def call_count(text):
    """Number of apparent function calls in a function body.

    A byte-scorer has no notion of semantics, so it can be gamed: duplicating a
    call adds instructions that happen to align the word stream better and
    "improves" the score while making the code wrong.  Every gaming attempt seen
    so far -- on func_ov039_02083ed4 and func_ov039_02082774 -- was exactly a
    duplicated call, so the search rejects any candidate that adds one.
    """
    return len(re.findall(r"\b(?:func_|Otu|Main|Touch|Oam|Palette|Display|DMA|DC_|GX|MI_|Easy|Resource|Text_|Interrupts|DatMgr|Mem_|HBlank|Color)\w*\s*\(", text))


def score(work, func, verbose=False):
    base_c, base_o = work / "base.c", work / "base.o"
    ok, log = compile_c(base_c, base_o)
    if not ok:
        return None, None, "compile failed"
    try:
        _, tb = function_bytes(work / "target.o", func)
        _, cb = function_bytes(base_o, func)
    except KeyError as e:
        return None, None, str(e)
    strict, masked = compare(tb, cb)
    if verbose:
        print(f"  strict={strict} masked={masked} target={len(tb)//4}w build={len(cb)//4}w")
    return strict, masked, None


# ---------------------------------------------------------------- mutations

ASSIGN = re.compile(r"^(\s*)([\w\.\[\]>\-]+)\s*=\s*([^;]+);$", re.S)
IDENT = re.compile(r"^[A-Za-z_]\w*$")


def _statements(body):
    """Split a function body into top-level statement strings."""
    out, buf, depth, i = [], [], 0, 0
    while i < len(body):
        ch = body[i]
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == ";" and depth == 0:
            out.append("".join(buf) + ";")
            buf = []
        else:
            buf.append(ch)
        i += 1
    if "".join(buf).strip():
        out.append("".join(buf))
    return [s for s in out if s.strip()]


def _rebuild(stmts, tail):
    return "".join(s if s.endswith(("{", "}", ";")) else s + ";" for s in stmts) + tail


def mut_reorder_stmts(fn, rng):
    """Swap two adjacent top-level statements, but never across a dependency.

    The byte scorer accepts a swap that puts `x = f();` after a use of `x`,
    because an uninitialised read is cheap to align. Two of the first batch's
    four "wins" were exactly that (`negative = scaled < 0` hoisted above the
    assignment to `scaled`), so a swap is rejected when the earlier statement
    assigns a name the later one reads.
    """
    stmts = _statements(fn)
    if len(stmts) < 2:
        return None
    for _ in range(8):
        i = rng.randrange(len(stmts) - 1)
        a, b = stmts[i], stmts[i + 1]
        m = ASSIGN.match(a.strip())
        if m:
            n = re.match(r"[\w\.\[\]>-]+", m.group(2))
            if n and re.search(r"\b%s\b" % re.escape(n.group(0)), b):
                continue
        stmts[i], stmts[i + 1] = b, a
        return _rebuild(stmts, "")
    return None


def mut_block_scope(fn, rng):
    """Wrap a random run of statements in a bare block, or unwrap one."""
    stmts = _statements(fn)
    cand = [i for i, s in enumerate(stmts) if s.rstrip().endswith("{")]
    if cand and rng.random() < 0.5:
        i = rng.choice(cand)
        inner = stmts[i].strip()
        j = i + 1
        while j < len(stmts) and not stmts[j].rstrip().endswith("}"):
            j += 1
        if j < len(stmts):
            body = _statements(inner[inner.index("{") + 1:inner.rindex("}")])
            new = stmts[:i] + body + stmts[j + 1:]
            return _rebuild(new, "")
    if len(stmts) < 2:
        return None
    i = rng.randrange(len(stmts) - 1)
    wrapped = "{" + stmts[i] + stmts[i + 1] + "}"
    new = stmts[:i] + [wrapped] + stmts[i + 2:]
    return _rebuild(new, "")


def mut_temp(fn, rng):
    """Split `x = <expr>;` into a fresh temp plus the assignment."""
    stmts = _statements(fn)
    idx = [i for i, s in enumerate(stmts) if ASSIGN.match(s.strip())]
    if not idx:
        return None
    i = rng.choice(idx)
    m = ASSIGN.match(stmts[i].strip())
    indent, lhs, rhs = m.group(1), m.group(2), m.group(3)
    n = rng.randrange(100)
    new = (f"{indent}s32 perm_tmp_{n} = {rhs};\n"
           f"{indent}{lhs} = perm_tmp_{n};")
    return _rebuild(stmts[:i] + [new] + stmts[i + 1:], "")


def mut_commutative(fn, rng):
    """Swap the operands of a top-level `a + b` or `a * b` assignment."""
    stmts = _statements(fn)
    idx = [i for i, s in enumerate(stmts) if ASSIGN.match(s.strip())]
    rng.shuffle(idx)
    for i in idx:
        m = ASSIGN.match(stmts[i].strip())
        indent, lhs, rhs = m.group(1), m.group(2), m.group(3)
        for op in (" + ", " * ", " & ", " | "):
            if op in rhs:
                a, b = rhs.split(op, 1)
                if a.strip() and b.strip():
                    stmts[i] = f"{indent}{lhs} = {b}{op}{a};"
                    return _rebuild(stmts, "")
    return None


def mut_cast(fn, rng):
    """Add or remove an explicit (s32) cast on an assignment's right side."""
    stmts = _statements(fn)
    idx = [i for i, s in enumerate(stmts) if ASSIGN.match(s.strip())]
    if not idx:
        return None
    i = rng.choice(idx)
    m = ASSIGN.match(stmts[i].strip())
    indent, lhs, rhs = m.group(1), m.group(2), m.group(3)
    if rhs.strip().startswith("(s32)"):
        rhs = rhs.strip()[5:]
    else:
        rhs = f"(s32)({rhs})"
    stmts[i] = f"{indent}{lhs} = {rhs};"
    return _rebuild(stmts, "")


PTR_LOCAL = re.compile(r"^\s*([\w\*]+)\s*\*\s*(\w+)\s*=\s*(.+?);\s*$")


def mut_inline_ptr(fn, rng):
    """Replace a cached pointer local's uses with its initialiser expression.

    This is the high-value mutation for this overlay: the target repeatedly
    re-derives a row pointer (`ldr [r4,#0]` then `add r1, r4, r0, lsl #4`) where
    a source that caches the pointer in a local emits the multiply once. Caching
    a pointer is the natural thing to write, so no amount of reordering finds
    this -- only dropping the local does.
    """
    lines = fn.split("\n")
    cands = [i for i, l in enumerate(lines) if PTR_LOCAL.match(l)]
    if not cands:
        return None
    i = rng.choice(cands)
    m = PTR_LOCAL.match(lines[i])
    var, expr = m.group(2), m.group(3)
    uses = [k for k, l in enumerate(lines) if re.search(rf"\b{re.escape(var)}\b", l) and k != i]
    if not uses:
        return None
    out = lines[:i] + lines[i + 1:]
    for k in sorted(uses, reverse=True):
        if k > i:
            k -= 1
        out[k] = re.sub(rf"\b{re.escape(var)}\b", f"({expr})", out[k])
    return "\n".join(out)


def mut_dup_stmt(fn, rng):
    """Duplicate a side-effect-free-looking statement next to itself."""
    stmts = _statements(fn)
    if len(stmts) < 2:
        return None
    i = rng.randrange(len(stmts))
    s = stmts[i]
    if any(op in s for op in ("++", "--", "=", "bl ", "call")):
        return None
    return _rebuild(stmts[:i + 1] + [s] + stmts[i + 1:], "")



DECL_LINE = re.compile(
    r"^\s*(?:s|u|)?"  # signed/unsigned prefix or none
    r"(?:s32|u32|s16|u16|s8|u8|char|void|BOOL|Vec|Sprite|Otu\w+|Task\w*|Task|"
    r"Data|display|int)\b[^;=]*;\s*$"
)


def mut_reorder_decls(fn, rng):
    """Swap two adjacent local declaration lines.

    mwcc picks the register for two locals from their declaration order, so a
    pure r6<->r7 or r0<->r2 swap is often this and nothing else.
    """
    lines = fn.split("\n")
    idx = [i for i, l in enumerate(lines) if DECL_LINE.match(l)]
    pairs = [(i, i + 1) for i in range(len(lines) - 1) if i in idx and (i + 1) in idx]
    if not pairs:
        return None
    i, j = rng.choice(pairs)
    lines[i], lines[j] = lines[j], lines[i]
    return "\n".join(lines)


def mut_signedness(fn, rng):
    """Flip a local or cast between signed and unsigned 16-bit."""
    lines = fn.split("\n")
    cand = [i for i, l in enumerate(lines) if re.search(r"\bs16\b|\bu16\b|\(s16\)|\(u16\)", l)]
    if not cand:
        return None
    i = rng.choice(cand)
    l = lines[i]
    if "s16" in l:
        lines[i] = l.replace("s16", "u16")
    else:
        lines[i] = l.replace("u16", "s16")
    return "\n".join(lines)

MUTATIONS = [
    (mut_inline_ptr, 6.0),
    (mut_reorder_stmts, 3.0),
    (mut_block_scope, 2.0),
    (mut_temp, 3.0),
    (mut_commutative, 1.5),
    (mut_cast, 1.5),
    (mut_dup_stmt, 0.5),
    (mut_reorder_decls, 4.0),
    (mut_signedness, 1.5),
]


def body_of(fn_text):
    b = fn_text.index("{")
    return b + 1, fn_text.rindex("}")


def mutate(fn_text, rng):
    """Return a mutated copy of the function text, or None.

    Mutations act on the function *body*, but the declaration lines a
    pointer-inlining pass needs to find live in the body too here because the
    locals are declared at the top of it.  Anything the mutation set cannot
    express (new locals, changed types) stays out of scope on purpose: the
    search is a supplement to reading the target, not a replacement.
    """
    funcs, funcs_w = [m for m, _ in MUTATIONS], [w for _, w in MUTATIONS]
    for _ in range(12):
        f = rng.choices(funcs, weights=funcs_w, k=1)[0]
        lo, hi = body_of(fn_text)
        nb = f(fn_text[lo:hi], rng)
        if nb is None:
            continue
        cand = fn_text[:lo] + nb + fn_text[hi:]
        if cand != fn_text:
            return cand
    return None


def main():
    if len(sys.argv) < 3:
        sys.stderr.write(__doc__)
        sys.exit(2)
    mode, workname = sys.argv[1], sys.argv[2]
    work = Path(workname)
    func = work.name
    orig = (work / "base.c").read_text(encoding="latin-1")

    if mode == "score":
        s, m, err = score(work, func, verbose=True)
        if err:
            print("error:", err)
            sys.exit(1)
        print(f"score {s}")
        return

    iters = int(sys.argv[3]) if len(sys.argv) > 3 else 400
    rng = random.Random(int(sys.argv[4]) if len(sys.argv) > 4 else 1234)

    best_s, best_m, err = score(work, func)
    if err:
        sys.exit(f"baseline: {err}")
    print(f"baseline strict={best_s} masked={best_m}", flush=True)
    if best_s == 0:
        print("already a byte-exact match")
        return

    best_src = orig
    base_calls = call_count(orig)
    (work / "best.c").write_text(best_src, encoding="latin-1")
    tried = kept = rejected = 0
    for it in range(iters):
        cand = mutate(best_src, rng)
        if cand is None:
            continue
        # Guard against the scorer being gamed by added instructions.
        if call_count(cand) > base_calls:
            rejected += 1
            continue
        (work / "base.c").write_text(cand, encoding="latin-1")
        s, m, err = score(work, func)
        tried += 1
        if err or s is None or s >= best_s:
            continue
        best_s, best_m, best_src = s, m, cand
        (work / "best.c").write_text(best_src, encoding="latin-1")
        kept += 1
        print(f"  iter {it:4d}: strict {s} masked {m}  (kept)", flush=True)
        if s == 0:
            break

    # Leave the best candidate in place so a follow-up score is reproducible.
    (work / "base.c").write_text(best_src, encoding="latin-1")
    score(work, func)
    print(f"done: {tried} tried, {rejected} rejected as call-count increases, "
          f"{kept} improvements, best strict={best_s} masked={best_m}")


if __name__ == "__main__":
    main()
