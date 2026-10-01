#!/bin/bash
# Reseed and search several ov039 functions in one go.
#
#   bash run_permuter_batch.sh "<seconds per function>"
#
# Every function in the list is re-imported from the *current* source first, so
# each search starts from the best checkpoint found so far rather than from an
# older one, and a win on one function does not stale the others.
set -u

SECS="${1:-120}"
JOBS="${2:-8}"
FNS="func_ov039_020827d0 func_ov039_020831d8 func_ov039_02082774 func_ov039_02082e98 func_ov039_02082ff8 func_ov039_02083ed4 func_ov039_02083cbc func_ov039_020832c0"

for FN in $FNS; do
  echo "############ $FN ############"
  python3 tools/perm_setup.py "$FN" 2>&1 | tail -1
  mkdir -p permtmp
  export TMPDIR="$PWD/permtmp" TMP="$PWD/permtmp" TEMP="$PWD/permtmp"
  timeout "$SECS" python3 \
    /mnt/c/Users/COMIRAN/AppData/Local/Temp/opencode/decomp-permuter/permuter.py \
    -j "$JOBS" --best-only --stop-on-zero "nonmatchings/$FN" 2>&1 | tail -2
  ls -d "nonmatchings/$FN"/output-* 2>/dev/null \
    | while read -r d; do printf "  %s  " "$d"; cat "$d/score.txt" 2>/dev/null || echo '?'; done
done
