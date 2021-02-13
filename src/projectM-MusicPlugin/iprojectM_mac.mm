// main drawing and interface code for Mac OS iTunes
// Mischa Spiegelmock, 2012

// based on the iTunes SDK example code

// https://www.fenestrated.net/mirrors/Apple%20Technotes%20(As%20of%202002)/tn/tn2016.html

#import "iprojectM.hpp"

#import <AppKit/AppKit.h>
#import <OpenGL/gl3.h>
#import <string.h>
#include "libprojectM/cocoatoprojectM.h"

#define kTVisualPluginName CFSTR("projectM")

extern "C" OSStatus iTunesPluginMainMachO( OSType inMessage, PluginMessageInfo *inMessageInfoPtr, void *refCon ) __attribute__((visibility("default")));

static bool _keypressProjectM( VisualPluginData * visualPluginData, projectMEvent eventType, NSEvent *event );

#if USE_SUBVIEW
@interface VisualView : NSOpenGLView
{
	VisualPluginData *	_visualPluginData;
}

@property (nonatomic, assign) VisualPluginData * visualPluginData;

- (void)drawRect:(NSRect)dirtyRect;
- (BOOL)acceptsFirstResponder;
- (BOOL)becomeFirstResponder;
- (BOOL)resignFirstResponder;
- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;

@end

#endif	// USE_SUBVIEW

void DrawVisual( VisualPluginData * visualPluginData )
{
	CGPoint where;

    VISUAL_PLATFORM_VIEW drawView = visualPluginData->subview;
    if (!visualPluginData->readyToDraw)
        return;
    
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // render
    visualPluginData->pm->renderFrame();

    glFlush();
    
    
    // TODO: artwork overlay
    
	// should we draw the info/artwork in the bottom-left corner?
	time_t		theTime = time( NULL );

	if ( theTime < visualPluginData->drawInfoTimeOut )
	{
		where = CGPointMake( 10, 10 );

		// if we have a song title, draw it (prefer the stream title over the regular name if we have it)
		NSString *				theString = NULL;

		if ( visualPluginData->streamInfo.streamTitle[0] != 0 )
			theString = [NSString stringWithCharacters:&visualPluginData->streamInfo.streamTitle[1] length:visualPluginData->streamInfo.streamTitle[0]];
		else if ( visualPluginData->trackInfo.name[0] != 0 )
			theString = [NSString stringWithCharacters:&visualPluginData->trackInfo.name[1] length:visualPluginData->trackInfo.name[0]];
		
		if ( theString != NULL )
		{
			NSDictionary *		attrs = [NSDictionary dictionaryWithObjectsAndKeys:[NSColor whiteColor], NSForegroundColorAttributeName, NULL];
			
			[theString drawAtPoint:where withAttributes:attrs];
		}

		// draw the artwork
		if ( visualPluginData->currentArtwork != NULL )
		{
			where.y += 20;

			[visualPluginData->currentArtwork drawAtPoint:where fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:0.75];
		}
	}
}


//-------------------------------------------------------------------------------------------------
//	UpdateArtwork
//-------------------------------------------------------------------------------------------------
//
void UpdateArtwork( VisualPluginData * visualPluginData, CFDataRef coverArt, UInt32 coverArtSize, UInt32 coverArtFormat )
{
	// release current image
	[visualPluginData->currentArtwork release];
	visualPluginData->currentArtwork = NULL;
	
	// create 100x100 NSImage* out of incoming CFDataRef if non-null (null indicates there is no artwork for the current track)
	if ( coverArt != NULL )
	{
		visualPluginData->currentArtwork = [[NSImage alloc] initWithData:(NSData *)coverArt];
		
		[visualPluginData->currentArtwork setSize:CGSizeMake( 100, 100 )];
	}
	
	UpdateInfoTimeOut( visualPluginData );
}

//-------------------------------------------------------------------------------------------------
//	InvalidateVisual
//-------------------------------------------------------------------------------------------------
//
void InvalidateVisual( VisualPluginData * visualPluginData )
{
	(void) visualPluginData;

#if USE_SUBVIEW
	// when using a custom subview, we invalidate it so we get our own draw calls
	[visualPluginData->subview setNeedsDisplay:YES];
#endif
}

