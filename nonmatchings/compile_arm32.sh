#!/usr/bin/env bash
# Compile command for decomp-permuter on this machine.
#
# Invoked as ./compile.sh input.c -o output.o.  Two Windows-specific deviations
# from the upstream arm32_compile_example.sh:
#
#   * no `wine` -- the repo's mwccarm.exe is already a native Windows binary, so
#     wibo/wine is not needed and would only slow the loop down;
#   * the flag set is copied from the unit's actual ninja command line rather
#     than the example's, which differs in several places that change codegen
#     (-inline noauto, -w off, -sym on, -str noreuse, -msgstyle gcc, and the
#     full include list). Scoring against a differently-configured build would
#     optimise for the wrong target.
#
# mwccarm.exe is a native Windows PE, so it is invoked directly; the path is
# absolute so the permuter's working directory does not matter.

# This script runs inside WSL.  mwccarm.exe is a native Windows PE that locates
# its own install directory from argv[0], and WSL interop hands it a /mnt path
# it cannot resolve -- hence "Cannot find my executable".  Invoking it through
# `cmd.exe /c` makes argv[0] a real Windows path, which is the whole fix.  Input
# and output paths are translated to Windows form for the same reason.
WORKING_DIR="/mnt/e/Git/twewy"
CC_WIN='E:\Git\twewy\tools\mwccarm\2.0\sp1p5\mwccarm.exe'

winpath() {
  # /mnt/e/... -> E:\...
  local p="$1"
  echo "$p" | sed -e 's|^/mnt/\([a-z]\)/|\1:\\|' -e 's|/|\\\\|g'
}

C_NAME="$1"
O_NAME="$3"

cd "$WORKING_DIR" || exit 1

C_WIN="$(winpath "$(readlink -f "$C_NAME")")"
O_WIN="$(winpath "$(readlink -f "$O_NAME")")"

cmd.exe /c "$CC_WIN" -O4,p -enum int -char signed -proc arm946e -gccext,on \
    -fp soft -inline noauto -RTTI off -interworking -w off -sym on -gccinc \
    -nolink -msgstyle gcc -enc SJIS -ipa file -str noreuse \
    -Cpp_exceptions off \
    -i include -i libs/include -i libs/c/include -i libs/cpp/include \
    -i libs/nitro/include -i libs/runtime/include \
    -lang=c99 -d REGION_USA -c "$C_WIN" -o "$O_WIN"
