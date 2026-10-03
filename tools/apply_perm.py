#!/usr/bin/env python3
"""Cache the scene pointer as a u8* in func_ov039_020827d0.

decomp-permuter's best candidate for this function differs from the committed
source in exactly one structural way: it copies `scene` into a `u8 *` local and
adds the stage offset to that, rather than re-deriving the byte pointer from the
typed `TinPinSlammer_Scene *` at each of its nine uses.

That is the same "do not let the compiler recompute from a different type"
trick as the cached-pointer removal elsewhere in this overlay, inverted: here
the *base* is cached and the offset re-added, so mwcc keeps one byte pointer in
a callee-saved register instead of recomputing a typed one per call.

    python tools/apply_perm.py func_ov039_020827d0
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main():
    func = sys.argv[1]
    unit = sys.argv[2] if len(sys.argv) > 2 else "src/Debug/Sugata/TinPinSlammer/OtuScene"
    src = ROOT / f"{unit}.c"
    text = src.read_text(encoding="latin1")

    m = re.search(rf"\nvoid {re.escape(func)}\(.*?\n\}}", text, re.S)
    if not m:
        sys.exit(f"{func} not found in {src}")
    body = m.group(0)

    if "u8* base" in body:
        print("already applied")
        return

    # Insert the local as the first statement, then route every OTU_STAGE()
    # through it.
    decl = "\n    // decomp-permuter's finding: cache the scene as a byte pointer and add\n" \
           "    // the stage offset to that, so mwcc keeps one base in a register instead\n" \
           "    // of re-deriving a typed pointer at each of the nine uses below.\n" \
           "    u8* base = (u8*)scene;\n"
    body = body.replace("{\n", "{" + decl, 1)
    body = body.replace("OTU_STAGE(scene)", "((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET))")

    src.write_text(text[:m.start()] + body + text[m.end():], encoding="latin1")
    print(f"{func}: applied the byte-pointer base cache")


if __name__ == "__main__":
    main()
