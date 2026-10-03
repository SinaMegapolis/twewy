#!/usr/bin/env python3
"""Move a contiguous run of top-level functions to the end of a C file.

Clang-format and pre-commit hooks rewrap as they go, so block surgery by hand
tends to leave the file out of address order. This does it mechanically.

    python tools/moveblock.py <file.c> <first-fn> <last-fn>
"""
import re
import sys


def main():
    path, first, last = sys.argv[1:4]
    src = open(path, encoding="latin1").read()

    # Include any doc comment immediately above the first function.
    start = src.index(first)
    m = None
    for cand in re.finditer(r"/\*\*(?:[^*]|\*(?!/))*\*/\s*\n(?=(?:static )?[\w\* ]*" + re.escape(first) + r"\()", src):
        if cand.start() < start:
            m = cand
    if m:
        start = m.start()

    # End at the closing brace of the last function, at column 0.
    li = src.index(last)
    end = src.index("\n}\n", li) + 3

    block = src[start:end]
    rest = src[:start] + src[end:]
    # Re-attach: drop a blank line the removal may have doubled up.
    rest = rest.replace("\n\n\n", "\n\n")
    open(path, "w", encoding="latin1").write(rest.rstrip("\n") + "\n\n" + block.lstrip("\n"))
    print(f"moved {first}..{last} ({len(block)} bytes) to end of {path}")


if __name__ == "__main__":
    main()
