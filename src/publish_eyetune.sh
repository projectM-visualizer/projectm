#!/bin/bash

# Options
repo=projectm
SRC=/c/GIT/$repo

# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}


build_logs=$SRC/build_logs
mkdir -p $build_logs

if test ${GCCBUILD+defined}; then
echo "GCC BUILD"
filename=$build_logs/projectm-gcc-$(timestamp).txt
else
echo "MSVC BUILD"
filename=$build_logs/projectm-msvc-$(timestamp).txt
fi

sh -x publish_eyetune_build.sh \
2>&1 \
| tee -a $filename

echo "done"
