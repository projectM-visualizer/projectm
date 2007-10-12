/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualQuickTime.cpp
 *
 */

/***************************************************************************

Copyright (c) 2004-2007 Heiko Wichmann (http://www.imagomat.de/vizkit)


This software is provided 'as-is', without any expressed or implied warranty. 
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#include "VisualQuickTime.h"
#include "VisualErrorHandling.h"
#include "VisualDataStore.h"



using namespace VizKit;


OSErr VisualQuickTime::initialize() {
	OSErr err = noErr;
#if TARGET_OS_WIN
	char errLog[256];
	err = InitializeQTML(0);
	if (err != noErr) {
		sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return err;
	}
	err = EnterMovies();
	if (err != noErr) {
		sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return err;
	}
#endif
	return err;
}


OSErr VisualQuickTime::terminate() {
	OSErr err = noErr;
#if TARGET_OS_WIN
	ExitMovies();
	TerminateQTML();
#endif
	return err;
}


OSStatus VisualQuickTime::getGraphicsImporterForFile(const VisualFile& aFile, GraphicsImportComponent& importComponent, OSType fileType) {
	OSErr err = noErr;
	OSStatus osStatus = noErr;
	char errLog[256];
	FSSpec fsSpec;
	importComponent = NULL;
	
	osStatus = VisualFile::setFSSpecByFileRef(aFile, fsSpec);
	if (osStatus != noErr) {
		sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", osStatus, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return osStatus;
	}

	if (fileType != '    ') {
		osStatus = VisualQuickTime::getGraphicsImporterComponentForFileType(fileType, importComponent);
		if (osStatus != noErr) {
			sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", osStatus, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			return osStatus;
		}
		ComponentResult compErr = GraphicsImportSetDataFile(importComponent, &fsSpec);
		if (compErr != noErr) {
			sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			return static_cast<OSStatus>(compErr);
		}
	} else {
		err = GetGraphicsImporterForFile(&fsSpec, &importComponent);
		if (err != noErr) {
			sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			return static_cast<OSStatus>(err);
		}
	}
	
	return osStatus;
}


OSStatus VisualQuickTime::getGraphicsImporterForHandle(Handle dataHandle, GraphicsImportComponent& importComponent, OSType fileType) {
	OSErr err = noErr;
	OSStatus status = noErr;
	if (fileType != '    ') {
		status = VisualQuickTime::getGraphicsImporterComponentForFileType(fileType, importComponent);
		if (status != noErr) return status;
		ComponentResult compErr = GraphicsImportSetDataHandle(importComponent, dataHandle);
		if (compErr != noErr) return static_cast<OSStatus>(1001);
	} else {
		err = GetGraphicsImporterForDataRef(dataHandle, HandleDataHandlerSubType, &importComponent);
	}
	return static_cast<OSStatus>(err);
}


OSStatus VisualQuickTime::getImageDimensions(GraphicsImportComponent& importComponent, UInt32& width, UInt32& height) {

	width = 0;
	height = 0;

	ImageDescriptionHandle anImageDescriptionHdl = (ImageDescriptionHandle)NewHandle(sizeof(ImageDescriptionHandle));
	HLock((Handle)anImageDescriptionHdl);

	ComponentResult compErr = GraphicsImportGetImageDescription(importComponent, &anImageDescriptionHdl);
	if (compErr != noErr) return static_cast<OSStatus>(1001);
	
	width = (*anImageDescriptionHdl)->width;
	height = (*anImageDescriptionHdl)->height;

	HUnlock((Handle)anImageDescriptionHdl);
	DisposeHandle((Handle)anImageDescriptionHdl);
	
	return noErr;
}


OSStatus VisualQuickTime::setMaxQuality(GraphicsImportComponent& importComponent) {
	ComponentResult compErr = GraphicsImportSetQuality(importComponent, codecLosslessQuality);
	if (compErr != noErr) return static_cast<OSStatus>(1001);
	return noErr;
}


OSStatus VisualQuickTime::flipImportMatrix(GraphicsImportComponent& importComponent, UInt32 imageWidth, UInt32 imageHeight) {
	MatrixRecord matrix;
	SetIdentityMatrix(&matrix);
	TranslateMatrix(&matrix, -X2Fix(0.5 * imageWidth), -X2Fix(0.5 * imageHeight));
	ScaleMatrix(&matrix, X2Fix(1.0), X2Fix(-1.0), X2Fix (0.0), X2Fix (0.0));	
	TranslateMatrix(&matrix, X2Fix(0.5 * imageWidth), X2Fix(0.5 * imageHeight));		
	ComponentResult compErr = GraphicsImportSetMatrix(importComponent, &matrix);
	if (compErr != noErr) return static_cast<OSStatus>(1001);
	return noErr;
}


#if TARGET_OS_MAC
CGImageRef VisualQuickTime::createCGImageRef(GraphicsImportComponent& importComponent) {
	CGImageRef imageRef = NULL;
	ComponentResult compErr = GraphicsImportCreateCGImage(importComponent, &imageRef, kGraphicsImportCreateCGImageUsingCurrentSettings);
	if (compErr != noErr) {
		char errLog[256];
		sprintf(errLog, "GraphicsImportCreateCGImage err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return NULL;
	}
	return imageRef;
}
#endif


#if TARGET_OS_WIN
UInt32* VisualQuickTime::getBitmapOfDrawing(GraphicsImportComponent& importComponent, UInt32 width, UInt32 height) {

	char errLog[256];
	CGrafPtr gSavePort = nil;
	GDHandle gSaveDevice = nil;
	GWorldPtr coverArtGWorld = NULL;
	ComponentResult compErr = noErr;

	UInt32* bitmapData = (UInt32*)calloc(width * height, sizeof(UInt32));

	if (bitmapData == NULL) {
		writeLog("no bitmapData in getBitmapOfDrawing");
		return NULL;
	}
	OSType pixelFormat = k32BGRAPixelFormat;

	Rect destRect;
	destRect.top = 0;
	destRect.left = 0;
	destRect.bottom = static_cast<short>(height);
	destRect.right = static_cast<short>(width);
	QTNewGWorldFromPtr(&coverArtGWorld, pixelFormat, &destRect, NULL, NULL, 0, bitmapData, width * 4);

	if (coverArtGWorld == NULL) {
		writeLog("no new GWorld in getBitmapOfDrawing");
	}

	GetGWorld(&gSavePort,&gSaveDevice);

	compErr = GraphicsImportSetGWorld(importComponent, coverArtGWorld, NULL); // set the destination of the importer component
	if (compErr != noErr) {
		sprintf(errLog, "GraphicsImportSetGWorld err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}

	if (compErr == noErr) {
		if (GetGWorldPixMap(coverArtGWorld) && LockPixels(GetGWorldPixMap(coverArtGWorld))) {
			// call LockPixels to prevent the base address for
			// an offscreen pixel image from being moved while you 
			// draw into or copy from its pixel map

			compErr = GraphicsImportSetGraphicsMode(importComponent, graphicsModeStraightAlpha, NULL);
			if (compErr != noErr) {
				sprintf(errLog, "GraphicsImportSetGraphicsMode err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
			}
			compErr = GraphicsImportDraw(importComponent); // draw image to locked pixmap
		} else {
			writeLog("err GetGWorldPixMap and LockPixels");
		}
	}

	if (compErr != noErr) {
		sprintf(errLog, "GraphicsImportDraw err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		DisposeGWorld (coverArtGWorld); // dump gworld
		coverArtGWorld = 0;
		free(bitmapData);
		bitmapData = NULL;
		CloseComponent(importComponent); // dump component
		return NULL;
	}

	SetGWorld(gSavePort,gSaveDevice);

	UnlockPixels(GetGWorldPixMap(coverArtGWorld));
	DisposeGWorld(coverArtGWorld);
	coverArtGWorld = NULL;

	return bitmapData;
}
#endif


OSStatus VisualQuickTime::writeARGBBitmapToFile(UInt32* argbBitmap, const VisualFile& aFile, UInt32 width, UInt32 height, OSType aFileFormatType) {

	OSStatus osStatus = noErr;
	OSErr osErr = noErr;
	ComponentResult compErr = noErr;
	char errLog[256];

	GWorldPtr imageGWorld = NULL;

	OSType pixelFormat = k32ARGBPixelFormat;

	Rect destRect;
	destRect.top = 0;
	destRect.left = 0;
	destRect.bottom = static_cast<short>(height);
	destRect.right = static_cast<short>(width);
#if TARGET_OS_MAC
	osErr = QTNewGWorldFromPtr(&imageGWorld, pixelFormat, &destRect, NULL, NULL, kNativeEndianPixMap, argbBitmap, width * 4);
#endif
#if TARGET_OS_WIN
	osErr = QTNewGWorldFromPtr(&imageGWorld, pixelFormat, &destRect, NULL, NULL, 0, argbBitmap, width * 4);
#endif
	if (imageGWorld == NULL) {
		return static_cast<OSStatus>(1001);
	}
	if (osErr != noErr) {
		sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", osErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return static_cast<OSStatus>(osErr);
	}

	FSSpec fsSpec;
	
	osStatus = VisualFile::setFSSpecByFileRef(aFile, fsSpec);
	if (osStatus != noErr) {
		sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", osStatus, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return osStatus;
	}

	GraphicsExportComponent ge = NULL;
	
    osErr = OpenADefaultComponent(GraphicsExporterComponentType, aFileFormatType, &ge);
	if (osErr != noErr) {
		sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", osErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return static_cast<OSStatus>(osErr);
	}

    compErr = GraphicsExportSetInputGWorld(ge, imageGWorld);
	if (compErr != noErr) {
		sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return static_cast<OSStatus>(compErr);
	}

    compErr = GraphicsExportSetOutputFile(ge, &fsSpec);
	if (compErr != noErr) {
		sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return static_cast<OSStatus>(compErr);
	}

    compErr = GraphicsExportDoExport(ge, nil);
	if (compErr != noErr) {
		sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", compErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return static_cast<OSStatus>(compErr);
	}
	
    osErr = CloseComponent(ge);
	if (osErr != noErr) {
		sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", osErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return static_cast<OSStatus>(osErr);
	}

	return osStatus;
}


OSStatus VisualQuickTime::getGraphicsImporterComponentForFileType(OSType fileType, GraphicsImportComponent& importComponent) {
	OSErr osErr = OpenADefaultComponent(GraphicsImporterComponentType, fileType, &importComponent);
	return static_cast<OSStatus>(osErr);
}
