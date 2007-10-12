/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualImage.h
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

#ifndef VisualImage_h
#define VisualImage_h

#include "VisualGraphicTypes.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualString; // Forward declaration (to avoid include of header file).
	class VisualStringStyle; // Forward declaration (to avoid include of header file).
	class VisualFile; // Forward declaration (to avoid include of header file).
	class VisualTextureContainer; // Forward declaration (to avoid include of header file).
	class VisualConvolutionFilter; // Forward declaration (to avoid include of header file).

	/**
	 * Bitmap raster image.
	 */
	class VisualImage {
	
	public:

		/**
		 * The constructor. 
		 */
		VisualImage();

		/**
		 * The destructor.
		 */
		~VisualImage();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualImage.
		 */
		VisualImage(const VisualImage& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualImage.
		 */
		VisualImage& operator=(const VisualImage& other);

		/**
		 * Initializes the image with a file.
		 * @param aFile Reference to a file.
		 * @return Error state (0 means no Error).
		 */
		OSStatus initWithFile(const VisualFile& aFile);

		/**
		 * Initializes the image with a data handle.
		 * @param aDataHandle A handle of the data.
		 * @param aFileFormatType The file type of the image (e.g.\ JPEG or PNGf).
		 * @return Error state (0 means no Error).
		 * @remarks Handle can be created from regular 8bit pointer with PtrToHand().
		 */
		OSStatus initWithDataHandle(Handle aDataHandle, OSType aFileFormatType);

		/**
		 * Initializes the image with a data pointer to a pointer.
		 * @param aPointerToPointerToBuffer A pointer to pointer to the data.
		 * @param size The size of the data pointed to in bytes.
		 * @param aFileFormatType The file type of the image (e.g.\ JPEG or PNGf).
		 * @return Error state (0 means no Error).
		 */
		OSStatus initWithDataPointerToPointer(const unsigned char** const aPointerToPointerToBuffer, UInt32 size, OSType aFileFormatType);

		/**
		 * Initializes the image with a styled string.
		 * @param stringValue The string to convert into bitmap image.
		 * @param stringStyle The style information of the string.
		 * @return Error state (0 means no Error).
		 */
		OSStatus initWithString(const VisualString& stringValue, VisualStringStyle& stringStyle);

		/**
		 * Initializes the image with a resource image.
		 * @param nameId The name resp. id of the resource.
		 * @param type The declared type of resource.
		 * @return Error state (0 means no Error).
		 */
#if TARGET_OS_WIN
		OSStatus initWithResource(int nameId);
#endif
#if TARGET_OS_MAC
		OSStatus initWithResource(char* name);
#endif

		/**
		 * Initializes the image with an URL.
		 * @param anURL URL of the image.
		 * @return Error state (0 means no Error).
		 */
#if TARGET_OS_MAC
		OSStatus initWithURL(VisualString& anURL);
#endif
		
		/**
		 * Writes the image to a file on disk.
		 * @param aVisualFile Access to file the image is to be written into.
		 * @param aFileFormatType The requested file format of the image file to be written.
		 * @return Error state (0 means no Error).
		 * @remarks For possible file formats consult enum in QuickTimeComponents.h (kQTFileTypePicture = 'PICT', kQTFileTypeMovie = 'MooV', kQTFileTypePICS = 'PICS', kQTFileTypeGIF = 'GIFf', kQTFileTypePNG = 'PNGf', 
						kQTFileTypeTIFF = 'TIFF', kQTFileTypePhotoShop = '8BPS', kQTFileTypeSGIImage = '.SGI', kQTFileTypeBMP = 'BMPf', kQTFileTypeJPEG = 'JPEG', kQTFileTypeJFIF = 'JPEG', kQTFileTypeMacPaint = 'PNTG', 
						kQTFileTypeTargaImage = 'TPIC', kQTFileTypeQuickDrawGXPicture = 'qdgx', kQTFileTypeQuickTimeImage = 'qtif', kQTFileType3DMF = '3DMF', kQTFileTypeFLC = 'FLC ', kQTFileTypeFlash = 'SWFL',
						kQTFileTypeFlashPix = 'FPix', kQTFileTypeMP4 = 'mpg4', kQTFileTypePDF = 'PDF ', kQTFileType3GPP = '3gpp', kQTFileTypeAMR = 'amr ', kQTFileTypeSDV = 'sdv ', kQTFileType3GP2 = '3gp2', 
						kQTFileTypeAMC = 'amc ', kQTFileTypeJPEG2000 = 'jp2 ').
		 */
		OSStatus writeToFile(const VisualFile& aVisualFile, OSType aFileFormatType = 'PNGf');

		/**
		 * Applies a convolution filter to the image.
		 * @param aConvolutionFilter The convolution filter to apply to the image.
		 */
		void applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter);

		/**
		 * Returns the width of the bitmap image (in pixels).
		 * @return The width of the bitmap image (in pixels).
		 */
		UInt32 getWidth(void);

		/**
		 * Returns the height of the bitmap image (in pixels).
		 * @return The height of the bitmap image (in pixels).
		 */
		UInt32 getHeight(void);

		/**
		 * Returns the logical width of the texture.
		 * @return The logical width of the texture.
		 */
		double getLogicalWidth(void);

		/**
		 * Returns the logical height of the texture.
		 * @return The logical height of the texture.
		 */
		double getLogicalHeight(void);

		/**
		 * Draws the image.
		 * @param aVertexChain The chain of vertices with which the image is drawn.
		 */
		void draw(VertexChain* aVertexChain);

		/**
		 * Answers the question whether the image is empty or set.
		 */
		bool isEmpty(void);
		
		/**
		 * Creates an image with a file.
		 * @param aFile Reference to a file.
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithFile(const VisualFile& aFile);

		/**
		 * Creates an image with a data handle.
		 * @param aDataHandle A handle of the data.
		 * @param aFileFormatType The file type of the image (e.g.\ JPEG or PNGf).
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithDataHandle(Handle aDataHandle, OSType aFileFormatType);

		/**
		 * Creates an image with a data pointer to a pointer.
		 * @param aPointerToPointerToBuffer A pointer to pointer to the data.
		 * @param size The size of the data pointed to in bytes.
		 * @param aFileFormatType The file type of the image (e.g.\ JPEG or PNGf).
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithDataPointerToPointer(const unsigned char** const aPointerToPointerToBuffer, UInt32 size, OSType aFileFormatType);

		/**
		 * Creates an image with a styled string.
		 * @param stringValue The string to convert into bitmap image.
		 * @param stringStyle The style information of the string.
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		VisualImage* createWithString(const VisualString& stringValue, VisualStringStyle& stringStyle);

		/**
		 * Creates an image with a resource image.
		 * @param name The name resp. id of the resource.
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
#if TARGET_OS_MAC
		VisualImage* createWithResource(char* name);
#endif
#if TARGET_OS_WIN
		VisualImage* createWithResource(int nameId);
#endif

		/**
		 * Creates an image with an URL.
		 * @param anURL URL of the image.
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
#if TARGET_OS_MAC
		static VisualImage* createWithURL(VisualString& anURL);
#endif

	private:

		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualImage.
		 */
		void copy(const VisualImage& other);
		
		/** Pointer to the VisualTextureContainer. */
		VisualTextureContainer* visualTextureContainer;
		
		/** True if image has been set. False if image is not valid or has not been set yet. */
		bool isSet;

	};

}

#endif /* VisualImage_h */
