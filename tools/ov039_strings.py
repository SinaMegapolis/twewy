#!/usr/bin/env python3
"""Dump ov039's .data string region with sizes and decoded contents.

Writes to a file rather than stdout: the contents are Shift-JIS, which the
Windows console codepage cannot represent.

    python tools/ov039_strings.py > out.txt
"""
import re
import sys

ASM = "build/usa/asm/ov039_4.s"
START = "data_ov039_0209a16c:"
END = "data_ov039_0209a2e0:"

txt = open(ASM, errors="replace").read()
seg = txt[txt.index(START):txt.index(END)]

out = []
total = 0
for m in re.finditer(r"^(data_ov039_\w+):( ; ambiguous)?\n((?:    \.byte [^\n]*\n)+)",
                     seg, re.M):
    name = m.group(1)
    amb = "  (ambiguous)" if m.group(2) else ""
    raw = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", m.group(3)))
    n = len(raw)
    total += n
    body = raw.split(b"\x00")[0]
    try:
        dec = body.decode("shift_jis")
    except UnicodeDecodeError:
        dec = "<not shift-jis: " + body.hex() + ">"
    printable = "".join(c if 32 <= ord(c) < 127 else f"\\x{ord(c):02x}" for c in dec)
    out.append(f"{name}  size={n:3d}{amb}\n    {printable!r}")

out.append(f"\ntotal bytes in region: {total}")
sys.stdout.write("\n".join(out) + "\n")
