#!/usr/bin/env bash

# It will detect "path/to/$RODINIA_ROOT_DIR/" w.r.t "path/to/$RODINIA_ROOT_DIR/scripts" dir
RODINIA_ROOT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd .. && pwd )"

export MY_SCRIPT_DIR="$RODINIA_ROOT_DIR/scripts"
export MY_VERIFICATION_DIR="$RODINIA_ROOT_DIR/data/verification_data/"
export MY_VERIFICATION_DUMP=1

RESULT_DIR="$RODINIA_ROOT_DIR/results"

# Check for the --delete-old-verification-data flag
# If this flags passed, delete old verification data folder
for arg in "$@"; do
    if [ "$arg" == "--delete-old-verification-data" ]; then
        echo "Deleting old verification data..."
        rm -Rf "$MY_VERIFICATION_DIR"
        break
    fi
done


# echo "MY_SCRIPT_DIR = $MY_SCRIPT_DIR" 
# echo "RODINIA_ROOT_DIR = $RODINIA_ROOT_DIR"

mkdir -p "$MY_VERIFICATION_DIR"

BM="$($MY_SCRIPT_DIR/rodinia_cuda_apps_list.sh)"

"$MY_SCRIPT_DIR/run_timed_common.sh" "cuda" "$BM" ./run_verify "$RESULT_DIR"