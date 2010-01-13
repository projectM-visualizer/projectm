/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: CocoaStringTexture.m
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

#import "CocoaStringTexture.h"

#import <mach-o/dyld.h>

#include "VisualDispatch.h"


CGContextRef CreateBitmapContext( int inWidth, int inHeight, void ** outBitmap );

BOOL appLoadVal = NO;


OSStatus initCocoaForCarbonBundle() {

	if (appLoadVal == YES) {
		return noErr;
	}
	
	appLoadVal = NSApplicationLoad();
	if (appLoadVal == NO) {
		NSLog (@"VizKit: NSApplicationLoad failed\n");
	}

    return noErr;
}


Boolean cocoaFontIsAvailable(NSString* fontName) {

	initCocoaForCarbonBundle();

	Boolean result = false;

	NSString* fontNameStr;
	NSArray* fontArray;
	NSEnumerator* e;

	NSFontManager* fontManager = [NSFontManager sharedFontManager];

	fontArray = [fontManager availableFonts];
	e = [fontArray objectEnumerator];
	while ((fontNameStr = [e nextObject])) {
		if ([fontNameStr isEqualToString:fontName]) {
			result = true;
		}
		/* NSLog(fontNameStr); */
	}
	
	return result;

}


OSStatus activateCocoaFont(NSString* fontName) {

	initCocoaForCarbonBundle();

	OSStatus status = noErr;
	OSErr err = noErr;

	/* assume Font is available as resource of bundle */
	FSRef fsRef;
	FSSpec fsSpec;
	NSBundle* appBundle = [NSBundle bundleWithIdentifier:[NSString stringWithCString:getVisualPluginDomainIdentifier() encoding:NSUTF8StringEncoding]];
	NSString* fontsPath = [appBundle resourcePath];
	fontsPath = [fontsPath stringByAppendingString: @"/DEVROYE_.otf"];
	status = FSPathMakeRef((UInt8 *)[fontsPath UTF8String], &fsRef, NULL);
	if (status == noErr) {
		err = FSGetCatalogInfo(&fsRef, kFSCatInfoNone, NULL, NULL, &fsSpec, NULL);
	}
	if (err == noErr) {
		ATSFontContainerRef anATSFontContainerRef;
		status = ATSFontActivateFromFileSpecification(&fsSpec, kATSFontContextLocal, kATSFontFormatUnspecified, NULL, kATSOptionFlagsDefault, &anATSFontContainerRef);
		// ATSFontActivateFromFileReference
	} else {
		status = 1001;
	}

	return status;

}


bool getDimensionsOfStringBitmap(CFStringRef stringValue, UInt32* imageWidth, UInt32* imageHeight, const char* const fontNameStr, double* fontSize, double red, double green, double blue, UInt16 maxPixelWidth, UInt16 maxPixelHeight, const char* const alignment) {

	initCocoaForCarbonBundle();
	
	if (!stringValue) {
		return false;
	}
	
	NSAutoreleasePool* localPool = [[NSAutoreleasePool alloc] init];
	
	NSString* aFontNameStr;
	
	NSColor* textColor;
	NSColor* boxColor;
	NSColor* borderColor;
	NSSize marginSize;
	NSSize frameSize;
	
	NSMutableAttributedString* theAttributedString;
	NSMutableDictionary* fontAttribs;

	NSGraphicsContext* context = NULL;
	
	context = [NSGraphicsContext currentContext];
	[context saveGraphicsState];
	
	aFontNameStr = [[NSString alloc] initWithCStringNoCopy:(char*)fontNameStr length:strlen(fontNameStr) freeWhenDone:NO];
	
    fontAttribs = [NSMutableDictionary dictionary];
	[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size: (float)*fontSize] forKey:NSFontAttributeName];
	[fontAttribs setObject: [NSColor colorWithDeviceRed:(float)red green:(float)green blue:(float)blue alpha:1.0f] forKey: NSForegroundColorAttributeName];

	theAttributedString = [[NSMutableAttributedString alloc] initWithString:(NSString*)stringValue attributes:fontAttribs];
	
	frameSize = [theAttributedString size];
	if (maxPixelWidth > 0 && maxPixelHeight > 0) {
		while ((frameSize.width > (float)maxPixelWidth) || (frameSize.height > (float)maxPixelHeight)) {
			*fontSize -= 0.1f;
			[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:(float)*fontSize] forKey:NSFontAttributeName];
			[theAttributedString setAttributes:fontAttribs range:NSMakeRange(0, [theAttributedString length])];
			frameSize = [theAttributedString size];
		}
	} else if (maxPixelWidth > 0) {
		while (frameSize.width > (float)maxPixelWidth) {
			*fontSize -= 0.1f;
			[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:(float)*fontSize] forKey:NSFontAttributeName];
			[theAttributedString setAttributes:fontAttribs range:NSMakeRange(0, [theAttributedString length])];
			frameSize = [theAttributedString size];
		}
	} else if (maxPixelHeight > 0) {
		while (frameSize.height > (float)maxPixelHeight) {
			*fontSize -= 0.1f;
			[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:(float)*fontSize] forKey:NSFontAttributeName];
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
	
	textColor = [NSColor colorWithDeviceRed:(float)red green:(float)green blue:(float)blue alpha:1.0f];
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

    return true;
}


