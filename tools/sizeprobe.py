#!/usr/bin/env python3
"""Find which member of TinPinSlammer_State makes mwcc inflate its sizeof.

Compile a probe per candidate struct shape and read the emitted .bss object
size back with dsd's elf-symbols dumper.  A .bss of N bytes means sizeof is 4N.

    python tools/sizeprobe.py
"""
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CC = ROOT / "tools" / "mwccarm" / "2.0" / "sp1p5" / "mwccarm.exe"

FLAGS = ["-O4,p", "-enum", "int", "-char", "signed", "-proc", "arm946e",
         "-gccext,on", "-fp", "soft", "-inline", "noauto", "-RTTI", "off",
         "-interworking", "-w", "off", "-sym", "on", "-gccinc", "-nolink",
         "-msgstyle", "gcc", "-enc", "SJIS", "-ipa", "file", "-str", "noreuse",
         "-Cpp_exceptions", "off", "-lang=c99", "-d", "REGION_USA"]

PREAMBLE = "#include <nitro/types.h>\n"

# Each shape is a struct body; the probe declares it and emits a .bss array.
SHAPES = {
    "full": """
    u8 pad_000[0x698];
    s32 unk_698;
    s32 unk_69C;
    u8 pad_6A0[4];
    s32 unk_6A4;
    u8 resHandler[0x18];
    u8 pad_6C0[0x41C];
    s32 unk_ADC;
    s32 unk_AE0;
    u8 pad_AE4[0xC];
    s32 unk_AF0;
    s32 unk_AF4;
    u8 pad_AF8[0x3E8];
    s32 unk_EE0;
    s32 unk_EE4;
    s32 unk_EE8;
    s32 unk_EEC;
    u8 pad_EF0[0x1D4];
""",
    "no_tail_fields": """
    u8 pad_000[0x698];
    s32 unk_698;
    s32 unk_69C;
    u8 pad_6A0[4];
    s32 unk_6A4;
    u8 resHandler[0x18];
    u8 pad_6C0[0x41C];
    s32 unk_ADC;
    s32 unk_AE0;
    u8 pad_AE4[0xC];
    s32 unk_AF0;
    s32 unk_AF4;
    u8 pad_AF8[0x3E8];
    s32 unk_EE0;
    s32 unk_EE4;
    s32 unk_EE8;
    s32 unk_EEC;
""",
    "no_pad_EF0": """
    u8 pad_000[0x698];
    s32 unk_698;
    s32 unk_69C;
    u8 pad_6A0[4];
    s32 unk_6A4;
    u8 resHandler[0x18];
    u8 pad_6C0[0x41C];
    s32 unk_ADC;
    s32 unk_AE0;
    u8 pad_AE4[0xC];
    s32 unk_AF0;
    s32 unk_AF4;
    u8 pad_AF8[0x3E8];
    s32 unk_EE0;
    s32 unk_EE4;
    s32 unk_EE8;
    s32 unk_EEC;
    u8 pad_EF0[1];
""",
    "all_u8_fields": """
    u8 pad_000[0x698];
    u8 f_698[4];
    u8 f_69C[4];
    u8 pad_6A0[4];
    u8 f_6A4[4];
    u8 resHandler[0x18];
    u8 pad_6C0[0x41C];
    u8 f_ADC[4];
    u8 f_AE0[4];
    u8 pad_AE4[0xC];
    u8 f_AF0[4];
    u8 f_AF4[4];
    u8 pad_AF8[0x3E8];
    u8 f_EE0[4];
    u8 f_EE4[4];
    u8 f_EE8[4];
    u8 f_EEC[4];
    u8 pad_EF0[0x1D4];
""",
}


def probe(body, td):
    src = Path(td) / "p.c"
    obj = Path(td) / "p.o"
    src.write_text(
        PREAMBLE
        + "typedef struct {" + body + "} S;\n"
        + "enum { N = sizeof(S) / 4 };\n"
        + "char ProbeArray[N];\n"
    )
    cmd = [str(CC), *FLAGS]
    for p in ["include", "libs/include", "libs/c/include", "libs/cpp/include",
              "libs/nitro/include", "libs/runtime/include"]:
        cmd += ["-i", p]
    cmd += ["-c", str(src), "-o", str(obj)]
    subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if not obj.exists():
        return None
    p = subprocess.run([str(ROOT / "dsd.exe"), "dump", "elf-symbols",
                        "--elf-path", str(obj)],
                       cwd=ROOT, capture_output=True, text=True)
    for line in p.stdout.splitlines():
        if "ProbeArray" in line and ".bss" in line:
            m = re.search(r"0x([0-9a-f]+)\s+global", line)
            if m:
                return int(m.group(1), 16)
    return None


def main():
    with tempfile.TemporaryDirectory() as td:
        for name, body in SHAPES.items():
            n = probe(body, td)
            if n is None:
                print(f"{name:18s} build failed")
                continue
            print(f"{name:18s} sizeof = {n:#x}")


if __name__ == "__main__":
    sys.exit(main())
