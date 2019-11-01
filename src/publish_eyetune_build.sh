#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

# Options
repo=projectm
SRC=/c/GIT/$repo

echo "Building EyeTune"
cd $SRC/src 
MSBuild.exe "$SRC/src/EyeTune.WindowsUniversal.Application.sln" //p:Configuration=Release
 