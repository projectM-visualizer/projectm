/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: CocoaStringTexture.h
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


#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>


NSAutoreleasePool* temporaryAutoreleasePool;


/* CocoaStringTexture generates a string texture by means of Cocoa framework. */

/**
 * Initializes the Cocoa graphics engine for the Carbon app.
 * NSApplicationLoad() is called. Without call of NSApplicationLoad() Cocoa graphics will not be generated.
 * The Cocoa documentation explains about NSApplicationLoad(): 
 * "Startup function to call when running Cocoa code from a Carbon application.
 * You typically call this before calling other Cocoa code in a Plug-in loaded into a primarily Carbon application."
 * @remarks To access the function with CFBundleGetFunctionPointerForName() the visibility is set to default.
 */
__attribute__((visibility("default")))
OSStatus initCocoaForCarbonBundle(void);

/**
 * Returns the dimensions (width and height in pixels) of an ARGB bitmap of a styled string.
 * @param stringValue The string value that should be converted into a texture.
 * @param[out] bitmapWidth The width of the string (in pixels).
 * @param[out] bitmapHeight The height of the string (in pixels).
 * @param fontNameStr The name of the font.
 * @param[in,out] fontSize The font size. On return the fontSize is set and with regard to the optional contraints maxPixelWidth and maxPixelHeight.
 * @param red The red color component of the text font.
 * @param green The green color component of the text font.
 * @param blue The blue color component of the text font.
 * @param maxPixelWidth The requested maximum width of the texture. If 0 the argument is ignored.
 * @param maxPixelHeight The requested maximum height of the texture. If 0 the argument is ignored.
 * @param alignment The horizontal alignment of the string. Possible values: left, center, right.
 * @remarks To access the function with CFBundleGetFunctionPointerForName() the visibility is set to default.
*/
__attribute__((visibility("default")))
bool getDimensionsOfStringBitmap(CFStringRef stringValue, UInt32* bitmapWidth, UInt32* bitmapHeight, const char* const fontNameStr, double* fontSize, double red, double green, double blue, UInt16 maxPixelWidth, UInt16 maxPixelHeight, const char* const alignment);

/**
 * Writes ARGB pixel bitmap data into provided buffer.
 * @param stringValue The string value that should be converted into a texture.
 * @param bitmapWidth The width of the bitmap (in pixels). The size of the bitmap can be different from the exact size of the string so that it may fit for a power-of-2 texture.
 * @param bitmapHeight The height of the bitmap (in pixels). The size of the bitmap can be different from the exact size of the string so that it may fit for a power-of-2 texture.
 * @param fontNameStr The name of the font.
 * @param fontSize The font size.
 * @param red The red color component of the text font.
 * @param green The green color component of the text font.
 * @param blue The blue color component of the text font.
 * @param maxPixelWidth The requested maximum width of the texture. If 0 the argument is ignored.
 * @param maxPixelHeight The requested maximum height of the texture. If 0 the argument is ignored.
 * @param alignment The horizontal alignment of the string. Possible values: left, center, right.
 * @param[out] bitmapOut Pointer to pointer to the buffer into which the pixel bitmap data is written.
 * @remarks The bitmapOut buffer is assumed to have allocated sufficient memory. To access the function with CFBundleGetFunctionPointerForName() the visibility is set to default.
*/
__attribute__((visibility("default")))
bool getStringBitmapData(CFStringRef stringValue, UInt32 bitmapWidth, UInt32 bitmapHeight, const char* const fontNameStr, double fontSize, double red, double green, double blue, const char* const alignment, UInt32** bitmapOut);

/**
 * Returns a list of names of installed fonts.
 * @return A list of names of installed fonts.
 */
__attribute__((visibility("default")))
NSArray* getCocoaListOfFontNames(void);

/**
 * Answers the question whether a font is currently available.
 * @param fontName The name of the font.
 * @return noErr if font is available.
 */
__attribute__((visibility("default")))
Boolean cocoaFontIsAvailable(NSString* fontName);

/**
 * Activates a font by loading a font resource.
 * @param fontName The name of the font.
 * @return Result of operation.
 */
__attribute__((visibility("default")))
OSStatus activateCocoaFont(NSString* fontName);

/**
 * Prints a CFString to log.
 * @param aString The string that is to be printed.
 * @remarks To access the function with CFBundleGetFunctionPointerForName() the visibility is set to default.
 */
__attribute__((visibility("default")))
void logCFString(CFStringRef aString);

/**
 * Releases the temporary autorelease pool.
 */
__attribute__((visibility("default")))
void releaseTemporaryAutoreleasePool(void);
