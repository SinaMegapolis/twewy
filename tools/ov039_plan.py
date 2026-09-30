#!/usr/bin/env python3
"""Plan the ov039 TU split.

The overlay is a scene/infra block followed by 30 self-contained task TUs.  Each
task's TaskHandle points at its "Tsk_OtosuGame_*" string and its RunTask
callback, and the tasks are laid out back-to-back in .text, so each task owns
one contiguous address range.  This prints those ranges with function counts so
the split can be planned.

    python tools/ov039_plan.py            # all tasks
    python tools/ov039_plan.py --sort size
"""
import argparse
import re

SYMS = "config/usa/arm9/overlays/ov039/symbols.txt"
RELS = "config/usa/arm9/overlays/ov039/relocs.txt"
TEXT_START = 0x020824A0
TEXT_END = 0x02098E24


def functions():
    out = []
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:function\(arm,size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)", line)
        if m:
            out.append((int(m.group(3), 16), int(m.group(2), 16)))
    out.sort()
    return out


def relocs():
    out = {}
    for line in open(RELS):
        m = re.match(r"from:(0x[0-9a-f]+) kind:(\w+) to:(0x[0-9a-f]+)", line)
        if m:
            out[int(m.group(1), 16)] = (m.group(2), int(m.group(3), 16))
    return out


def task_entries(rel):
    """[(run_addr, name)] for every Tsk_/Seq_ TaskHandle in the overlay."""
    out = []
    for addr, (kind, target) in rel.items():
        if kind != "load":
            continue
        nxt = rel.get(addr + 4)
        if not nxt or nxt[0] != "load":
            continue
        # the handle's word0 must point at a string; the name is the target's
        name = STRING_NAMES.get(target)
        if name and (name.startswith("Tsk_") or name.startswith("Seq_")):
            out.append((nxt[1], name))
    return sorted(set(out))


def load_string_names():
    """data symbol address -> decoded string, from the dsd disassembly."""
    txt = open("build/usa/asm/ov039_4.s", errors="replace").read()
    addr = {}
    for m in re.finditer(r"^(\S+):\n((?:    \.byte [^\n]*\n)+)", txt, re.M):
        bs = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", m.group(2)))
        addr[m.group(1)] = bs.split(b"\x00")[0].decode("ascii", "replace")
    sym = {}
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:\S+(?:,\S+)? addr:(0x[0-9a-f]+)", line)
        if m:
            sym[m.group(1)] = int(m.group(2), 16)
    # address -> decoded string, for the data symbols the asm names
    return {sym[name]: text for name, text in addr.items() if name in sym}


STRING_NAMES = {}


def main():
    global STRING_NAMES
    ap = argparse.ArgumentParser()
    ap.add_argument("--sort", choices=["addr", "size", "fns"], default="addr")
    a = ap.parse_args()

    STRING_NAMES = load_string_names()
    fns = functions()
    entries = task_entries(relocs())
    if not entries:
        print("no task handles found -- is build/usa/asm/ov039_4.s present?")
        return

    rows = []
    scene_end = entries[0][0]
    rows.append((TEXT_START, scene_end, "scene/infra (no task handle)", True))
    for i, (st, name) in enumerate(entries):
        en = entries[i + 1][0] if i + 1 < len(entries) else TEXT_END
        rows.append((st, en, name, False))

    out = []
    for st, en, name, is_scene in rows:
        sel = [(x, y) for x, y in fns if st <= x < en]
        out.append((name, st, en, len(sel), sum(y for _, y in sel), is_scene))

    if a.sort == "size":
        out.sort(key=lambda r: -r[4])
    elif a.sort == "fns":
        out.sort(key=lambda r: -r[3])
    else:
        out.sort(key=lambda r: r[1])

    print(f"{'unit':30s} {'start':>10s} {'end':>10s} {'fns':>5s} {'bytes':>7s}")
    for name, st, en, n, sz, is_scene in out:
        tag = "  <- starts here" if name == "Tsk_OtosuGame_badge" else ""
        print(f"{name:30s} {st:#010x} {en:#010x} {n:5d} {sz:7d}{tag}")
    print(f"\ntotal {len(fns)} functions, {sum(y for _, y in fns)} bytes")


if __name__ == "__main__":
    main()
