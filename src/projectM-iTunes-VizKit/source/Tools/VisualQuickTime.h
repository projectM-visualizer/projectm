/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualQuickTime.h
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

#ifndef VisualQuickTime_h
#define VisualQuickTime_h

#include "VisualFile.h"


#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


#if TARGET_OS_MAC
	#include <QuickTime/QuickTime.h>
#endif

#if TARGET_OS_WIN
	#include <QT/QTML.h>
	#include <QT/Movies.h>
	#include <QT/FixMath.h>
#endif


namespace VizKit {


	/**
	 * Collection of QuickTime functionality.\ Access of QuickTime functionality is restricted to this class.
	 */
	class VisualQuickTime {
	
	public:

		/**
		 * Initializes the QuickTime system component.\ Necessary resp. recommended on Windows.
		 * @return Error state as SInt16 OSErr.
		 */
		static OSErr initialize(void);

		/**
		 * Terminates the QuickTime system component.\ Necessary resp. recommended on Windows.
		 * @return Error state as SInt16 OSErr.
		 */
		static OSErr terminate(void);

		/**
		 * Returns a GraphicsImporter component suitable for a file.
		 * @param aFile Reference to a file.
		 * @param[out] importComponent The found GraphicsImporter component.
		 * @param fileType Optional file type.\ Providing the file type as a hint may speed up finding the correct component.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus getGraphicsImporterForFile(const VisualFile& aFile, GraphicsImportComponent& importComponent, OSType fileType = '    ');
		
		/**
		 * Returns a GraphicsImporter component suitable for a data handle.
		 * @param dataHandle The data handle.
		 * @param[out] importComponent The found GraphicsImporter component.
		 * @param fileType Optional file type.\ Providing the file type as a hint may speed up finding the correct component.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus getGraphicsImporterForHandle(Handle dataHandle, GraphicsImportComponent& importComponent, OSType fileType = '    ');
		
		/**
		 * Returns the dimensions (width and height) in pixels.
		 * @param importComponent The GraphicsImporter component.
		 * @param[out] width The width of the image.
		 * @param[out] height The height of the image.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus getImageDimensions(GraphicsImportComponent& importComponent, UInt32& width, UInt32& height);

		/**
		 * Sets the import quality to maximum for the GraphicsImporter component.
		 * @param importComponent The GraphicsImporter component for which the quality is set.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus setMaxQuality(GraphicsImportComponent& importComponent);

		/**
		 * Flips the import matrix.
		 * @param importComponent The GraphicsImporter component for which the matrix is flipped.
		 * @param imageWidth The width of the image.
		 * @param imageHeight The height of the image.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus flipImportMatrix(GraphicsImportComponent& importComponent, UInt32 imageWidth, UInt32 imageHeight);

#if TARGET_OS_MAC
		/**
		 * Creates a CGImage.
		 * @param importComponent The GraphicsImporter component for which the import matrix is flipped.
		 * @return A pointer to the CGImage.
		 */
		static CGImageRef createCGImageRef(GraphicsImportComponent& importComponent);
#endif

#if TARGET_OS_WIN
		/**
		 * Returns a pointer to the bitmap that is created by drawing the content of the GraphicsImportComponent.
		 * @param importComponent The GraphicsImporter component for which the import matrix is flipped.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to the bitmap.
		 */
		static UInt32* getBitmapOfDrawing(GraphicsImportComponent& importComponent, UInt32 width, UInt32 height);
#endif

		/**
		 * Writes the bitmap data into a file stored on hard disk.
		 * @param argbBitmap Pointer to RGBA bitmap data.
		 * @param aFile Reference to a file.
		 * @param width The width of the bitmap.
		 * @param height The height of the bitmap.
		 * @param aFileFormatType The requested file format of the image file to be written.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus writeARGBBitmapToFile(UInt32* argbBitmap, const VisualFile& aFile, UInt32 width, UInt32 height, OSType aFileFormatType);

	private:

		/** The constructor.\ VisualQuickTime is a collection of static methods.\ Class does not need any instances.\ Constructor is private and not implemented. */
		VisualQuickTime();

		/** The destructor.\ VisualQuickTime is a collection of static methods.\ Class does not need any instances.\ Destructor is private and not implemented. */
		~VisualQuickTime();

		/**
		 * Returns a GraphicsImporter component suitable for the file type.
		 * @param fileType The file type expressed as 4-char code file type.
		 * @param[out] importComponent The found GraphicsImporter component.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks Private helper function which returns QuickTime's component for a specific file type.
		 */
		static OSStatus getGraphicsImporterComponentForFileType(OSType fileType, GraphicsImportComponent& importComponent);

	};

}

#endif /* VisualQuickTime_h */
