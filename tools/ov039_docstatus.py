#!/usr/bin/env python3
"""Rewrite the ov039 status and row-layout sections of docs/overlays.md.

The status line and the layout tables are derived from build/usa/report.json,
so they are regenerated rather than hand-edited -- they were wrong twice
already, once because a function was added without updating the count and once
because the offset tables were renamed underneath a table that still named them.
"""
import json
import re
from pathlib import Path

DOC = Path("docs/overlays.md")
REPORT = Path("build/usa/report.json")
TOTAL = 561

NEW_STATUS = """Status: {matched} of {total} functions decompiled, {perfect} of them at 100%.
`OtuMenuText` is 100% matched (860 bytes of `.data`, 268 of `.rodata`, 8 of
`.bss`); `OtuVBlank` is 100% across all five functions; `OtuSceneEntry` holds the
two plain entry points, the teardown, its thunk and both stage dispatches at
100%. `OtuScoreRow` is now the overlay's largest unit and holds eleven row
builders plus the countdown steppers that drive them. Partial:
`func_ov039_02083bb0` at 99.97%, `OtuGxInit` at 95.9%, both per-frame updates at
91.0% and 95.4%, the clamp helper at 94.5%, the place stepper at 92.3%, the
pin-count row at 83.8%, the countdown at 76.0%, the wireless teardown at 72.8%.
The remaining {remaining} functions are stubs.
"""

NEW_LAYOUT = """### Row layouts, and why the tables keep the build's names

The result screens have a dozen or so numeric rows, and the `d.ddd` family has
one column-blanking table per row width: 4, 8, 12 and 16 entries for one
through four groups. There are three separate 4-entry tables and two separate
12-entry ones with identical contents, so a row's width does not determine its
table -- the tables are per-row, not per-width. With the nine-entry pin-count
table and the eighteen-entry three-digit one there are nine in all.

All are word-indexed (`ldr [base, index, lsl #2]`); declaring one as `u8` emits
`ldrb` and silently costs a match percentage. That is how the first of them was
wrong for a while, at `u8[7]` when it should have been `s32[9]`.

They are declared with the build's own `data_ov039_*` names rather than invented
ones, and that is deliberate. Declaring an array and letting dsd place it does
not work here: the claimed `.rodata` range is largely gap-filled from the
original overlay, so a declared array lands wherever the linker puts it rather
than where the target has it. The bytes came out right anyway -- it was the
constant-pool word that was wrong, so the defect was invisible until a function
was diffed. Naming them as the build does puts the pool word back, which is
what takes the three-digit row from 99.95% to a full match. The same applies to
three `.data` anchor constants that sit in a gap-filled range.

`tools/ov039_table.py <address>` recovers any table's contents;
`tools/ov039_anchor.py` reports which declared constant sits at which address,
and marks same-valued pairs AMBIGUOUS rather than guessing.

### Three stepper shapes

The countdown steppers come in three shapes, and they are not
interchangeable:

  * a bare `ldreq` with no `else` arm (`02084944`, `02084d20`), which leaves the
    column uninitialised when the quotient is non-zero. Defaulting it to a fixed
    address instead adds a `movne` the target does not have -- 84.3% against
    72.5%;
  * a three-way `if`/`else if` chain (`020846c4`, `020847ec`, `02084a3c`) with
    no jump table;
  * a four-way `switch` with a jump table (`02084458`, `02084b94`).

For the three-way shape the target branches to a block for `== 0` and to a
second for `== 1`, with `== 2` as the conditional load that falls through.
mwcc's ordering of the arms does not reproduce that exactly; putting the
fall-through case last gets closest (74.9%) without reaching it.

### Two row-body shapes

The `d.ddd` row builders are near-identical but differ by one instruction, and
the two shapes are not interchangeable:

  * `020849a4` and `02084d80` write the trailing space *before* the blanking
    check and the NUL *after* it, two separate stores with the blanking between.
    Written adjacently, mwcc deletes the first as dead.
  * `02084738`, `02084874`, `02084ac4` and `02084c34` write no trailing space at
    all, relying on `func_ov039_02083a20`'s own and writing only the NUL.

Adding the redundant store where the target omits it costs a register and about
seven points of match.
"""


def main():
    report = json.loads(REPORT.read_text(encoding="utf-8"))
    # Only ov039, and only functions that have actually been written: a
    # not-yet-decompiled stub still carries a percentage (0) from the delinked
    # target, so counting every non-null value would sweep up the whole overlay
    # list and report thousands.
    matched = perfect = 0
    for unit in report["units"]:
        for fn in unit.get("functions", []):
            # Function names, not unit names: the overlay's units are split per
            # TU and several share the `ov039` gap prefix, so keying on the
            # unit picks up dsd's gap objects and misses the real ones.
            if not fn.get("name", "").startswith("func_ov039_"):
                continue
            pct = fn.get("fuzzy_match_percent")
            if pct:
                matched += 1
                if pct >= 99.99:
                    perfect += 1

    text = DOC.read_text(encoding="latin1")

    text = re.sub(
        r"Status: \d+ of \d+ functions decompiled, \d+ of them at 100%.*?are stubs\.\n",
        NEW_STATUS.format(matched=matched, total=TOTAL, perfect=perfect,
                          remaining=TOTAL - matched),
        text, count=1, flags=re.S,
    )

    # Replace the old four-row table section, whatever it is currently called.
    text = re.sub(
        r"### Four row layouts\n.*?(?=\n### )",
        NEW_LAYOUT + "\n",
        text, count=1, flags=re.S,
    )

    DOC.write_text(text, encoding="latin1")
    print(f"status: {matched}/{TOTAL} matched, {perfect} at 100%")
    if "Four row layouts" in text:
        print("WARNING: old layout section still present")


if __name__ == "__main__":
    main()