//-------------------------------------------------------------------------------------------------
//	CreateVisualContext
//-------------------------------------------------------------------------------------------------
//
OSStatus ActivateVisual( VisualPluginData * visualPluginData, VISUAL_PLATFORM_VIEW destView, OptionBits options )
{
	OSStatus			status = noErr;

	visualPluginData->destView			= destView;
	visualPluginData->destRect			= [destView bounds];
	visualPluginData->destOptions		= options;
    visualPluginData->readyToDraw = false;

	UpdateInfoTimeOut( visualPluginData );

#if USE_SUBVIEW

	// NSView-based subview
	visualPluginData->subview = [[VisualView alloc] initWithFrame:visualPluginData->destRect];
	if ( visualPluginData->subview != NULL )
	{
		[visualPluginData->subview setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];

		[visualPluginData->subview setVisualPluginData:visualPluginData];

		[destView addSubview:visualPluginData->subview];
	}
	else
	{
		status = memFullErr;
	}

    
    [[visualPluginData->subview openGLContext] makeCurrentContext];

    
#endif
    NSLog(@"activate visual");
    if (visualPluginData->pm == NULL) {
        
        NSBundle* me = [NSBundle bundleForClass: VisualView.class];
        NSLog(@"main bundle: %@", [me bundlePath]);
        NSString* presetsPath = [me pathForResource:@"presets" ofType:nil];
        NSLog(@"presets path %@", presetsPath);
        
        initProjectM(visualPluginData, std::string([presetsPath UTF8String]));
        
        // correctly size it
        ResizeVisual(visualPluginData);
    }
    
    NSLog(@"activated visual");
    
	return status;
}

//-------------------------------------------------------------------------------------------------
//	MoveVisual
//-------------------------------------------------------------------------------------------------
//
OSStatus MoveVisual( VisualPluginData * visualPluginData, OptionBits newOptions )
{
	visualPluginData->destRect	  = [visualPluginData->destView bounds];
	visualPluginData->destOptions = newOptions;

	return noErr;
}

//-------------------------------------------------------------------------------------------------
//	DeactivateVisual
//-------------------------------------------------------------------------------------------------
//
OSStatus DeactivateVisual( VisualPluginData * visualPluginData )
{
#if USE_SUBVIEW
	[visualPluginData->subview removeFromSuperview];
	[visualPluginData->subview autorelease];
	visualPluginData->subview = NULL;
	[visualPluginData->currentArtwork release];
	visualPluginData->currentArtwork = NULL;
#endif

	visualPluginData->destView			= NULL;
	visualPluginData->destRect			= CGRectNull;
	visualPluginData->drawInfoTimeOut	= 0;
    visualPluginData->readyToDraw = false;

    if (visualPluginData->pm != NULL) {
        delete(visualPluginData->pm);
        visualPluginData->pm = NULL;
    }
	
	return noErr;
}

//-------------------------------------------------------------------------------------------------
//	ResizeVisual
//-------------------------------------------------------------------------------------------------
//
OSStatus ResizeVisual( VisualPluginData * visualPluginData )
{
    visualPluginData->destRect = [[NSScreen mainScreen] convertRectToBacking:([visualPluginData->subview bounds])];

    if (visualPluginData->pm != NULL) {
        visualPluginData->pm->projectM_resetGL(visualPluginData->destRect.size.width, visualPluginData->destRect.size.height);
        NSLog(@"resized to %@ %@", [NSNumber numberWithDouble: visualPluginData->destRect.size.width], [NSNumber numberWithDouble: visualPluginData->destRect.size.height]);
        
        visualPluginData->readyToDraw = true;
    }

	return noErr;
}

//-------------------------------------------------------------------------------------------------
//	ConfigureVisual
//-------------------------------------------------------------------------------------------------
//
OSStatus ConfigureVisual( VisualPluginData * visualPluginData )
{
	(void) visualPluginData;

	// load nib
	// show modal dialog
	// update settings
	// invalidate

	return noErr;
}

#pragma mark -

#if USE_SUBVIEW

@implementation VisualView

@synthesize visualPluginData = _visualPluginData;

