#!/usr/bin/env python3
"""
Decodes Apl_Fuk/mestxt.bin into readable text.

Container format
----------------
  mestable.bin : MessageTableEntry { s32 offset; s32 size; } [n]
  mestxt.bin   : concatenated messages, each an array of SysCodes terminated
                 by SYSFONT_CODE_STR_END (0xFFFF).

A SysCode is a glyph index in the font's own code space, or a control code.
See include/Util/SysFont.h and src/Util/SysFont.c.

Text escapes
------------
  \\n                        SYSFONT_CODE_LINEBREAK
  \\\\                       a literal backslash
  <c1>..<cF>                 set text colour (SYSFONT_CODE_COLOR_FIRST + n)
  <u32> <s32> <str> <chr>    format specifiers, one vararg each
  <fx0>..<fx4>               fixed-point, n fractional digits
  <w1x> <w2x>                single/double width glyph cells
  <msgend>                   SYSFONT_CODE_MSG_END
  {XXXX}                     raw SysCode, for any glyph or control code with no known mapping
"""

import argparse
import collections
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from glyph_names import GLYPH_NAMES

REPO = Path(__file__).resolve().parents[2]  # tools/asset/mesdump.py -> repo root
VERSIONS = ("usa", "jp")


def data_dir(version):
    return REPO / "extract" / version / "files/Apl_Fuk"


def out_dir(version):
    return REPO / "assets" / version / "text"


# Terminator for messages
STR_END = 0xFFFF

# Control codes for the message system
CONTROL = {
    0xFFFD: "<msgend>",
    0xFFFE: "\\n",
    0xFFE0: "<w1x>",
    0xFFE1: "<w2x>",
    0xFFD0: "<u32>",
    0xFFD1: "<s32>",
    0xFFD2: "<str>",
    0xFFD3: "<chr>",
}
CONTROL.update({0xFFC0 + n: "<fx%d>" % n for n in range(5)})
CONTROL.update({0xFFB1 + n: "<c%X>" % (n + 1) for n in range(0xF)})

# Inline icon glyphs. These are pictograms with no Unicode equivalent,
# though the devs left a legend embedded in the message table
ICONS = [
    "po",
    "ne",
    "ab",
    "to",
    "sl",
    "dr",
    "ko",
    "ci",
    "mi",
    "re",
    "pu",
    "bt",
    "at",
    "mu",
]
CONTROL.update({0x0226 + n: f"<ic_{name}>" for n, name in enumerate(ICONS)})

# The font's code space (first glyph index, characters in glyph order)
# The glyph numbers of the message code space, with the character each glyph
# draws.  From 0x108 on, the names come from the retail USA ROM: its
# SysFontDsCodeMap (the table transcribed in src/Util/SysFont.c) maps a DS
# character code to a glyph, and the DS character set names the code, so the
# pair names the glyph.  tools/asset/verify_charmap.py checks the table in
# the source against the ROM and reports every glyph that disagrees.
CHARMAP_RUNS = [
    (0, "".join(chr(0x20 + n) for n in range(95))),  # ASCII 0x20-0x7E
    # The kana run is contiguous and in plain Unicode order: the ROM's code
    # map points 0x3041 (ぁ) at glyph 0x5F and 0x30A1 (ァ) at glyph 0xB2, and
    # the run reaches 0x30F6 (ヶ) at 0x107 with no gaps.  There are no separate
    # "extra kana" blocks; the small vowels simply live between the large ones.
    (95, "".join(chr(c) for c in range(0x3041, 0x3094))),   # hiragana ぁ..ん
    (178, "".join(chr(c) for c in range(0x30A1, 0x30F7))),  # katakana ァ..ヶ
    (264, "、。，．・：；？！゛゜´`¨＾〓ゝゞ々ー―‐／＼～｜…‘’“”（）〔〕【】｛｝〈〉「」＋－±×÷＝∞∴°′″＆☆★○●◎◇◆□■△▲▽▼※〒→←↑↓♯♭♪€‚„"),  # symbols and Latin-1, from the ROM's code map
    (345, "ˆŒ"),  # symbols and Latin-1, from the ROM's code map
    (350, "•˜™›œ¡¢£"),  # symbols and Latin-1, from the ROM's code map
    (359, "©®"),  # symbols and Latin-1, from the ROM's code map
    (364, "·¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ"),  # symbols and Latin-1, from the ROM's code map
    (390, "ØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõö"),  # symbols and Latin-1, from the ROM's code map
    (421, "øùúûüý"),
    (457, "　0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz−"),  # the ideographic space, the half-width digits and letters, and the minus sign
]


