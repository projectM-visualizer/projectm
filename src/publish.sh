#!/bin/sh

TARGET_DIR = $1

publish {

DIR=$1

cd ${DIR}
svn-clean -f
cmake -DUSE_FTGL=ON -DUSE-CG=ON -DCMAKE_BUILD_TYPE=Release
make package_source
cp *.gz -f ${TARGET_DIR}
}


publish "libprojectM"
exit