- (id)initWithFrame:(NSRect)frame
{
    NSLog(@"initWithFrame called");
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
//        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
//        NSOpenGLPFAColorSize    , 24                           ,
//        NSOpenGLPFAAlphaSize    , 8                            ,
//        NSOpenGLPFADoubleBuffer ,
        NSOpenGLPFAAccelerated  ,
        0
    };
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    if (pixelFormat == nil)
    {
        NSLog(@"Unable to create pixel format.");
        return self;
    }
    self = [super initWithFrame:frame pixelFormat:pixelFormat];
    if (self == nil)
    {
        NSLog(@"Unable to create an OpenGL context.");
        return self;
    }

    NSLog(@"super initWithFrame called");

    return self;
}

//-------------------------------------------------------------------------------------------------
//	isOpaque
//-------------------------------------------------------------------------------------------------
//
- (BOOL)isOpaque
{
	// your custom views should always be opaque or iTunes will waste CPU time drawing behind you
	return YES;
}

//-------------------------------------------------------------------------------------------------
//	drawRect
//-------------------------------------------------------------------------------------------------
//
-(void)drawRect:(NSRect)dirtyRect
{
	if ( _visualPluginData != NULL )
	{
		DrawVisual( _visualPluginData );
	}
}

//-------------------------------------------------------------------------------------------------
//	acceptsFirstResponder
//-------------------------------------------------------------------------------------------------
//
- (BOOL)acceptsFirstResponder
{
	return YES;
}

//-------------------------------------------------------------------------------------------------
//	becomeFirstResponder
//-------------------------------------------------------------------------------------------------
//
- (BOOL)becomeFirstResponder
{
	return YES;
}

//-------------------------------------------------------------------------------------------------
//	resignFirstResponder
//-------------------------------------------------------------------------------------------------
//
- (BOOL)resignFirstResponder
{
	return YES;
}

- (void)keyDown:(NSEvent *)event {
    if ( _keypressProjectM( _visualPluginData, PROJECTM_KEYDOWN, event ) )
        return;

	[super keyDown:event];
}

- (void)keyUp:(NSEvent *)event {
    if ( _keypressProjectM( _visualPluginData, PROJECTM_KEYUP, event ) )
        return;
    
	[super keyUp:event];
}

static bool _keypressProjectM( VisualPluginData * visualPluginData, projectMEvent eventType, NSEvent *eventRec ) {
    // Handle key events here.
	// Do not eat the space bar, ESC key, TAB key, or the arrow keys: iTunes reserves those keys.
    
    projectMKeycode keycode = cocoa2pmKeycode(eventRec);
    projectMModifier mod = cocoa2pmModifier(eventRec);
    
    switch ([eventRec keyCode]) {
        case kVK_Tab:
        case kVK_Space:
        case kVK_LeftArrow:
        case kVK_RightArrow:
        case kVK_UpArrow:
        case kVK_DownArrow:
        case kVK_Escape:
            break;
            
        default:
            keypressProjectM( visualPluginData, eventType, keycode, mod );
            return true;
    }
    
    return false;
}

@end

#endif	// USE_SUBVIEW

#pragma mark -

//-------------------------------------------------------------------------------------------------
//	GetVisualName
//-------------------------------------------------------------------------------------------------
//
void GetVisualName( ITUniStr255 name )
{
	CFIndex length = CFStringGetLength( kTVisualPluginName );

	name[0] = (UniChar)length;
	CFStringGetCharacters( kTVisualPluginName, CFRangeMake( 0, length ), &name[1] );
}

//-------------------------------------------------------------------------------------------------
//	GetVisualOptions
//-------------------------------------------------------------------------------------------------
//
OptionBits GetVisualOptions( void )
{
	OptionBits		options = (kVisualUsesOnly3D | kVisualWantsIdleMessages);
	
#if USE_SUBVIEW
	options |= kVisualUsesSubview;
#endif

	return options;
}

//-------------------------------------------------------------------------------------------------
//	iTunesPluginMainMachO
//-------------------------------------------------------------------------------------------------
//
OSStatus iTunesPluginMainMachO( OSType message, PluginMessageInfo * messageInfo, void * refCon )
{
	OSStatus		status;
	
	(void) refCon;
	
	switch ( message )
	{
		case kPluginInitMessage:
			status = RegisterVisualPlugin( messageInfo );
			break;
			
		case kPluginCleanupMessage:
			status = noErr;
			break;
			
		default:
			status = unimpErr;
			break;
	}
	
	return status;
}
