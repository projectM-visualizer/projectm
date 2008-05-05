#!/bin/sh
PATH_TO_QT = /Developer/Frameworks/.../
QT_VERSION = 4.3
APP = projectM-jack.app

mkdir ${APP}/Contents/Frameworks
cp -R ${PATH_TO_QT}/QtCore.framework projectM-jack.app/Contents/Frameworks
cp -R ${PATH_TO_QT}/QtGui.framework projectM-jack.app/Contents/Frameworks

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
	 projectM-jack.app/Contents/Frameworks/QtCore.framework/Versions/4.0/QtCore

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui \
	projectM-jack.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui

install_name_tool -change path/to/Qt/lib/QtCore.framework/Versions/4.0/QtCore
         @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore
         projectM-jack.app/Contents/MacOs/projectM-jack
install_name_tool -change path/to/qt/lib/QtGui.framework/Versions/4.0/QtGui
         @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui
         projectM-jack.app/Contents/MacOs/projectM-jack