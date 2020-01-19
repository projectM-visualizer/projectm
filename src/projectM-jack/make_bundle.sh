#!/bin/sh
### make_bund.sh A simple script to create a mac bundle ####
### Author: Carmelo Piccione (carmelo.piccione+projectM@gmail.com) ###

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

cp projectM-jack ${APP}/Contents/MAcOs/
cp -f ${PROJECTM_QT_BUILD_PATH} ${APP}/Contents/Plugins/libprojectM-qt.dylib

## [2] Copy each qt framework into the bundle
cp -Rf ${PATH_TO_QT}/QtCore.framework ${APP}/Contents/Frameworks
cp -Rf ${PATH_TO_QT}/QtGui.framework ${APP}/Contents/Frameworks
cp -Rf ${PATH_TO_QT}/QtXml.framework ${APP}/Contents/Frameworks
cp -Rf ${PATH_TO_QT}/QtOpenGL.framework ${APP}/Contents/Frameworks


## [3] Change id of each qt framework to specialized one for this bundle
install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore ${APP}/Contents/Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui ${APP}/Contents/Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui

install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml ${APP}/Contents/Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml
	
install_name_tool -id @executable_path/../Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL ${APP}/Contents/Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL
install_name_tool -id @executable_path/../Plugins/libprojectM-qt.dylib ${APP}/Contents/Plugins/libprojectM-qt.dylib


## [4] Change reference ids in projectM-jack to the local referred to id's
install_name_tool -change QtCore.framework/Versions/${QT_VERSION}/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore ${APP}/Contents/MacOs/projectM-jack

install_name_tool -change QtGui.framework/Versions/${QT_VERSION}/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui ${APP}/Contents/MacOs/projectM-jack
		 
install_name_tool -change QtXml.framework/Versions/${QT_VERSION}/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml ${APP}/Contents/MacOs/projectM-jack
		 
install_name_tool -change QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL ${APP}/Contents/MacOs/projectM-jack 

install_name_tool -change ${PROJECTM_QT_BUILD_PATH} @executable_path/../Plugins/libprojectM-qt.dylib ${APP}/Contents/MacOs/projectM-jack 

# [4b] Change projectM-qt library reference to bundle version

## [5] Change references from one qt lib to another qt lib from system to local		 
install_name_tool -change QtCore.framework/Versions/${QT_VERSION}/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore ${APP}/Contents/Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui		 
install_name_tool -change QtCore.framework/Versions/${QT_VERSION}/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore ${APP}/Contents/Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml		 

install_name_tool -change QtCore.framework/Versions/${QT_VERSION}/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore ${APP}/Contents/Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL		 
install_name_tool -change QtGui.framework/Versions/${QT_VERSION}/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui ${APP}/Contents/Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL

install_name_tool -change QtCore.framework/Versions/${QT_VERSION}/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore ${APP}/Contents/Plugins/libprojectM-qt.dylib
install_name_tool -change QtGui.framework/Versions/${QT_VERSION}/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui ${APP}/Contents/Plugins/libprojectM-qt.dylib
install_name_tool -change QtXml.framework/Versions/${QT_VERSION}/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml ${APP}/Contents/Plugins/libprojectM-qt.dylib
install_name_tool -change QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL ${APP}/Contents/Plugins/libprojectM-qt.dylib
