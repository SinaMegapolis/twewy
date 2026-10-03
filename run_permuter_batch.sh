#!/bin/bash
# Reseed and search the ov039 functions closest to matching, in one go.
#
#   bash run_permuter_batch.sh "<seconds per function>" [jobs] [fn ...]
#
# With no function list it uses the current 95%+ imperfect set; functions given
# on the command line replace it. run_permuter_list.sh re-imports each function
# from the *current* source before searching, so a win on one function does not
# stale the others.
set -u

SECS="${1:-120}"
JOBS="${2:-8}"

# The near-miss set as of the last campaign. Stale by design is fine here -- the
# point of passing functions explicitly is to override it.
DEFAULT="func_ov039_0208a490 func_ov039_020855e0 func_ov039_02092d0c func_ov039_02092348 func_ov039_02093a30 func_ov039_0208d210 func_ov039_0208fee0 func_ov039_0208e9f8"
FNS="${*:3}"
FNS="${FNS:-$DEFAULT}"

export JOBS
exec bash "$(dirname "$0")/run_permuter_list.sh" "$SECS" $FNS
