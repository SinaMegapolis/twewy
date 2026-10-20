#!/usr/bin/env python3
"""
Shows the Japanese sentences that use a message code.

The Japanese release's messages are Japanese, so a sentence names the kanji
it contains: "ウルサ" says that the katakana at 0x100 is ル, and the
kanji read the same way.  This tool prints the messages that use a code
(most frequent first), so a kanji can be read out of them.

The USA release kept a few Japanese strings, and it numbers the kanji the
same way the Japanese release does, so a kanji named in one release is named
in the other.

Usage:
  python tools/asset/kanjihunt.py                 # the most common kanji codes
  python tools/asset/kanjihunt.py 0x02BB          # the sentences for one code
  python tools/asset/kanjihunt.py --all 0x02BB    # every message that uses it
"""

import argparse
import collections
import struct
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
KANJI_LO = 0x209


def read_messages(folder, charmap):
    """{index: text} for every message in a region's mestxt.bin."""
    import mesdump

    table = (folder / "mestable.bin").read_bytes()
    text = (folder / "mestxt.bin").read_bytes()
    out = {}
    for index in range(len(table) // 8):
        offset, size = struct.unpack_from("<ii", table, index * 8)
        codes = struct.unpack_from("<%dH" % (size // 2), text, offset)
        pieces = []
        for code in codes:
            if code == 0xFFFF:
                break
            pieces.append(mesdump.CONTROL.get(code) or charmap.get(code, "[%04X]" % code))
        out[index] = "".join(pieces)
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[1])
    parser.add_argument("code", nargs="?", type=lambda s: int(s, 0),
                        help="the message code to look for")
    parser.add_argument("--all", action="store_true",
                        help="show every message that uses the code")
    parser.add_argument("--count", type=int, default=12,
                        help="how many codes to list (default 12)")
    parser.add_argument("--version", choices=("usa", "jp"), default="jp")
    args = parser.parse_args()

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import mesdump

    folder = REPO / "extract" / args.version / "files" / "Apl_Fuk"
    if not folder.exists():
        print("no %s data at %s" % (args.version, folder))
        return 1

    charmap = mesdump.load_charmap(args.version)
    messages = read_messages(folder, charmap)

    if args.code is None:
        counts = collections.Counter()
        for text in messages.values():
            for code in range(KANJI_LO, 0x1000):
                if "[%04X]" % code in text:
                    counts[code] += 1
        print("the codes the messages use that the map does not name:")
        for code, n in counts.most_common(args.count):
            print("  0x%04X  %d messages" % (code, n))
        return 0

    code = args.code
    print("messages that use 0x%04X:" % code)
    shown = 0
    for index, text in sorted(messages.items()):
        if ("[%04X]" % code) not in text:
            continue
        print("  %5d  %s" % (index, text))
        shown += 1
        if not args.all and shown >= args.count:
            break
    if not shown:
        print("  (none)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
