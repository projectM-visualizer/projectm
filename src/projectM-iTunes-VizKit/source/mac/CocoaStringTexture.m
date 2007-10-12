/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: CocoaStringTexture.m
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

#import "CocoaStringTexture.h"

#import <mach-o/dyld.h>

#include "VisualDispatch.h"


CGContextRef CreateBitmapContext( int inWidth, int inHeight, void ** outBitmap );

__private_extern__ BOOL fontIsAvailable(NSString* fontName);
__private_extern__ OSStatus activateFont(NSString* fontName);


OSStatus initCocoaForCarbonBundle() {

	BOOL appLoadVal;
	
	appLoadVal = NSApplicationLoad();
	if (appLoadVal == NO) {
		NSLog (@"NSApplicationLoad failed\n");
	}

    return noErr;
}


BOOL fontIsAvailable(NSString* fontName) {

	BOOL fontIsActive = NO;

	NSString* fontNameStr;
	NSArray* fontArray;
	NSEnumerator* e;

	NSFontManager* fontManager = [NSFontManager sharedFontManager];

	fontArray = [fontManager availableFonts];
	e = [fontArray objectEnumerator];
	while ((fontNameStr = [e nextObject])) {
		if ([fontNameStr isEqualToString:fontName]) {
			fontIsActive = YES;
		}
		/* NSLog(fontNameStr); */
	}

	return fontIsActive;

}


OSStatus activateFont(NSString* fontName) {

	OSStatus status = noErr;
	OSErr err = noErr;
	
	BOOL fontIsActive = NO;

	NSString* fontNameStr;
	NSArray* fontArray;
	NSEnumerator* e;

	NSFontManager* fontManager = [NSFontManager sharedFontManager];

	fontArray = [fontManager availableFonts];
	e = [fontArray objectEnumerator];
	while ((fontNameStr = [e nextObject])) {
		if ([fontNameStr isEqualToString:fontName]) {
			fontIsActive = YES;
		}
	}

	if (fontIsActive == NO) {
		/* assume Font is available as resource of bundle */
		FSRef fsRef;
		FSSpec fsSpec;
		NSBundle* appBundle = [NSBundle bundleWithIdentifier:[NSString stringWithCString:getVisualPluginDomainIdentifier()]];
		/* NSString * fontsPath = [[appBundle resourcePath] stringByAppendingPathComponent:fontName]; */
		NSString* fontsPath = [appBundle resourcePath];
		status = FSPathMakeRef((UInt8 *)[fontsPath UTF8String], &fsRef, NULL);
		if (status == noErr) {
			err = FSGetCatalogInfo(&fsRef, kFSCatInfoNone, NULL, NULL, &fsSpec, NULL);
		}
		if (err == noErr) {
			ATSFontContainerRef anATSFontContainerRef;
			status = ATSFontActivateFromFileSpecification(&fsSpec, kATSFontContextLocal, kATSFontFormatUnspecified, NULL, kATSOptionFlagsDefault, &anATSFontContainerRef);
		} else {
			status = 1000;
		}
	}

	return status;

}


