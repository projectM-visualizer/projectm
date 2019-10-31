#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

# Options
repo=projectm
SRC=/d/GIT/$repo
DEST=$SRC/projectm
###

echo "Building EyeTune"
cd $SRC/src && msbuild EyeTune.WindowsUniversal.Application.sln /m /p:Configuration=RELEASE /p:Platform=x64
 