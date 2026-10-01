#!/usr/bin/env python3
"""Build a standalone compile+score harness for one function.

decomp-permuter is not installed on this box, and its usual flow leans on tools
this machine does not have (a Linux compiler wrapper, an ARM assembler).  The
search itself is easy to reproduce, though, and this builds the two things it
needs:

  <workdir>/base.c    a translation unit containing the whole real file's
                      context -- its includes, typedefs, statics and data -- with
                      every function *except* the one under test removed.  That
                      keeps struct layouts, macros and file-local tables exactly
                      as the real build sees them, which is what makes the
                      standalone score agree with the in-tree score.
  <workdir>/target.o  the delinked target object, which the scorer reads the
                      expected bytes from.

Then compile with the unit's real mwcc flags and score with tools/perm_score.py.

    python tools/perm_harness.py <unit> <function> [workdir]
"""

import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

COMPILER = "./tools/mwccarm/2.0/sp1p5/mwccarm.exe"
CC_FLAGS = (
    "-O4,p -enum int -char signed -proc arm946e -gccext,on -fp soft "
    "-inline noauto -RTTI off -interworking -w off -sym on -gccinc -nolink "
    "-msgstyle gcc -enc SJIS -ipa file -str noreuse -Cpp_exceptions off"
)
CC_INCLUDES = (
    "-i include -i libs/include -i libs/c/include -i libs/cpp/include "
    "-i libs/nitro/include -i libs/runtime/include"
)


def split_top_level(text):
    """Split a function body into top-level statements / blocks.

    Braces, parens and brackets are tracked so a compound statement stays whole;
    only separators at depth zero become breaks.
    """
    out, buf, depth, i = [], [], 0, 0
    while i < len(text):
        ch = text[i]
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == ";" and depth == 0:
            out.append("".join(buf) + ";")
            buf = []
        elif ch == "{" and depth == 1 and "".join(buf).strip() == "":
            # A bare block at statement position: capture it whole.
            j, d = i, 0
            while j < len(text):
                if text[j] == "{":
                    d += 1
                elif text[j] == "}":
                    d -= 1
                    if d == 0:
                        break
                j += 1
            out.append(text[i:j + 1])
            buf = []
            i = j + 1
            continue
        else:
            buf.append(ch)
        i += 1
    if "".join(buf).strip():
        out.append("".join(buf))
    return [s for s in out if s.strip()]


def find_function(text, func):
    """Return (start, end) byte range of `func`'s definition, or None."""
    name_re = re.compile(rf"^(?!static\b)(?:[\w\*]+\s+)+{re.escape(func)}\s*\(", re.M)
    for m in name_re.finditer(text):
        brace = text.find("{", m.end())
        semi = text.find(";", m.end())
        if brace == -1 or (semi != -1 and semi < brace):
            continue  # forward declaration
        depth, i = 0, brace
        while i < len(text):
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
                if depth == 0:
                    return m.start(), i + 1
            i += 1
    return None


def strip_other_functions(text, keep):
    """Remove every function definition except `keep`.

    Data definitions, typedefs, extern declarations and the file's leading
    comment block are all left alone -- only `name(...) { ... }` at column 0 goes.
    """
    out, pos = [], 0
    for m in re.finditer(r"^(?!static\b)(?:[\w\*]+\s+)+(\w+)\s*\([^;{]*\)\s*\{", text, re.M):
        name = m.group(1)
        if name == keep:
            continue
        brace = text.index("{", m.end() - 1)
        depth, i = 0, brace
        while i < len(text):
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        out.append(text[pos:m.start()])
        pos = i + 1
    out.append(text[pos:])
    return "".join(out)


def main():
    unit, func = sys.argv[1], sys.argv[2]
    work = Path(sys.argv[3] if len(sys.argv) > 3 else f"permwork/{func}")
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)

    src = ROOT / f"{unit}.c"
    # latin1 rather than shift_jis: a decode failure on a stray byte in a comment
    # should not stop the harness being built, and the bytes round-trip either
    # way since they are never re-encoded.
    text = src.read_text(encoding="latin1")

    span = find_function(text, func)
    if span is None:
        sys.exit(f"{func} not found in {src}")
    fn_text = text[span[0]:span[1]]

    stripped = strip_other_functions(text, func)
    span2 = find_function(stripped, func)
    base = stripped[:span2[0]] + fn_text + stripped[span2[1]:]
    (work / "base.c").write_text(base, encoding="latin1")

    tgt = ROOT / "build/usa/delinks" / f"{unit}.o"
    if not tgt.exists():
        sys.exit(f"missing target object {tgt}")
    shutil.copy(tgt, work / "target.o")

    print(f"harness: {work}")
    print(f"  base.c   {len(base)} bytes, function {len(fn_text)} bytes")
    print(f"  target.o copied from {tgt}")


if __name__ == "__main__":
    main()