OSStatus getDimensionsOfStringBitmap(CFStringRef stringValue, UInt32* imageWidth, UInt32* imageHeight, const char* const fontNameStr, float* fontSize, float red, float green, float blue, UInt16 maxPixelWidth, UInt16 maxPixelHeight, const char* const alignment) {

	NSAutoreleasePool* localPool = [[NSAutoreleasePool alloc] init];
	
	NSString* aFontNameStr;
	OSStatus err;
	
	NSColor* textColor;
	NSColor* boxColor;
	NSColor* borderColor;
	NSSize marginSize;
	NSSize frameSize;
	
	NSMutableAttributedString* theAttributedString;
	NSMutableDictionary* fontAttribs;

	BOOL theFontIsAvailable;
	NSGraphicsContext* context = NULL;
	
	context = [NSGraphicsContext currentContext];
	[context saveGraphicsState];
	
	aFontNameStr = [[NSString alloc] initWithCStringNoCopy:(char*)fontNameStr length:strlen(fontNameStr) freeWhenDone:NO];
	
	theFontIsAvailable = fontIsAvailable(aFontNameStr);

	if (theFontIsAvailable == NO) {
		/* if the font is not available we will load it as private resource of our bundle */
		err = activateFont(aFontNameStr);
		if (err != noErr) {
			return 1000;
		} else {
			BOOL fontIsActive = NO;
			NSString* fontName;
			NSArray* fontArray;
			NSEnumerator* e;

			NSFontManager* fontManager = [NSFontManager sharedFontManager];

			fontArray = [fontManager availableFonts];
			e = [fontArray objectEnumerator];
			while ((fontName = [e nextObject])) {
				if ([fontName isEqualToString:aFontNameStr]) {
					fontIsActive = YES;
				}
			}
			if (fontIsActive == NO) {
				/* default fallback font */
				//[aFontNameStr release];
				//aFontNameStr = [[NSString alloc] initWithCStringNoCopy:"Lucida Grande" length:strlen("Lucida Grande") freeWhenDone:NO];
			}
		}
	}
	
    fontAttribs = [NSMutableDictionary dictionary];
	[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size: *fontSize] forKey:NSFontAttributeName];
	[fontAttribs setObject: [NSColor colorWithDeviceRed:red green:green blue:blue alpha:1.0f] forKey: NSForegroundColorAttributeName];

	theAttributedString = [[NSMutableAttributedString alloc] initWithString:(NSString*)stringValue attributes:fontAttribs];
	
	frameSize = [theAttributedString size];
	if (maxPixelWidth > 0 && maxPixelHeight > 0) {
		while ((frameSize.width > (float)maxPixelWidth) || (frameSize.height > (float)maxPixelHeight)) {
			*fontSize -= 0.1f;
			[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:*fontSize] forKey:NSFontAttributeName];
			[theAttributedString setAttributes:fontAttribs range:NSMakeRange(0, [theAttributedString length])];
			frameSize = [theAttributedString size];
		}
	} else if (maxPixelWidth > 0) {
		while (frameSize.width > (float)maxPixelWidth) {
			*fontSize -= 0.1f;
			[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:*fontSize] forKey:NSFontAttributeName];
			[theAttributedString setAttributes:fontAttribs range:NSMakeRange(0, [theAttributedString length])];
			frameSize = [theAttributedString size];
		}
	} else if (maxPixelHeight > 0) {
		while (frameSize.height > (float)maxPixelHeight) {
			*fontSize -= 0.1f;
			[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:*fontSize] forKey:NSFontAttributeName];
			[theAttributedString setAttributes:fontAttribs range:NSMakeRange(0, [theAttributedString length])];
			frameSize = [theAttributedString size];
		}
	}
	
	if (strcmp(alignment, "left") == 0) {
		[theAttributedString setAlignment:NSLeftTextAlignment range:NSMakeRange(0, [theAttributedString length])];
	} else if (strcmp(alignment, "center") == 0) {
		[theAttributedString setAlignment:NSCenterTextAlignment range:NSMakeRange(0, [theAttributedString length])];
	} else if (strcmp(alignment, "right") == 0) {
		[theAttributedString setAlignment:NSRightTextAlignment range:NSMakeRange(0, [theAttributedString length])];
	}
	
	textColor = [NSColor colorWithDeviceRed:red green:green blue:blue alpha:1.0f];
	boxColor = [NSColor colorWithDeviceRed:0.0f green:0.0f blue:0.0f alpha:0.0f];
	borderColor = [NSColor colorWithDeviceRed:0.0f green:0.0f blue:0.0f alpha:0.0f];

	marginSize.width = 4.0f;
	marginSize.height = 4.0f;

	frameSize = [theAttributedString size];
	frameSize.width += marginSize.width * 2.0f;
	frameSize.height += marginSize.height * 2.0f;

	*imageWidth = (UInt16)frameSize.width;
	*imageHeight = (UInt16)frameSize.height;

    [theAttributedString release];

	[aFontNameStr release];
	
	[context restoreGraphicsState];

    [localPool release];

    return noErr;
}


