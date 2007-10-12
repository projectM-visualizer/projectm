/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualTextureContainer.h
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

#ifndef VisualTextureContainer_h
#define VisualTextureContainer_h

#include "VisualGraphicTypes.h"

#include <map>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualConvolutionFilter; // Forward declaration (to avoid include of header file).
	class VisualString; // Forward declaration (to avoid include of header file).
	class VisualStringStyle; // Forward declaration (to avoid include of header file).
	class VisualFile; // Forward declaration (to avoid include of header file).
	
	/**
	 * A data container with a reference to an image texture.
	 */
	class VisualTextureContainer {

	public:

		/**
		 * The constructor.
		 */
		VisualTextureContainer();

		/**
		 * Another constructor of a VisualTextureContainer with dimensions and power-of-2-requirements as arguments.
		 * @param anImageWidth The image width.		 
		 * @param anImageHeight The image height.
		 * @param useRectExtensionBool The image height.\ True if graphics system does support rectangular texture extension.\ False otherwise.
		 */
		VisualTextureContainer(UInt32 anImageWidth, UInt32 anImageHeight, bool useRectExtensionBool);
		
		/**
		 * The destructor.
		 */
		~VisualTextureContainer();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualTextureContainer.
		 */
		VisualTextureContainer(const VisualTextureContainer& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualTextureContainer.
		 */
		VisualTextureContainer& operator=(const VisualTextureContainer& other);

		/**
		 * Initializes a texture by passing a data handle.
		 * @param dataHandle A handle of the data.
		 * @param fileType The file type of the data (e.g.\ JPEG or PNGf).
		 * @return Error status.
		 * @remarks Handle can be created from regular 8bit pointer with PtrToHand().
		 */
		OSStatus initWithDataHandle(Handle dataHandle, OSType fileType);

		/**
		 * Initializes a texture by passing a data pointer to a pointer.
		 * @param aPointerToPointerToBuffer A pointer to pointer to the data.
		 * @param size The size of the data pointed to in bytes.
		 * @param aFileFormatType The file type of the image (e.g.\ JPEG or PNGf).
		 * @return Error state (0 means no Error).
		 */
		OSStatus initWithDataPointerToPointer(const unsigned char** const aPointerToPointerToBuffer, UInt32 size, OSType aFileFormatType);

		/**
		 * Initializes a texture by passing a file.
		 * @param aFile Reference to a file.
		 * @param fileType Optional file type.\ Providing the file type as a hint may speed up finding the correct component.
		 * @return Error status.
		 */
		OSStatus initWithFile(const VisualFile& aFile, OSType fileType = '    ');

		/**
		 * Creates a texture of the passed string.
		 * @param stringValue The string to plot to texture.
		 * @param stringStyle The style information of the string.
		 * @return Error number.
		 */
		OSStatus initWithString(const VisualString& stringValue, VisualStringStyle& stringStyle);

		/**
		 * Creates a texture of the passed url.
		 * @param url The url of the image data.
		 * @return Error number.
		 */
#if TARGET_OS_MAC
		OSStatus initWithURL(VisualString& anURL);
#endif
		/**
		 * Creates a texture with the content of the current screen (framebuffer).
		 * @param xPos The horizontal x position of the screen rect.
		 * @param yPos The vertical y position of the screen rect.
		 * @param width The width of the rectangle.
		 * @param height The height of the rectangle.
		 * @return Error number.
		 */
		OSStatus initWithFramebuffer(UInt32 xPos, UInt32 yPos, UInt32 width, UInt32 height);
		
		/**
		 * Returns bitmap data of the texture.
		 * @param format The format of the requested pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param type The data type of the pixel data (8-bit byte, float, etc.).
		 * @return A pointer to the rgba pixel data.
		 */	
		UInt32* getTexturePixels(const UInt16 format, const UInt16 type);

		/**
		 * Sets the pixel width of the texture.
		 * @param aTextureWidth The pixel width of the texture.
		 */
		void setTextureWidth(UInt32 aTextureWidth);

		/**
		 * Sets the pixel height of the texture.
		 * @param aTextureHeight The pixel height of the texture.
		 */
		void setTextureHeight(UInt32 aTextureHeight);
		
		/**
		 * Sets the logical width of the texture.
		 * @param aTextureLogicalWidth The logical width of the texture.\ The logical width typically ranges from 0.0 to 1.0.\ If the imageWidth is smaller than the textureWidth, aTextureLogicalWidth is smaller than the maximum value.\ If useRectExtension == true then aTextureLogicalWidth equals imageWidth and textureWidth.
		 */
		void setTextureLogicalWidth(double aTextureLogicalWidth);
		
		/**
		 * Sets the logical height of the texture.
		 * @param aTextureLogicalHeight The logical height of the texture.\ The logical height typically ranges from 0.0 to 1.0.\ If the imageHeight is smaller than the textureHeight, aTextureLogicalHeight is smaller than the maximum value.\ If useRectExtension == true then aTextureLogicalHeight equals imageHeight and textureHeight.
		 */
		void setTextureLogicalHeight(double aTextureLogicalHeight);

		/**
		 * Sets the pixel width of the image data.
		 * @param anImageWidth The pixel width of the image data.
		 */
		void setImageWidth(UInt32 anImageWidth);

		/**
		 * Sets the pixel height of the image data.
		 * @param anImageHeight The pixel height of the image data.
		 */
		void setImageHeight(UInt32 anImageHeight);

		/**
		 * Returns the name/number/id of the current texture.
		 * @return The name/number/id of the current texture.
		 */
		UInt32 getTextureName(void) const;

		/**
		 * Returns the width of the texture.
		 * @return The width of the texture.
		 */
		UInt32 getTextureWidth(void) const;

		/**
		 * Returns the height of the texture.
		 * @return The height of the texture.
		 */
		UInt32 getTextureHeight(void) const;

		/**
		 * Returns the logical width of the texture.
		 * @return The logical width of the texture.
		 */
		double getTextureLogicalWidth(void) const;

		/**
		 * Returns the logical height of the texture.
		 * @return The logical height of the texture.
		 */
		double getTextureLogicalHeight(void) const;

		/**
		 * Returns the number of pixels of image data in horizontal direction.
		 * @return The pixel width of the image data.
		 */
		UInt32 getImageWidth(void) const;

		/**
		 * Returns the number of pixels of image data in vertical direction.
		 * @return The pixel height of the image data.
		 */
		UInt32 getImageHeight(void) const;

		/**
		 * Returns whether the texture is drawn with rectangular texture extension.
		 * @return True if graphics system does support rectangular texture extension.\ False otherwise.
		 * @remarks With rectangular texure extension non-power-of-2-rectangles can be stared in tetxures.
		 */
		bool getUseRectExtension(void) const;

		/**
		 * Frees any resources possibly allocated for texture data.
		 */
		void releaseTextureData(void);

		/**
		 * Applies a convolution filter to the texture.
		 * @param aConvolutionFilter The convolution filter to apply to the texture.
		 */
		void applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualTextureContainer.
		 */
		void copy(const VisualTextureContainer& other);
		
		UInt32* getRectPixels(const UInt16 format, const UInt16 type);

		/** A TextureRefCountMap is a map of texture names and ref counts. */
		typedef std::map<UInt32, UInt32> TextureRefCountMap;
		
		/** The map of the texture names and ref counts. */
		static TextureRefCountMap textureRefCountMap;
		
		bool textureIsSet; /**< True if the texture container actually contains a texture. */

		UInt32 textureName; /**< The name (number, id) of the texture. */
		
		UInt32 textureWidth; /**< The pixel width of the texture. */
		
		UInt32 textureHeight; /**< The pixel height of the texture. */
		
		double textureLogicalWidth; /**< The logical width of the texture. */
		
		double textureLogicalHeight; /**< The logical height of the texture. */
		
		UInt32 imageWidth; /**< The pixel width of the image data. */
		
		UInt32 imageHeight; /**< The pixel height of the image data. */
		
		UInt16 pixelFormat; /**< The format of the pixel data (rgba, luminance, etc.). */
		
		UInt16 dataType; /**< The data type of the pixel data (8-bit byte, float, etc.). */
		
		bool useRectExtension; /**< True if graphics system does support rectangular texture extension.\ False otherwise. */
		
		UInt32* pixelBuffer; /**< Pointer to 4-channel 32-bit pixel buffer of current texture (8 bit per channel).\ NULL if there is no pixel buffer of the texture available. */

	};

}


#endif /* VisualTextureContainer_h */
