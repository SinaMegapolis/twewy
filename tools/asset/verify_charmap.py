#!/usr/bin/env python3
"""
Checks the message character map against the retail ROM.

The game's message tables store glyph numbers, and the retail ROM carries the
table that says which character each glyph is: ``SysFontDsCodeMap`` maps a DS
character code to a glyph number, and the DS character set names the code.
``src/Util/SysFont.c`` transcribes that table, so the pair names the glyph -
no guessing from the artwork needed.

This tool therefore does three things:

  * reads the table out of the retail ROM and out of the source, and reports
    any difference between the two (a transcription slip in the source);
  * reads the character map in mesdump.py and reports every glyph whose name
    differs from the one the ROM implies;
  * lists the glyphs the ROM's table names that the character map leaves
    unnamed, and the ones the map names that the ROM does not reach.

Usage:
  python tools/asset/verify_charmap.py                 # check both regions
  python tools/asset/verify_charmap.py --version usa
  python tools/asset/verify_charmap.py --rom extract/baserom_twewy_usa.nds
"""

import argparse
import re
import struct
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SLOTS = 0x209
TABLE_SIGNATURE = struct.pack("<HH", 0x006C, 0x004C) + struct.pack("<HH", 0xE00A, 0x01B5)

# The DS character set, for the codes the game's table uses.  Every code is a
# Unicode value: the kana runs are contiguous in the ROM's table (glyph 0x5F
# holds 0x3041, 0x60 holds 0x3042, and so on), and the rendered sheet shows the
# small kana in the odd slots and the large ones in the even slots, which is
# the Unicode order ぁあぃいぅう... rather than the DS charset's own order.


def ds_charset():
    """DS code -> character, for every code the game's table uses."""
    out = {}
    for code in range(0x20, 0x7F):
        out[code] = chr(code)
    for code in range(0xA1, 0x100):
        out[code] = chr(code)
    for code in (0x0152, 0x0153, 0x02C6, 0x02DC, 0x2010, 0x2015, 0x2018,
                 0x2019, 0x201A, 0x201C, 0x201D, 0x201E, 0x2022, 0x2026,
                 0x2032, 0x2033, 0x203A, 0x203B, 0x20AC, 0x2122, 0x2190,
                 0x2191, 0x2192, 0x2193, 0x221E, 0x2234, 0x25A0, 0x25A1,
                 0x25B2, 0x25B3, 0x25BC, 0x25BD, 0x25C6, 0x25C7, 0x25CB,
                 0x25CE, 0x25CF, 0x2605, 0x2606, 0x266A, 0x266D, 0x266F,
                 0x3000, 0x3001, 0x3002, 0x3005, 0x3008, 0x3009, 0x300C,
                 0x300D, 0x3010, 0x3012, 0x3014, 0x3015, 0x309B, 0x309C,
                 0x309D, 0x309E, 0x30FB, 0x30FC):
        out[code] = chr(code)
    # the DS charset's fullwidth forms.  Four of them are not the Unicode
    # character of the same code, which the rendered sheet shows: 0xFF3B and
    # 0xFF3D are the tortoise-shell brackets, 0xFF40 is the grave accent and
    # 0xFFE3 is the geta mark.
    for code in range(0xFF01, 0xFF5F):
        out[code] = chr(code)
    out[0xFF3B] = "【"
    out[0xFF3D] = "】"
    out[0xFF40] = "`"
    out[0xFFE3] = "〓"
    for code in range(0x3041, 0x3094):
        out[code] = chr(code)
    for code in range(0x30A1, 0x30F7):
        out[code] = chr(code)
    for i in range(0x1D):
        out[0xE000 + i] = chr(0xE000 + i)
    return out


CHARSET = ds_charset()


def rom_table(rom):
    """The table's 521 slots, read out of the retail ROM."""
    hit = rom.find(TABLE_SIGNATURE)
    if hit < 0:
        raise SystemExit("SysFontDsCodeMap not found in the ROM")
    out = []
    for slot in range(SLOTS):
        ds, glyph = struct.unpack_from("<HH", rom, hit + slot * 4)
        out.append((ds, glyph))
    return out


def source_table(source):
    """The table's slots, read out of src/Util/SysFont.c."""
    start = source.index("SysFontDsCodeMap[0x209] = {")
    body = source[start:source.index("};", start)]
    out = []
    for m in re.finditer(r"\{(0x[0-9A-Fa-f]+),\s*(0x[0-9A-Fa-f]+)\}", body):
        out.append((int(m.group(1), 16), int(m.group(2), 16)))
    if len(out) != SLOTS:
        raise SystemExit("the source table has %d entries, expected %d" % (len(out), SLOTS))
    return out


def named_glyphs(table):
    """Glyph -> character, for every entry whose DS code is known."""
    out = {}
    for ds, glyph in table:
        ch = CHARSET.get(ds)
        if ch is not None and glyph not in out:
            out[glyph] = ch
    return out


def load_charmap():
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import mesdump
    return mesdump.load_charmap()


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[1])
    parser.add_argument("--rom", type=Path, default=REPO / "extract/baserom_twewy_usa.nds")
    parser.add_argument("--source", type=Path, default=REPO / "src/Util/SysFont.c")
    parser.add_argument("--quiet", action="store_true", help="only report problems")
    args = parser.parse_args()

    if not args.rom.exists():
        print("no retail ROM at %s; nothing to check against" % args.rom)
        return 0
    rom = args.rom.read_bytes()
    rom_entries = rom_table(rom)
    src_entries = source_table(args.source.read_text(encoding="utf-8"))
    problems = 0

    if rom_entries != src_entries:
        problems += 1
        print("the table in the source differs from the ROM:")
        for a, b in zip(src_entries, rom_entries):
            if a != b:
                print("  source %04X -> %04X, rom %04X -> %04X" % (a[0], a[1], b[0], b[1]))
        if len(src_entries) != len(rom_entries):
            print("  source has %d entries, the ROM has %d" % (len(src_entries), len(rom_entries)))
    elif not args.quiet:
        print("the table in %s matches the ROM (%d entries)" % (
            args.source.relative_to(REPO), len(rom_entries)))

    named = named_glyphs(rom_entries)
    charmap = load_charmap()

    wrong = []
    for glyph, ch in sorted(named.items()):
        have = charmap.get(glyph)
        if have != ch:
            wrong.append((glyph, have, ch))
    if wrong:
        problems += 1
        print("\n%d glyph(s) the character map names differently from the ROM:" % len(wrong))
        for glyph, have, ch in wrong:
            print("  0x%04X  map %-8r rom %r" % (glyph, have, ch))
    elif not args.quiet:
        print("the character map agrees with the ROM on all %d named glyphs" % len(named))

    unnamed = sorted(set(named) - set(charmap))
    if unnamed:
        print("\n%d glyph(s) the ROM names that the character map leaves unnamed:" % len(unnamed))
        for glyph in unnamed:
            print("  0x%04X  (the ROM says %r)" % (glyph, named[glyph]))

    extra = sorted(set(charmap) - set(named))
    if extra and not args.quiet:
        print("\n%d character map entries the ROM's table does not reach:" % len(extra))
        for glyph in extra:
            print("  0x%04X  %r" % (glyph, charmap[glyph]))

    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
