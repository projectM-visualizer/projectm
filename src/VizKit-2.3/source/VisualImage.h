/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualImage.h
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

#ifndef VisualImage_h
#define VisualImage_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"
#include "VisualObject.h"
#include "VisualNotification.h"
#include "VisualHistogram.h"
#include "VisualThreading.h"

namespace VizKit {
	
	class VisualString; // Forward declaration (to avoid include of header file).
	class VisualStyledString; // Forward declaration (to avoid include of header file).
	class VisualFile; // Forward declaration (to avoid include of header file).
	class VisualTextureContainer; // Forward declaration (to avoid include of header file).
	class VisualConvolutionFilter; // Forward declaration (to avoid include of header file).
	class VisualCamera; // Forward declaration (to avoid include of header file).
	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	

	/**
	 * Bitmap raster image.
	 */
	class VisualImage : public VisualObject {
		
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
		 * Copies the current VisualImage and returns a pointer to a new VisualImage.
		 * @remarks Cloning the image returns an image pointing to the same framebuffer data. To create an independent image, use createIndependentCopy().
		 */
		virtual VisualImage* clone(void) const;
		
		/**
		 * Creates an independent copy of the image.
		 * @remarks The image copy is independent of the original image. Cloning the image returns an image pointing to the same framebuffer data.
		 */
		VisualImage* cloneIndependently(void) const;
		
		/**
		 * Initializes the image with a file.
		 * @param aFile Reference to a file.
		 * @return True on success, false on failure.
		 */
		bool initWithFile(VisualFile& aFile);
		
		/**
		 * Initializes the image with a buffer of ARGB pixel data.
		 * @param argbPixels Pointer to ARGB pixel data.
		 * @param width The width of the pixel data.
		 * @param height The height of the pixel data.
		 * @return True on succes, false on failure.
		 * @remarks No reference to the pixel data is stored, pixel data can be savely disposed after the call.
		 */
		bool initWithARGBPixelData(PixelColor* argbPixels, uint32 width, uint32 height);
		
		/**
		 * Initializes the image with a data pointer to encoded image data.
		 * @param bufferData A pointer to the data.
		 * @param size The size of the data pointed to in bytes.
		 * @return True on success, false on failure.
		 */
		bool initWithEncodedData(const char* const bufferData, size_t size);
		
		/**
		 * Initializes the image with a styled string.
		 * @param styledString The styled string to convert into bitmap image.
		 * @return True on success, false on failure
		 */
		bool initWithStyledString(VisualStyledString& styledString);
		
#if TARGET_OS_WIN
		/**
		 * Initializes the image with a resource image.
		 * @param nameId The id of the resource.
		 * @return True on success, false on failure.
		 */
		bool initWithResource(int nameId);
#endif
#if TARGET_OS_MAC
		/**
		 * Initializes the image with a resource image.
		 * @param name The name of the resource.
		 * @return True on success, false on failure.
		 */
		bool initWithResource(const char* name);
#endif
		
		/**
		 * Initializes the image with an URL.
		 * @param anURL URL of the image.
		 * @param anId Identifier that can be queried to track the progress of this asynchronous initialization.
		 * @return True on success, false on failure.
		 */
		bool initWithURL(VisualString& anURL, VisualItemIdentifier& anId);
		
		/**
		 * Initializes the image with loaded encoded image data.
		 * @return True on success, false on failure.
		 */
		bool initWithLoadedEncodedData(void);
		
		/**
		 * Initializes the image with contents of the current screen (framebuffer).
		 * @param clipRect The dimensions and position of the clip rect.
		 * @return True on success, false on failure.
		 */
		bool initWithFramebuffer(const BottomLeftPositionedPixelRect& clipRect);
		
		/**
		 * Writes the image as PNG encoded data to a file on disk.
		 * @param aVisualFile Access to file the image is to be written into.
		 * @return True on success, false on failure.
		 */
		bool writeToPNGFile(VisualFile& aVisualFile) const;

		/**
		 * Writes the image as PNG encoded data to a file on disk asynchronously.
		 * @param aVisualFile Access to file the image is to be written into.
		 */
		void writeToPNGFileAsync(VisualFile& aVisualFile);

		/**
		 * Writes the image as PNG encoded data to a file on disk asynchronously and deletes the VisualImage afterwards.
		 * @param aVisualFile Access to file the image is to be written into.
		 */
		void writeToPNGFileAsyncAndDelete(VisualFile& aVisualFile);
		
		/**
		 * Applies a convolution filter to the image.
		 * @param aConvolutionFilter The convolution filter to apply to the image.
		 */
		void applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter);
		
		/**
		 * Returns the width of the bitmap image (in pixels).
		 * @return The width of the bitmap image (in pixels).
		 */
		uint32 getWidth(void) const;
		
		/**
		 * Returns the height of the bitmap image (in pixels).
		 * @return The height of the bitmap image (in pixels).
		 */
		uint32 getHeight(void) const;
		
		/**
		 * Returns the logical width of the texture.
		 * @return The logical width of the texture.
		 */
		double getLogicalWidth(void) const;
		
		/**
		 * Returns the logical height of the texture.
		 * @return The logical height of the texture.
		 */
		double getLogicalHeight(void) const;
		
