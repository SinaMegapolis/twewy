#!/bin/bash
# Run decomp-permuter over a list of ov039 functions.
#
#   bash run_permuter_list.sh "<seconds each>" "<fn> <fn> ..."
#
# Jobs per function comes from $JOBS (default 8).
#
# Each function is re-imported from the *current* source first, so a search
# always starts from the best checkpoint found so far rather than an older one.
# Scores are the permuter's own byte-distance metric (lower is better), not
# objdiff's match percentage; the two are not comparable.
set -u

SECS="${1:-120}"
shift
JOBS="${JOBS:-8}"
FNS="$*"

PERM=/mnt/c/Users/COMIRAN/AppData/Local/Temp/opencode/decomp-permuter
cd /mnt/e/Git/twewy || exit 1

# The permuter hands candidates to compile.sh through Python's tempfile, and
# mwccarm.exe is a native Windows binary that cannot read WSL's /tmp.
mkdir -p permtmp
TMPDIR="$PWD/permtmp"
export TMPDIR TMP="$TMPDIR" TEMP="$TMPDIR"

for FN in $FNS; do
  echo "############ $FN ############"
  python3 tools/perm_setup.py "$FN" >/dev/null 2>&1 || {
    echo "  perm_setup failed for $FN"; continue; }

  base=$(python3 "$PERM/permuter.py" "nonmatchings/$FN" --debug 2>&1 |
         grep -o 'base score = [0-9]*' | head -1)
  echo "  baseline: ${base:-unavailable}"

  timeout "$SECS" python3 "$PERM/permuter.py" \
    -j "$JOBS" --best-only --stop-on-zero "nonmatchings/$FN" >/dev/null 2>&1

  best=$(ls -d "nonmatchings/$FN"/output-* 2>/dev/null |
         while read -r d; do printf "%s %s\n" "$(cat "$d/score.txt" 2>/dev/null || echo 999999)" "$d"; done |
         sort -n | head -1)
  echo "  best:     ${best:-no candidates}"
done