bool getStringBitmapData(CFStringRef stringValue, UInt32 bitmapWidth, UInt32 bitmapHeight, const char* const fontNameStr, double fontSize, double red, double green, double blue, const char* const alignment, UInt32** bitmapOut) {

	initCocoaForCarbonBundle();
	
	NSAutoreleasePool* localPool = [[NSAutoreleasePool alloc] init];
	
	bool success = true;
	
	NSString* aFontNameStr;
	
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
	NSGraphicsContext* context = NULL;
	float imageWidth = 0.0f;
	float imageHeight = 0.0f;
	
	context = [NSGraphicsContext currentContext];
	[context saveGraphicsState];
	
	aFontNameStr = [[NSString alloc] initWithCStringNoCopy:(char*)fontNameStr length:strlen(fontNameStr) freeWhenDone:NO];
	
    fontAttribs = [NSMutableDictionary dictionary];
	[fontAttribs setObject: [NSFont fontWithName:aFontNameStr size:(float)fontSize] forKey:NSFontAttributeName];
	[fontAttribs setObject: [NSColor colorWithDeviceRed:(float)red green:(float)green blue:(float)blue alpha:1.0f] forKey:NSForegroundColorAttributeName];
	
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
	
	textColor = [NSColor colorWithDeviceRed:(float)red green:(float)green blue:(float)blue alpha:1.0f];
	
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

	/*
	[boxColor set];
	NSRectFill(NSMakeRect(0.0f, 0.0f, frameSize.width, frameSize.height));

	[borderColor set]; 
	NSFrameRect(NSMakeRect(0.0f, 0.0f, frameSize.width, frameSize.height));
	[textColor set];
	*/
	
	imageWidth = [theAttributedString size].width;
	imageHeight = [theAttributedString size].height;
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

    return success;
}


void logCFString(CFStringRef aString) {
	initCocoaForCarbonBundle();
	NSLog(@"\"%@\"", (NSString*)aString);
}


CGContextRef CreateBitmapContext( int inWidth, int inHeight, void ** outBitmap )
{
	initCocoaForCarbonBundle();
	
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


NSComparisonResult fontNameListCompare(id obj1, id obj2, void *context);

NSComparisonResult fontNameListCompare(id obj1, id obj2, void *context) {
    /* Do the comparison here */
	/* (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontFamilyMembersArray, k), CFSTR("fontFamilyMemberName")); */
	return [(NSString*)[obj1 objectForKey:@"fontFamilyName"] compare:[obj2 objectForKey:@"fontFamilyName"]];
}


NSArray* getCocoaListOfFontNames() {

	initCocoaForCarbonBundle();

	temporaryAutoreleasePool = [[NSAutoreleasePool alloc] init];

	NSString* fontFamilyName = nil;
	NSArray* fontFamilyArray = nil;
	NSEnumerator* fontFamilyArrayEnumerator = nil;
	NSMutableArray* fontArray = nil;
	NSMutableArray* fontFamilyMembersArray = nil;
	NSArray* fontFamilyMembers = nil;
	NSMutableDictionary* fontFamilyMembersDict = nil;
	NSString* fontFamilyMemberFontName = nil;
	NSString* fontFamilyMemberAttribute = nil;
	NSMutableDictionary* fontFamilyDict = nil;
	NSFontManager* fontManager = nil;
	unsigned i = 0;
	unsigned fontFamilyMembersCount = 0;
	
	fontArray = [[NSMutableArray alloc] init];
	
	fontManager = [NSFontManager sharedFontManager];
	
	fontFamilyArray = [fontManager availableFontFamilies];
	
	fontFamilyArrayEnumerator = [fontFamilyArray objectEnumerator];
	while ((fontFamilyName = [fontFamilyArrayEnumerator nextObject])) {

		fontFamilyDict = [[NSMutableDictionary alloc] initWithCapacity:0];
		[fontFamilyDict setObject:fontFamilyName forKey:@"fontFamilyName"];
		
		fontFamilyMembers = [[NSFontManager sharedFontManager] availableMembersOfFontFamily:fontFamilyName];
		
		fontFamilyMembersCount = 0;
		if (fontFamilyMembers != nil) {
			fontFamilyMembersCount = [fontFamilyMembers count];
		}
		
		if (fontFamilyMembersCount > 0) {
		
			fontFamilyMembersArray = [[NSMutableArray alloc] init];
			
			for (i = 0; i < fontFamilyMembersCount; i++) {
				fontFamilyMembersDict = [[NSMutableDictionary alloc] initWithCapacity:0];
				
				fontFamilyMemberFontName = [[NSString alloc] initWithString:[[fontFamilyMembers objectAtIndex:i] objectAtIndex:0]];
				fontFamilyMemberAttribute = [[NSString alloc] initWithString:[[fontFamilyMembers objectAtIndex:i] objectAtIndex:1]];

				[fontFamilyMembersDict setObject:fontFamilyMemberFontName forKey:@"fontFamilyMemberName"];
				[fontFamilyMembersDict setObject:fontFamilyMemberAttribute forKey:@"fontFamilyMemberAttribute"];
				[fontFamilyMembersArray addObject:fontFamilyMembersDict];
				
				[fontFamilyMemberFontName release];
				[fontFamilyMemberAttribute release];
				
				[fontFamilyMembersDict release];
			}
			[fontFamilyDict setObject:fontFamilyMembersArray forKey:@"fontFamilyMembersArray"];
			
			[fontFamilyMembersArray release];
		
		} else {
			[fontFamilyDict setObject:nil forKey:@"fontFamilyMembersArray"];
		}

		[fontArray addObject:fontFamilyDict];
		
		[fontFamilyDict release];
		
	}
	
	[fontArray sortUsingFunction:fontNameListCompare context:nil];
	
	[fontArray autorelease];
	
	return fontArray;
}


void releaseTemporaryAutoreleasePool() {
	initCocoaForCarbonBundle();
	
	[temporaryAutoreleasePool release];
	temporaryAutoreleasePool = nil;
}
