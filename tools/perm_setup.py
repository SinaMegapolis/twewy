#!/usr/bin/env python3
"""Set up a decomp-permuter directory for one ov039 function.

`permuter_import.py` targets the upstream Linux layout (it runs the import
through `./wibo` and then shells out to `arm-none-eabi-as`), so it does not
finish on this box.  This does the same job natively:

  nonmatchings/<fn>/base.c     the real TU with every function except <fn>
                               removed, so struct layouts, file-local statics
                               and macros are exactly what the real build sees
  nonmatchings/<fn>/target.s   via tools/perm_target.py
  nonmatchings/<fn>/target.o   assembled with arm-none-eabi-as
  nonmatchings/<fn>/compile.sh via nonmatchings/compile_arm32.sh
  nonmatchings/<fn>/settings.toml

    python tools/perm_setup.py <function> [unit]

Run the permuter from the repo root:

    python3 <permuter>/permuter.py -j 8 --best-only --stop-on-zero \\
        nonmatchings/<fn>
"""

import shutil
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from perm_harness import find_function, strip_other_functions  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent

# Resolved from PATH, with a Windows fallback. This script also runs inside WSL
# (the permuter needs a Linux environment), where the msys64 path does not
# exist -- hardcoding it made every batch run after the first function fail.
AS = shutil.which("arm-none-eabi-as") or r"C:\msys64\mingw64\bin\arm-none-eabi-as.exe"

UNITS = {
    "OtuScene": "src/Debug/Sugata/TinPinSlammer/OtuScene",
    "OtuSceneEntry": "src/Debug/Sugata/TinPinSlammer/OtuSceneEntry",
    "OtuText": "src/Debug/Sugata/TinPinSlammer/OtuText",
    "OtuVBlank": "src/Debug/Sugata/TinPinSlammer/OtuVBlank",
    "OtuGxInit": "src/Debug/Sugata/TinPinSlammer/OtuGxInit",
    "OtuScoreRow": "src/Debug/Sugata/TinPinSlammer/OtuScoreRow",
}


def owning_unit(func):
    """Which .c file defines `func`, by reading the build's asm tree index."""
    for name, unit in UNITS.items():
        src = ROOT / f"{unit}.c"
        if not src.exists():
            continue
        # latin1, not shift_jis: the permuter preprocesses with cpp, which treats
        # the bytes as opaque, and a decode can fail on a stray byte in a comment.
        text = src.read_text(encoding="latin1")
        if find_function(text, func):
            return unit
    return None


def main():
    func = sys.argv[1]
    unit = sys.argv[2] if len(sys.argv) > 2 else owning_unit(func)
    if not unit:
        sys.exit(f"could not find a defining unit for {func}")

    src = ROOT / f"{unit}.c"
    d = ROOT / "nonmatchings" / func
    d.mkdir(parents=True, exist_ok=True)

    # Confirm the unit really defines this function before preprocessing.
    probe = src.read_text(encoding="latin1")
    if find_function(probe, func) is None:
        sys.exit(f"{func} is not defined in {src}")

    # The permuter re-runs `cpp -P -nostdinc` over base.c, so the file must
    # already be fully pre-expanded: -nostdinc means the project's own #include
    # lines cannot be resolved at that point.  Preprocess with the real include
    # paths first, then strip the other functions.  -D'__attribute__(x)='
    # neutralises attribute syntax the project uses but the preprocessor does not
    # need to understand, matching upstream USAGE.md.
    cpp = shutil.which("cpp") or r"C:\msys64\mingw64\bin\cpp.exe"
    pre = subprocess.run(
        [cpp, "-P",
         "-I", "include", "-I", "libs/include", "-I", "libs/c/include",
         "-I", "libs/cpp/include", "-I", "libs/nitro/include",
         "-I", "libs/runtime/include",
         "-D__attribute__(x)=", "-DREGION_USA",
         str(src)],
        cwd=ROOT, capture_output=True, text=True, encoding="latin1",
    )
    if pre.returncode != 0:
        sys.exit(f"cpp failed:\n{pre.stderr[:800]}")
    stripped = strip_other_functions(pre.stdout, func)
    span = find_function(stripped, func)
    if span is None:
        sys.exit(f"{func} vanished from the stripped preprocessed source")

    (d / "base.c").write_text(stripped, encoding="latin1")

    shutil.copy(ROOT / "nonmatchings/compile_arm32.sh", d / "compile.sh")
    (d / "compile.sh").chmod(0o755)
    (d / "settings.toml").write_text(
        f'func_name = "{func}"\ncompiler_type = "mwcc"\n'
    )

    subprocess.run([sys.executable, str(ROOT / "tools/perm_target.py"), func],
                   cwd=ROOT, check=True)
    subprocess.run(
        [AS, "-march=armv5te", "-mfloat-abi=soft",
         str(d / "target.s"), "-o", str(d / "target.o")],
        cwd=ROOT, check=True,
    )

    n = len(stripped)
    print(f"{func}: base.c {n} bytes, target.o built, compile.sh + settings.toml in place")


if __name__ == "__main__":
    main()
