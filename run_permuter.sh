#!/bin/bash
# Run decomp-permuter for one ov039 function, from inside WSL.
#
# The permuter needs an ARM assembler, an ARM objdump and a C preprocessor.
# WSL's Ubuntu has the first two and python3 already; this installs gcc (for
# cpp) if missing, then runs the permuter against nonmatchings/<fn>.
#
#   bash run_permuter.sh <function> [jobs] [seconds]
set -u

FN="$1"
JOBS="${2:-8}"
SECS="${3:-120}"
PERM=/mnt/c/Users/COMIRAN/AppData/Local/Temp/opencode/decomp-permuter
REPO=/mnt/e/Git/twewy

# The permuter hands its candidate sources to compile.sh through Python's
# tempfile, and mwccarm.exe is a native Windows binary that cannot read WSL's
# /tmp.  Point TMPDIR at a path both can see, so the interop boundary is crossed
# once, in the path, rather than per file.
mkdir -p "$REPO/permtmp"
export TMPDIR="$REPO/permtmp"
export TMP="$TMPDIR"
export TEMP="$TMPDIR"

if ! command -v cpp >/dev/null 2>&1; then
  echo "installing gcc for cpp..."
  (apt-get install -y --no-install-recommends gcc >/dev/null 2>&1) || \
    (sudo apt-get install -y --no-install-recommends gcc >/dev/null 2>&1) || {
      echo "could not install gcc; run: sudo apt-get install -y gcc"; exit 1; }
fi

cd "$REPO" || exit 1
echo "=== baseline ($FN) ==="
python3 "$PERM/permuter.py" "nonmatchings/$FN" --debug 2>&1 | tail -3

echo "=== searching (${JOBS} jobs, ${SECS}s) ==="
timeout "$SECS" python3 "$PERM/permuter.py" -j "$JOBS" --best-only --stop-on-zero \
  "nonmatchings/$FN" 2>&1 | tail -30

echo "=== results ==="
ls -d "nonmatchings/$FN"/output-* 2>/dev/null | while read -r d; do
  printf "%s  " "$d"
  cat "$d/score.txt" 2>/dev/null || echo "?"
done