# The Japanese release's messages use a different code space.  Its sheet
# (extract/jp/.../Grp_Font.bin) draws the same glyphs as the USA sheet, but
# the message tables number the kana in Unicode order: the hiragana run is
# ぁ..ん from 0x5F and the katakana run is ァ..ヶ from 0xB2.  The Japanese
# ROM carries the same kind of DS-code -> glyph table the USA ROM has, and
# there it confirms the order: 0x3041 (ぁ) points at the glyph 0x5F and
# 0x30A1 (ァ) at the glyph 0xB2.  The ASCII runs and the symbol block are the
# same in both, so only the kana differ.  From 0x209 on the sheet holds a
# symbol block and then a kanji list that no ROM table names; those names are
# recorded in glyph_names.py as they are identified.
JP_HIRAGANA = "".join(chr(c) for c in range(0x3041, 0x3094))   # ぁ .. ん
JP_KATAKANA = "".join(chr(c) for c in range(0x30A1, 0x30F7))   # ァ .. ヶ


def load_charmap(version="usa"):
    """{glyph index: unicode char} for the whole known code space."""
    charmap = {}
    for start, chars in CHARMAP_RUNS:
        for n, ch in enumerate(chars):
            charmap[start + n] = ch
    if version == "jp":
        charmap = load_jp_charmap(charmap)
    charmap.update(GLYPH_NAMES)
    return charmap


def load_jp_charmap(charmap=None):
    """The Japanese code space, built on the shared parts of the USA one."""
    if charmap is None:
        charmap = load_charmap("usa")
    out = {code: ch for code, ch in charmap.items() if code < 0x5F or code >= 0x108}
    for n, ch in enumerate(JP_HIRAGANA):
        out[0x5F + n] = ch
    for n, ch in enumerate(JP_KATAKANA):
        out[0xB2 + n] = ch
    return out


# Only the backslash needs escaping, to allow it to represent an escape in text.
_ESCAPED = {"\\": "\\\\"}


def decode(codes, charmap):
    """SysCode[] -> escaped text. Stops at the first STR_END."""
    out = []
    for code in codes:
        if code == STR_END:
            break
        if code in CONTROL:
            out.append(CONTROL[code])
        elif code in charmap:
            ch = charmap[code]
            out.append(_ESCAPED.get(ch, ch))
        else:
            out.append(f"{{{code:04X}}}")
    return "".join(out)


def read_messages(version):
    """Yield (index, codes) for every entry of mestable.bin."""
    d = data_dir(version)
    table = (d / "mestable.bin").read_bytes()
    text = (d / "mestxt.bin").read_bytes()
    for i in range(len(table) // 8):
        offset, size = struct.unpack_from("<ii", table, i * 8)
        yield i, struct.unpack_from("<%dH" % (size // 2), text, offset)


MESTXT_COLUMNS = ("index", "text")
UNKNOWN_COLUMNS = ("glyph", "hex", "count", "first_msg", "first_msg_text")


def write_dump(path, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as f:
        f.write("\t".join(MESTXT_COLUMNS) + "\n")
        for index, text in rows:
            f.write(f"{index}\t{text}\n")


def cmd_dump(args):
    for version in args.versions:
        charmap = load_charmap(version)
        rows = []
        unknown = collections.Counter()
        samples = {}
        for index, codes in read_messages(version):
            text = decode(codes, charmap)
            rows.append((index, text))
            for code in codes:
                if code >= 0xFF00:  # control codes, STR_END included
                    continue
                # CONTROL also names the icon glyphs
                if code not in charmap and code not in CONTROL:
                    unknown[code] += 1
                    samples.setdefault(code, (index, text))

        d = out_dir(version)
        write_dump(d / "mestxt.tsv", rows)

        with (d / "unknown_glyphs.tsv").open("w", encoding="utf-8", newline="\n") as f:
            f.write("\t".join(UNKNOWN_COLUMNS) + "\n")
            for code, count in sorted(unknown.items()):
                index, text = samples[code]
                f.write(f"{code}\t0x{code:04X}\t{count}\t{index}\t{text}\n")

        print(f"{d.relative_to(REPO)}/mestxt.tsv: {len(rows)} messages")


def main():
    p = argparse.ArgumentParser(
        description=__doc__.split("\n")[1],
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    sub = p.add_subparsers(required=True)

    sp = sub.add_parser("dump", help="decode mestxt.bin into assets/<version>/text/")
    sp.add_argument(
        "-v",
        "--version",
        dest="versions",
        action="append",
        choices=VERSIONS,
        help="default: both",
    )
    sp.set_defaults(func=cmd_dump)

    args = p.parse_args()
    args.versions = args.versions or list(VERSIONS)
    sys.exit(args.func(args) or 0)


if __name__ == "__main__":
    main()
