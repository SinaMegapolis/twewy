#!/usr/bin/env python3
"""Rebuild ov039 and actually verify the USA ROM.

WHY THIS EXISTS
---------------
`ninja` on its own does not re-link the ROM after a source edit.  The `delink`
edge in build.ninja lists only config files and dsd.exe as inputs -- the
compiled `build/usa/src/**/*.o` objects are not among them -- so ninja treats
the delinked objects as up to date, skips the delink, and goes straight to the
sha1 check.  The sha1 then passes, because it is hashing the *previous* ROM.

That makes `ninja` reporting `twewy_usa.nds: OK` meaningless after an edit.
Confirmed directly: writing a sentinel value into a table inside the claimed
`.rodata` range left the ROM byte-identical, and the sentinel appeared nowhere
in build/usa/build/arm9_ov039.bin -- the object had been rebuilt but never
relinked.

So the delinked objects must be invalidated by hand before ninja will redo the
work.  Removing the module's gap objects is enough to dirty the delink edge.

    python tools/verify_rom.py [module]        # default: ov039
"""
import glob
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ROM = ROOT / "build" / "usa" / "twewy_usa.nds"
DELINKS = ROOT / "build" / "usa" / "delinks"


def run(cmd, **kw):
    return subprocess.run(cmd, cwd=ROOT, **kw)


def main():
    module = sys.argv[1] if len(sys.argv) > 1 else "ov039"

    # Force the whole delink, not just the overlay-level objects.
    #
    # The old glob was `DELINKS / f"*{module}*.o"` -- non-recursive, so it only
    # matched the handful of overlay-level objects directly under delinks/ (7 for
    # ov039) and left all ~800 per-translation-unit objects untouched, e.g.
    # delinks/src/Debug/Sugata/TinPinSlammer/OtuFieldAccess.o. Those are exactly
    # the ones ninja treats as current, so the delink was skipped and the sha1
    # hashed a stale ROM. That is the very failure this script exists to catch,
    # and the "ROM is newer than the objects" check could not see it because the
    # stale objects did not change either.
    stale = glob.glob(str(DELINKS / "**" / "*.o"), recursive=True)
    for p in stale:
        os.remove(p)
    print(f"invalidated {len(stale)} delinked object(s) for {module}")

    r = run(["ninja"], capture_output=True, text=True)
    sys.stderr.write(r.stdout + r.stderr)
    if r.returncode != 0:
        sys.exit("ninja failed")

    # The sha1 alone cannot distinguish "rebuilt and correct" from "never
    # rebuilt", so confirm the ROM is newer than what it was built from.
    rebuilt = [Path(p) for p in glob.glob(str(DELINKS / f"*{module}*.o"))]
    if rebuilt and ROM.stat().st_mtime < max(p.stat().st_mtime for p in rebuilt):
        newest = max(rebuilt, key=lambda p: p.stat().st_mtime)
        sys.exit(
            f"ROM is older than {newest.name} -- the link did not run, so the "
            f"sha1 above hashed a stale file"
        )
    if rebuilt:
        print(f"ROM is newer than {len(rebuilt)} relinked {module} object(s): link confirmed.")


if __name__ == "__main__":
    main()