OSStatus getStringBitmapData(CFStringRef stringValue, UInt32 bitmapWidth, UInt32 bitmapHeight, const char* const fontNameStr, float fontSize, float red, float green, float blue, const char* const alignment, UInt32** bitmapOut) {

	NSAutoreleasePool* localPool = [[NSAutoreleasePool alloc] init];
	
	NSString* aFontNameStr;
	OSStatus err;
	
	NSColor* textColor;
	NSColor* boxColor;
	NSColor* borderColor;
	NSSize frameSize;
	
	NSMutableAttributedString* theAttributedString;
	NSMutableDictionary* fontAttribs;

	NSImage* image;
	NSBitmapImageRep* bitmap;
	unsigned int bytesPerRow, bitsPerPixel, i;
	UInt32* from;
	UInt32* to;
	BOOL hasAlpha;
	BOOL theFontIsAvailable;
	NSGraphicsContext* context = NULL;
	
	context = [NSGraphicsContext currentContext];
	[context saveGraphicsState];
	
	aFontNameStr = [[NSString alloc] initWithCStringNoCopy:(char*)fontNameStr length:strlen(fontNameStr) freeWhenDone:NO];
	
	theFontIsAvailable = fontIsAvailable(aFontNameStr);

	if (theFontIsAvailable == NO) {
		/* if the font is not available we will load it as private resource of our bundle */
		err = activateFont(aFontNameStr);
		if (err != noErr) {
			return 1001;
		} else {
			BOOL fontIsActive = NO;
			NSString* fontName;
			NSArray* fontArray;
			NSEnumerator* e;

			NSFontManager* fontManager = [NSFontManager sharedFontManager];

			fontArray = [fontManager availableFonts];
			e = [fontArray objectEnumerator];
			while ((fontName = [e nextObject])) {
				if ([fontName isEqualToString:aFontNameStr]) {
					fontIsActive = YES;
				}
			}
			if (fontIsActive == NO) {
				/* default fallback font */
				//[aFontNameStr release];
				//aFontNameStr = [[NSString alloc] initWithCStringNoCopy:"Lucida Grande" length:strlen("Lucida Grande") freeWhenDone:NO];
			}
		}
	}
	
    fontAttribs = [NSMutableDictionary dictionary];
	[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size: fontSize] forKey:NSFontAttributeName];
	[fontAttribs setObject: [NSColor colorWithDeviceRed:red green:green blue:blue alpha:1.0f] forKey: NSForegroundColorAttributeName];
	/*
	//colorWithAlphaComponent
	// CGImageCreate
	// kCGImageAlphaPremultipliedFirst
	// CGContextSetShouldSmoothFonts
	
	// http: //www.idevgames.com/forum/archive/index.php/t-5750.html (CGImageCreate)
	
	//http: //www.idevgames.com/forum/archive/index.php/t-7560.html
	*/

	theAttributedString = [[NSMutableAttributedString alloc] initWithString:(NSString*)stringValue attributes:fontAttribs];
	
	if (strcmp(alignment, "left") == 0) {
		[theAttributedString setAlignment:NSLeftTextAlignment range:NSMakeRange(0, [theAttributedString length])];
	} else if (strcmp(alignment, "center") == 0) {
		[theAttributedString setAlignment:NSCenterTextAlignment range:NSMakeRange(0, [theAttributedString length])];
	} else if (strcmp(alignment, "right") == 0) {
		[theAttributedString setAlignment:NSRightTextAlignment range:NSMakeRange(0, [theAttributedString length])];
	}
	
	textColor = [NSColor colorWithDeviceRed:red green:green blue:blue alpha:1.0f];
	boxColor = [NSColor colorWithDeviceRed:0.0f green:0.0f blue:0.0f alpha:0.0f];
	borderColor = [NSColor colorWithDeviceRed:0.0f green:0.0f blue:0.0f alpha:0.0f];

	frameSize.width = (float)bitmapWidth;
	frameSize.height = (float)bitmapHeight;
	
	image = [[NSImage alloc] initWithSize:frameSize];
	
	[image setCacheMode:NSImageCacheNever];
	
