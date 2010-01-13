/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualDispatch.h
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

#ifndef VisualDispatch_h
#define VisualDispatch_h


#include "VisualTypes.h"


/* The dispatcher provides a c-callable interface to the VizKit classes for the Cocoa framework. */

#ifdef __cplusplus
extern "C" {
#endif
	
	
	/**
	 * Adds a pair of label and value to process monitor info array.
	 * The label-value-pairs of the process monitor info array are printed on canvas.
	 * @param labelStr The label of the process monitor info text.
	 * @param valueStr The value of the process monitor info text.
	 */
	void setProcessInfo(const char* const labelStr, const char* const valueStr);
	
	/**
	 * The setProcessInfo calls of VisualMain's HandleRenderMessage.
	 */
	void monitorRenderMessageProcess(void);
	
	/**
	 * The function queries the system and answers whether textures can have non power of 2 dimensions.
	 * Textures with exact dimensions need less space.
	 * To stay compatible any callee has to cope with a negative answer.
	 * @return 1 if graphics system does support rectangular texture extension. 0 otherwise.
	 */
	uint8 graphicsDoSupportTextureRectExtension(void);
	
#if TARGET_OS_MAC
	
	/** Set function pointers to Cocoa functions. Mac-only.
	 * To call Cocoa methods from C we have to define pointers to Cocoa methods.
	 */
	void setRefsToCocoaFunctionCalls(void);
	
	/**
	 * The function returns the dimensions (width and height) of a bitmap of a styled string in pixels.
	 * @param aVisualString The string to measure.
	 * @param[in,out] aVisualStringStyle The style of the string.
	 * @param[out] imageWidth Returns the width of the embedded track title font image (measured in pixels).
	 * @param[out] imageHeight Returns the height of the embedded track title font image (measured in pixels).
	 * @param maxPixelWidth The requested maximum width of the texture.
	 * @param maxPixelHeight The requested maximum height of the texture.
	 * @param alignment The horizontal alignment of the string.
	 * @return True on success, false on failure.
	 */
	bool getDimensionsOfCocoaStringBitmap(void* aVisualString, void* aVisualStringStyle, uint32* imageWidth, uint32* imageHeight, uint16 maxPixelWidth, uint16 maxPixelHeight, char* alignment);
	
	/**
	 * The function returns the bitmap data of a styled string.
	 * @param aVisualStyledString The styled string to convert into bitmap.
	 * @param bitmapWidth The width of the bitmap data (measured in pixels).
	 * @param bitmapHeight The height of the bitmap data (measured in pixels).
	 * @param alignment The horizontal alignment of the string.
	 * @param bitmapData Pointer to a pointer to the allocated bitmap data.
	 * @return True on success, false on failure.
	 */
	bool getCocoaStringBitmapData(void* aVisualStyledString, uint32 bitmapWidth, uint32 bitmapHeight, char* alignment, uint32** bitmapData);
	
	/**
	 * Copies BGRA bitmap data into a texture.
	 * @param textureNumber The texture number (texture name) of the texture.
	 * @param imageWidth The width of the bitmap data.
	 * @param imageHeight The height of the bitmap data.
	 * @param canUseRectExtension True if graphics system does support rectangular texture extension. False otherwise.
	 * @param bitmapData Pointer to pointer to the bitmap data
	 * @return True on success, false on failure.
	 */
	bool copyARGBBitmapDataToTexture(uint32 textureNumber, uint32 imageWidth, uint32 imageHeight, bool canUseRectExtension, uint16 format, uint16 dataType, uint32** bitmapData);
	
	/**
	 * Prints a VisualString to log.
	 * @param aVisualString The string that is to be printed.
	 * @remarks The passed-in file type is set to pointer-to-void to avoid include of VisualString c++ class definition.
	 */
	void logString(const void* aVisualString);
	
	/**
	 * Returns the identifier of the visualizer plug-in (as used on Mac OS X for preferences identification and as CFBundleIdentifier).
	 * @return The identifier of the visualizer plug-in.
	 */
	const char* getVisualPluginDomainIdentifier(void);
	
#endif
	
#ifdef __cplusplus
}
#endif


#endif /* VisualDispatch_h */
