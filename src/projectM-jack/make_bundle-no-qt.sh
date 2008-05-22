#!/bin/sh
### make_bundle-no-qt.sh A simple script to create a mac bundle without qt
### included in the bundle (depends on standard framework paths instead)
### Author: Carmelo Piccione (carmelo.piccione+projectM@gmail.com) 

# Path to installed qt framework on your machine
PATH_TO_QT=/Developer/SDKs/MacOSX10.4u.sdk/Library/Frameworks/
BUILD_PATH=/Users/struktured/projects/projectm_trunk/src
PROJECTM_QT_BUILD_PATH=${BUILD_PATH}/projectM-qt/libprojectM-qt.1.dylib

# the version of qt to bundle
QT_VERSION=4

# The bundle top level directory
APP=projectM-jack.app

## [0] Clean
rm -rf ${APP}

## [1] Create framework directory
mkdir ${APP}
mkdir ${APP}/Contents
mkdir ${APP}/Contents/Frameworks
mkdir ${APP}/Contents/Plugins
mkdir ${APP}/Contents/MacOs
mkdir ${APP}/Contents/Resources
mkdir ${APP}/Contents/Resources/fonts
mkdir ${APP}/Contents/Resources/presets

cp projectM-jack ${APP}/Contents/MacOs/
cp -f ${PROJECTM_QT_BUILD_PATH} ${APP}/Contents/Plugins/libprojectM-qt.dylib
cp -Rf ../../fonts/*.ttf ${APP}/Contents/Resources/fonts
cp -Rf ../../presets_projectM/*.prjm ${APP}/Contents/Resources/presets
cp -Rf ../../presets_projectM/*.milk ${APP}/Contents/Resources/presets
cp -f ${BUILD_PATH}/projectM-engine/config.inp ${APP}/Contents/Resources

## [3] Change id of each qt framework to specialized one for this bundle
	
install_name_tool -id @executable_path/../Plugins/libprojectM-qt.dylib ${APP}/Contents/Plugins/libprojectM-qt.dylib

install_name_tool -change ${PROJECTM_QT_BUILD_PATH} @executable_path/../Plugins/libprojectM-qt.dylib ${APP}/Contents/MacOs/projectM-jack 

