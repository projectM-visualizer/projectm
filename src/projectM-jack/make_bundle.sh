#!/bin/sh
### make_bund.sh A simple script to create a mac bundle ####
### Author: Carmelo Piccione (carmelo.piccione+projectM@gmail.com) ###

# Path to installed qt framework on your machine
PATH_TO_QT =  /Developer/SDKs/MacOSX10.4u.sdk/Library/Frameworks/

# the version of qt to bundle
QT_VERSION = 4.3

# The bundle top level directory
APP = projectM-jack.app

## [1] Create framework directory
mkdir -f ${APP}/Contents/Frameworks


## [2] Copy each qt framework into the bundle
cp -R ${PATH_TO_QT}/QtCore.framework ${APP}/Contents/Frameworks
cp -R ${PATH_TO_QT}/QtGui.framework ${APP}/Contents/Frameworks
cp -R ${PATH_TO_QT}/QtXml.framework ${APP}/Contents/Frameworks
cp -R ${PATH_TO_QT}/QtOpenGL.framework ${APP}/Contents/Frameworks


## [3] Change id of each qt framework to specialized one for this bundle
install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore \
	 ${APP}/Contents/Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui \
	${APP}/Contents/Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui

install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml \
	${APP}/Contents/Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml
	
install_name_tool -id @executable_path/../Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL \
	${APP}/Contents/Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL


## [4] Change reference ids in projectM-jack to the local referred to id's
install_name_tool -change ${PATH_TO_QT}/QtCore.framework/Versions/${QT_VERSION}/QtCore
         @executable_path/../Frameworks/QtCore.framework/Versions/${QT_VERSION}/QtCore
         ${APP}/Contents/MacOs/projectM-jack
		 
install_name_tool -change ${PATH_TO_QT}/QtGui.framework/Versions/${QT_VERSION}/QtGui
         @executable_path/../Frameworks/QtGui.framework/Versions/${QT_VERSION}/QtGui
         ${APP}/Contents/MacOs/projectM-jack
		 
		 
install_name_tool -change ${PATH_TO_QT}/QtXml.framework/Versions/${QT_VERSION}/QtXml
         @executable_path/../Frameworks/QtXml.framework/Versions/${QT_VERSION}/QtXml
         ${APP}/Contents/MacOs/projectM-jack
		 
install_name_tool -change ${PATH_TO_QT}/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL
         @executable_path/../Frameworks/QtOpenGL.framework/Versions/${QT_VERSION}/QtOpenGL
         ${APP}/Contents/MacOs/projectM-jack
		 