		/**
		 * Draws the image.
		 * @param aVertexChain The chain of vertices with which the image is drawn.
		 * @param debug If true, details of the drawing operation can be traced for diagnostical purposes.
		 */
		void draw(const VertexChain* const aVertexChain, bool debug = false);
		
		/**
		 * Answers the question whether the image is empty or set.
		 */
		bool isEmpty(void) const;
		
		/**
		 * Returns the blend mode of the image.
		 * @return The blend mode of the image.
		 */
		BlendMode getBlendMode(void) const;
		
		/**
		 * Sets the blend mode of the image.
		 * @param aBlendMode The blend mode of the image.
		 */
		void setBlendMode(BlendMode aBlendMode);
		
		/**
		 * Creates an image with a file.
		 * @param aFile Reference to a file.
		 * @return Initialized instance of VisualImage. NULL in case of failure.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithFile(VisualFile& aFile);
		
		/**
		 * Creates an image with a buffer of ARGB pixel data.
		 * @param argbPixels Pointer to ARGB pixel data.
		 * @param width The width of the pixel data.
		 * @param height The height of the pixel data.
		 * @return Initialized instance of VisualImage. NULL in case of failure.
		 * @remarks The caller has to release (delete) the allocated memory. No reference to the pixel data is stored, pixel data can be savely disposed after the call.
		 */
		static VisualImage* createWithARGBPixelData(PixelColor* argbPixels, uint32 width, uint32 height);
		
		/**
		 * Creates an image with a data pointer to encoded image data.
		 * @param bufferData A pointer to the data.
		 * @param size The size of the data pointed to in bytes.
		 * @return Initialized instance of VisualImage. NULL in case of failure.
		 * @remarks The caller has to release (delete) the initialized image.
		 */
		static VisualImage* createWithEncodedData(const char* const bufferData, uint32 size);
		
		/**
		 * Creates an image with a styled string.
		 * @param styledString The styled string to convert into bitmap image.
		 * @return Initialized instance of VisualImage. NULL in case of failure.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithStyledString(VisualStyledString& styledString);
		
#if TARGET_OS_MAC
		/**
		 * Creates an image with a resource image.
		 * @param name The name of the resource.
		 * @return Initialized instance of VisualImage. NULL in case of failure.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithResource(const char* name);
#endif
#if TARGET_OS_WIN
		/**
		 * Creates an image with a resource image.
		 * @param nameId The id of the resource.
		 * @return Initialized instance of VisualImage. NULL in case of failure.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithResource(int nameId);
#endif
		
		/**
		 * Creates an image with an URL.
		 * @param anURL URL of the image.
		 * @param anId Identifier that can be queried to track the progress of this asynchronous creation.
		 * @return Initialized instance of VisualImage.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithURL(VisualString& anURL, VisualItemIdentifier& anId);
		
		/**
		 * Creates an image with contents of the current screen (framebuffer).
		 * @param clipRect The dimensions and position of the clip rect.
		 * @remarks The caller has to release (delete) the allocated memory.
		 */
		static VisualImage* createWithFramebuffer(const BottomLeftPositionedPixelRect& clipRect);
		
		/**
		 * Resamples the pixel data of the texture to the given dimensions of the pixelRect.
		 * @param pixelRect The resampling dimensions.
		 */
		void resample(const PixelRect& pixelRect);
		
		/**
		 * Returns a pointer to the texture container of the image.
		 * @return A pointer to the texture container of the image.
		 */
		VisualTextureContainer* getTextureContainer(void) const;
		
		/**
		 * Notification that the loading of data ended.
		 * @param identifier The identifier of the data load request.
		 */	
		virtual void dataLoadDidEnd(const VisualItemIdentifier& identifier);

		/**
		 * Creates a histogram of RGB pixel color values. A histogram contains the salient color values of the image.
		 * @remarks The histogram is created asynchronously. On finish, a notification is sent (kImageHistogramCompletedMsg) on which actors can be registered. The image is downsampled and the histogram is calculated with the k-means algorithm.
		 */	
		void createHistogram(void);

		/**
		 * Returns the internal histogram.
		 * @return The histogram. Can be NULL in case no histogram has been set.
		 */	
		VisualHistogram::PixelColorHistogram* getHistogram(void);
		
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
		
		/** The blend mode of the image. */
		BlendMode blendMode;
		
		/**
		 * Returns the array of RGB color pixels as required to create a histogram.
		 * @param[out] inputValues The pixels that can be used as input for histogram creation.
		 */	
		void getRGBHistogramInputPixels(std::vector<PixelColor>& inputValues) const;
		
		/**
		 * Creates a histogram of RGB pixel color values.
		 * @param visualImageHistogramPixelColors Pixel color values and pointer to image.
		 */
#if TARGET_OS_MAC
		static OSStatus createHistogramOfRGBPixelsThread(void* visualImageHistogramPixelColors);
#endif
#if TARGET_OS_WIN
		static DWORD createHistogramOfRGBPixelsThread(LPVOID visualImageHistogramPixelColors);
#endif
		
		/** Histogram. */
		VisualHistogram::PixelColorHistogram* histogram;
		
		/**
		 * Exchange data type for histogram creation.
		 */
		typedef struct {
			std::vector<PixelColor> pixelColorValuesVector; /**< The RGB pixelcolor value. */
			VisualImage* visualImage; /**< Pointer to image. */
		} VisualImageHistogramPixelColors;
		
	};
	
}

#endif /* VisualImage_h */
