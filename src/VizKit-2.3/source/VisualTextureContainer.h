/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTextureContainer.h
 *
 */

/***************************************************************************
 
 Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)
 
 
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

#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

#include <map>


namespace VizKit {
	
	class VisualConvolutionFilter; // Forward declaration (to avoid include of header file).
	class VisualStyledString; // Forward declaration (to avoid include of header file).
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
		 * Initializes a texture by passing a file.
		 * @param aFile Reference to a file.
		 * @return True on success, false on failure.
		 */
		bool initWithFile(VisualFile& aFile);
		
		/**
		 * Initializes a texture by passing a data pointer to encoded image data.
		 * @param bufferData A pointer to the data.
		 * @param size The size of the data pointed to in bytes.
		 * @return True on success, false on failure.
		 */
		bool initWithEncodedData(const char* const bufferData, size_t size);

		/**
		 * Initializes a texture with a buffer of ARGB pixel data.
		 * @param argbPixels Pointer to ARGB pixel data.
		 * @param imageWidth The width of the image data.
		 * @param imageHeight The height of the image data.
		 * @param debug If true, canned sample data is used.
		 * @return True on success, false on failure.
		 * @remarks No reference to the pixel data is stored, pixel data can be savely disposed after the call.
		 */
		bool initWithARGBPixelData(PixelColor* argbPixels, size_t imageWidth, size_t imageHeight, bool debug = false);
		
		/**
		 * Initializes a texture with a buffer of BGRA pixel data.
		 * @param bgraPixels Pointer to BGRA pixel data.
		 * @param imageWidth The width of the image data.
		 * @param imageHeight The height of the image data.
		 * @param debug If true, canned sample data is used.
		 * @return True on success, false on failure.
		 * @remarks No reference to the pixel data is stored, pixel data can be savely disposed after the call.
		 */
		bool initWithBGRAPixelData(PixelColor* bgraPixels, size_t imageWidth, size_t imageHeight, bool debug = false);
		
		/**
		 * Creates a texture of the passed string.
		 * @param styledString The styled string to plot to texture.
		 * @return True on success, false on failure.
		 */
		bool initWithStyledString(VisualStyledString& styledString);
		
		/**
		 * Cleans the texture data.
		 */
		void clean(void);
		
		/**
		 * Creates a texture with contents of the current screen (framebuffer).
		 * @param clipRect The dimensions and position of the clip rect.
		 * @return True on success, false on failure.
		 */
		bool initWithFramebuffer(const BottomLeftPositionedPixelRect& clipRect);
		
		/**
		 * Returns bitmap data of the texture.
		 * @param debug If true, canned sample data is returned.
		 * @return A pointer to the argb pixel data.
		 * @remarks The pixel data is returned in dimensions of the image data (not necessarily the dimensions of the texture). The caller has to release the pixel data by calling free().
		 */	
		PixelColor* createARGBImagePixels(bool debug = false);
		
		/**
		 * Returns the name/number/id of the current texture.
		 * @return The name/number/id of the current texture.
		 */
		uint32 getTextureName(void) const;
		
		/**
		 * Returns the width of the texture.
		 * @return The width of the texture.
		 */
		size_t getTextureWidth(void) const;
		
		/**
		 * Returns the height of the texture.
		 * @return The height of the texture.
		 */
		size_t getTextureHeight(void) const;
		
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
		size_t getImageWidth(void) const;
		
		/**
		 * Returns the number of pixels of image data in vertical direction.
		 * @return The pixel height of the image data.
		 */
		size_t getImageHeight(void) const;
		
		/**
		 * Returns whether the texture is drawn with rectangular texture extension.
		 * @return True if graphics system does support rectangular texture extension. False otherwise.
		 * @remarks With rectangular texure extension non-power-of-2 (npot) rectangles can be stored in texture memory.
		 */
		bool getUseRectExtension(void) const;
		
		/**
		 * Frees any resources possibly allocated for texture data.
		 */
		void releaseTextureData(void);
		
		/**
		 * Answers the question whether currently a texture is set.
		 * @return True if currently a texture is set. False otherwise.
		 */
		bool textureIsSet(void);
		
		/**
		 * Applies a convolution filter to the texture.
		 * @param aConvolutionFilter The convolution filter to apply to the texture.
		 */
		void applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter);
		
		/**
		 * Resamples the pixel data of the texture to the given dimensions of the pixelRect.
		 * @param pixelRect The resampling dimensions.
		 */
		void resample(const PixelRect& pixelRect);
		
	private:
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualTextureContainer.
		 */
		void copy(const VisualTextureContainer& other);
		
		/**
		 * Private method accessing the pixels via drawing and reading.
		 * @param format The format of the requested pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param type The data type of the pixel data (8-bit byte, float, etc.).
		 * @remarks The caller has to release the pixel data by calling free().
		 */
		PixelColor* createReadPixels(const uint16 format, const uint16 type);
		
		/** A TextureRefCountMap is a map of texture names and ref counts. */
		typedef std::map<uint32, uint32> TextureRefCountMap;
		
		/** The map of the texture names and ref counts. */
		static TextureRefCountMap textureRefCountMap;
		
		bool aTextureIsSet; /**< True if the texture container actually contains a texture. */
		
		uint32 textureName; /**< The name (number, id) of the texture. */
		
		PixelRect textureRect; /**< The dimensions of the texture. */
		
		/**
		 * Logical size of a texture. The logical size of the texture depends on useRectExtension value.
		 */
		typedef struct {
			double width; /**< Logical texture width. */
			double height; /**< Logical texture height. */
		} LogicalSize;
		
		LogicalSize logicalSize; /**< The logical size of the texture. */
		
		PixelRect imageRect; /**< The dimensions of the image data. */
		
		bool useRectExtension; /**< True if the texture is created with rectangular texture extension (npot texture). False otherwise. */
		
	};
	
}


#endif /* VisualTextureContainer_h */
