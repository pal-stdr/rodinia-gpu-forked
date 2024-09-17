#!/usr/bin/env bash

DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

BM="$($DIR/rodinia_cuda_apps_list.sh)"

"$DIR/run_timed_common.sh" "cuda" "$BM" ./run