NS_DURING
		[image lockFocus];
NS_HANDLER
		NSLog(@"Exception thrown in createStringBitmap (NSImage lockFocus)");
		[image release];
		[theAttributedString release];
		[aFontNameStr release];
		[context restoreGraphicsState];
		[localPool release];
		return 1002;
NS_ENDHANDLER
	
	[boxColor set];
	NSRectFill(NSMakeRect (0.0f, 0.0f, frameSize.width, frameSize.height));

	/* [borderColor set]; 
	NSFrameRect (NSMakeRect (0.0f, 0.0f, frameSize.width, frameSize.height));
	[textColor set];
	*/
	
	float imageWidth = [theAttributedString size].width;
	float imageHeight = [theAttributedString size].height;
	[theAttributedString drawInRect:NSMakeRect(0.0f, 0.0f, imageWidth, imageHeight)];
	
	bitmap = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect(0.0f, 0.0f, frameSize.width, frameSize.height)];

	[image unlockFocus];

	/* colorSpaceName:NSCalibratedRGBColorSpace, NSDeviceRGBColorSpace */
	bytesPerRow = [bitmap bytesPerRow];
	bitsPerPixel = [bitmap bitsPerPixel];
	hasAlpha = [bitmap hasAlpha];
	/* flip image */
	from = (UInt32*)[bitmap bitmapData];
	to = *bitmapOut;
	for (i = 0; i < bitmapHeight; i++) {
		memcpy((to + (bytesPerRow / 4) * (bitmapHeight - i - 1)), (from + (bytesPerRow / 4) * i), bytesPerRow);
	}
	
	[bitmap release];
	[image release];

    [theAttributedString release];

	[aFontNameStr release];
	
	[context restoreGraphicsState];

    [localPool release];

    return noErr;
}


void logCFString(CFStringRef aString) {
	NSLog(@"\"%@\"", (NSString*)aString);
}


CGContextRef CreateBitmapContext( int inWidth, int inHeight, void ** outBitmap )
{
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    void *          bitmapData;
    int             bitmapByteCount;
    int             bitmapBytesPerRow;

    /* each pixel is represented by four bytes
    // (8 bits each of R, G, B, and alpha)
	*/
    bitmapBytesPerRow   = (inWidth * 4);
    bitmapByteCount     = (bitmapBytesPerRow * inHeight);

    /* create an RGB color space */
    colorSpace = CGColorSpaceCreateDeviceRGB();

    /* create the bitmap */
    bitmapData = malloc( bitmapByteCount );
    if( bitmapData != NULL )
    {

        /* create the context */
        context = CGBitmapContextCreate(    bitmapData,
                                            inWidth,
                                            inHeight,
                                            8,              /* bits per component */
                                            bitmapBytesPerRow,
                                            colorSpace,
                                            kCGImageAlphaPremultipliedLast);

        if( context != NULL )
        {
            /* Return the bitmap to the caller */
            /* so it can later be freed */
            if( outBitmap != NULL )
            {
                *outBitmap = bitmapData;
            }
        }
        else
        {
            /* the context couldnt be created for some reason,
				and we have no use for the bitmap without the context */
            free( bitmapData );
        }
    }

    /* the context retains the color space, so we can release it */
    CGColorSpaceRelease( colorSpace );

    return context;
}
