#!/bin/sh

export TARGET_DIR=$1
export VERSION=$2

# Build targets
export GZ=ON
export BZ2=OFF

#export SOURCE=ON

# CG Enablement
export CG=ON

# FTGL Enablement
export FTGL=ON

# openmp Enablement
export OPENMP=ON

publish() {
OLDDIR=`pwd`

DIR=$1
echo "publishing ${DIR}..."
cd ${DIR}
svn-clean -f
cmake -DUSE_FTGL=${FTGL} -DCPACK_SOURCE_TGZ=${GZ} -DUSE_OPENMP=${OPENMP} -DCPACK_SOURCE_TBZ2=${BZ2} -DCPACK_SOURCE_Z=OFF -DCPACK_SOURCE_TZ=OFF -DUSE_CG=${ON} -DCMAKE_BUILD_TYPE=Release > ${TARGET_DIR}/projectm-publish.log 2>&1

make package_source 
cp *.gz -f ${TARGET_DIR}
#cp *.bz2 -f ${TARGET_DIR}
cd ${OLDDIR}
}

release() {
scp ${TARGET_DIR}/*.gz w1z7ard,projectm@frs.sourceforge.net:/home/frs/project/p/pr/projectm/${VERSION}
}

publish "libprojectM"

publish "../fonts"
publish "../presets"
publish "../presets_milkdrop_104"
publish "../presets_milkdrop_200"
publish "../presets_milkdrop"
publish "../presets_projectM"

publish "projectM-qt"
publish "projectM-test"
publish "projectM-libvisual"
publish "projectM-libvisual-alsa"
publish "projectM-pulseaudio"
publish "projectM-jack"
publish ".."

release

echo "[publisher] complete."
exit
