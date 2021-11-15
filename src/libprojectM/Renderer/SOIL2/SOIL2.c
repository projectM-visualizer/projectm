/*
 	Fork by Martin Lucas Golini
 	 
 	Original author
	Jonathan Dummer
	2007-07-26-10.36

	Simple OpenGL Image Library 2

	Public Domain
	using Sean Barret's stb_image as a base

	Thanks to:
	* Sean Barret - for the awesome stb_image
	* Dan Venkitachalam - for finding some non-compliant DDS files, and patching some explicit casts
	* everybody at gamedev.net
*/

#define SOIL_CHECK_FOR_GL_ERRORS 0

#if defined( __APPLE_CC__ ) || defined ( __APPLE__ )
	#include <TargetConditionals.h>

	#if defined( __IPHONE__ ) || ( defined( TARGET_OS_IPHONE ) && TARGET_OS_IPHONE ) || ( defined( TARGET_IPHONE_SIMULATOR ) && TARGET_IPHONE_SIMULATOR )
		#define SOIL_PLATFORM_IOS
		#include <dlfcn.h>
	#else
		#define SOIL_PLATFORM_OSX
	#endif
#elif defined( __ANDROID__ ) || defined( ANDROID )
	#define SOIL_PLATFORM_ANDROID
#elif ( defined ( linux ) || defined( __linux__ ) || defined( __FreeBSD__ ) || defined(__OpenBSD__) || defined( __NetBSD__ ) || defined( __DragonFly__ ) || defined( __SVR4 ) )
	#define SOIL_X11_PLATFORM
#endif

#if ( defined( SOIL_PLATFORM_IOS ) || defined( SOIL_PLATFORM_ANDROID ) ) && ( !defined( SOIL_GLES1 ) && !defined( SOIL_GLES2 ) )
	#define SOIL_GLES2
#endif

#if ( defined( SOIL_GLES2 ) || defined( SOIL_GLES1 ) ) && !defined( SOIL_NO_EGL ) && !defined( SOIL_PLATFORM_IOS )
	#include <EGL/egl.h>
#endif

#if defined( SOIL_GLES3 )
    #include <GLES3/gl3.h>

	#define APIENTRY GL_APIENTRY
#endif

#if defined( SOIL_GLES2 )
	#ifdef SOIL_PLATFORM_IOS
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
	#else
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
	#endif

	#define APIENTRY GL_APIENTRY
#elif defined( SOIL_GLES1 )
	#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
	#endif
	#ifdef SOIL_PLATFORM_IOS
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
	#else
		#include <GLES/gl.h>
		#include <GLES/glext.h>
	#endif

	#define APIENTRY GL_APIENTRY
#else

#if defined( __WIN32__ ) || defined( _WIN32 ) || defined( WIN32 )
	#define SOIL_PLATFORM_WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wingdi.h>
	#include <GL/gl.h>
	
	#ifndef GL_UNSIGNED_SHORT_4_4_4_4
	#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
	#endif
	#ifndef GL_UNSIGNED_SHORT_5_5_5_1
	#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
	#endif
	#ifndef GL_UNSIGNED_SHORT_5_6_5
	#define GL_UNSIGNED_SHORT_5_6_5 0x8363
	#endif
#elif defined(__APPLE__) || defined(__APPLE_CC__)
	/*	I can't test this Apple stuff!	*/
	#include <OpenGL/gl.h>
	#include <Carbon/Carbon.h>
	#define APIENTRY
#elif defined( SOIL_X11_PLATFORM )
	#include <GL/gl.h>
	#include <GL/glx.h>
#else
	#include <GL/gl.h>
#endif

#endif

#ifndef GL_BGRA
#define GL_BGRA                                             0x80E1
#endif

#ifndef GL_RG
#define GL_RG                             0x8227
#endif

#include "SOIL2.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "image_helper.h"
#include "image_DXT.h"
#include "pvr_helper.h"
#include "pkm_helper.h"

#include <stdlib.h>
#include <string.h>

/*	error reporting	*/
const char *result_string_pointer = "SOIL initialized";

/*	for loading cube maps	*/
enum{
	SOIL_CAPABILITY_UNKNOWN = -1,
	SOIL_CAPABILITY_NONE = 0,
	SOIL_CAPABILITY_PRESENT = 1
};
static int has_cubemap_capability = SOIL_CAPABILITY_UNKNOWN;
int query_cubemap_capability( void );
#define SOIL_TEXTURE_WRAP_R					0x8072
#define SOIL_CLAMP_TO_EDGE					0x812F
#define SOIL_NORMAL_MAP						0x8511
#define SOIL_REFLECTION_MAP					0x8512
#define SOIL_TEXTURE_CUBE_MAP				0x8513
#define SOIL_TEXTURE_BINDING_CUBE_MAP		0x8514
#define SOIL_TEXTURE_CUBE_MAP_POSITIVE_X	0x8515
#define SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X	0x8516
#define SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y	0x8517
#define SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y	0x8518
#define SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z	0x8519
#define SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z	0x851A
#define SOIL_PROXY_TEXTURE_CUBE_MAP			0x851B
#define SOIL_MAX_CUBE_MAP_TEXTURE_SIZE		0x851C
/*	for non-power-of-two texture	*/
#define SOIL_IS_POW2( v ) ( ( v & ( v - 1 ) ) == 0 )
static int has_NPOT_capability = SOIL_CAPABILITY_UNKNOWN;
int query_NPOT_capability( void );
/*	for texture rectangles	*/
static int has_tex_rectangle_capability = SOIL_CAPABILITY_UNKNOWN;
int query_tex_rectangle_capability( void );
#define SOIL_TEXTURE_RECTANGLE_ARB				0x84F5
#define SOIL_MAX_RECTANGLE_TEXTURE_SIZE_ARB		0x84F8
/*	for using DXT compression	*/
static int has_DXT_capability = SOIL_CAPABILITY_UNKNOWN;
int query_DXT_capability( void );
#define SOIL_RGB_S3TC_DXT1		0x83F0
#define SOIL_RGBA_S3TC_DXT1		0x83F1
#define SOIL_RGBA_S3TC_DXT3		0x83F2
#define SOIL_RGBA_S3TC_DXT5		0x83F3
typedef void (APIENTRY * P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);
static P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC soilGlCompressedTexImage2D = NULL;

typedef void (APIENTRY *P_SOIL_GLGENERATEMIPMAPPROC)(GLenum target);
static P_SOIL_GLGENERATEMIPMAPPROC soilGlGenerateMipmap = NULL;

static int has_gen_mipmap_capability = SOIL_CAPABILITY_UNKNOWN;
static int query_gen_mipmap_capability( void );

static int has_PVR_capability = SOIL_CAPABILITY_UNKNOWN;
int query_PVR_capability( void );
static int has_BGRA8888_capability = SOIL_CAPABILITY_UNKNOWN;
int query_BGRA8888_capability( void );
static int has_ETC1_capability = SOIL_CAPABILITY_UNKNOWN;
int query_ETC1_capability( void );

/* GL_IMG_texture_compression_pvrtc */
#define SOIL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#define SOIL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#define SOIL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#define SOIL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03
#define SOIL_GL_ETC1_RGB8_OES                                     0x8D64

#if defined( SOIL_X11_PLATFORM ) || defined( SOIL_PLATFORM_WIN32 ) || defined( SOIL_PLATFORM_OSX )
typedef const GLubyte *(APIENTRY * P_SOIL_glGetStringiFunc) (GLenum, GLuint);
static P_SOIL_glGetStringiFunc soilGlGetStringiFunc = NULL;

static int isAtLeastGL3()
{
	static int is_gl3 = SOIL_CAPABILITY_UNKNOWN;

	if ( SOIL_CAPABILITY_UNKNOWN == is_gl3 )
	{
		const char * verstr	= (const char *) glGetString( GL_VERSION );
		is_gl3				= ( verstr && ( atoi(verstr) >= 3 ) );
	}

	return is_gl3;
}
#endif

#ifdef SOIL_PLATFORM_WIN32
static int soilTestWinProcPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	return 1;
}
#endif

void * SOIL_GL_GetProcAddress(const char *proc)
{
	void *func = NULL;

#if defined( SOIL_PLATFORM_IOS )
	func = dlsym( RTLD_DEFAULT, proc );
#elif defined( SOIL_GLES2 ) || defined( SOIL_GLES1 ) || defined( SOIL_GLES3 )
	#ifndef SOIL_NO_EGL
		func = eglGetProcAddress( proc );
	#else
		func = NULL;
    #endif
#elif defined( SOIL_PLATFORM_WIN32 )
	func =  wglGetProcAddress( proc );

	if (!soilTestWinProcPointer((const PROC)func))
		func = NULL;
#elif defined( SOIL_PLATFORM_OSX )
	/*	I can't test this Apple stuff!	*/
	CFBundleRef bundle;
	CFURLRef bundleURL =
	CFURLCreateWithFileSystemPath(
								  kCFAllocatorDefault,
								  CFSTR("/System/Library/Frameworks/OpenGL.framework"),
								  kCFURLPOSIXPathStyle,
								  true );
	CFStringRef extensionName =
	CFStringCreateWithCString(
							  kCFAllocatorDefault,
							  proc,
							  kCFStringEncodingASCII );
	bundle = CFBundleCreate( kCFAllocatorDefault, bundleURL );
	assert( bundle != NULL );

	func = CFBundleGetFunctionPointerForName( bundle, extensionName );

	CFRelease( bundleURL );
	CFRelease( extensionName );
	CFRelease( bundle );
#elif defined( SOIL_X11_PLATFORM )
	func =
#if !defined(GLX_VERSION_1_4)
	glXGetProcAddressARB
#else
	glXGetProcAddress
#endif
	( (const GLubyte *)proc );
#endif

	return func;
}

/* Based on the SDL2 implementation */
int SOIL_GL_ExtensionSupported(const char *extension)
{
	const char *extensions;
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');

	if (where || *extension == '\0')
	{
		return 0;
	}

	#if defined( SOIL_X11_PLATFORM ) || defined( SOIL_PLATFORM_WIN32 ) || defined( SOIL_PLATFORM_OSX )
	/* Lookup the available extensions */
	if ( isAtLeastGL3() )
	{
		GLint num_exts = 0;
		GLint i;

		if ( NULL == soilGlGetStringiFunc )
		{
			soilGlGetStringiFunc = (P_SOIL_glGetStringiFunc)SOIL_GL_GetProcAddress("glGetStringi");

			if ( NULL == soilGlGetStringiFunc )
			{
				return 0;
			}
		}

		#ifndef GL_NUM_EXTENSIONS
		#define GL_NUM_EXTENSIONS 0x821D
		#endif
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts);
		for (i = 0; i < num_exts; i++)
		{
			const char *thisext = (const char *) soilGlGetStringiFunc(GL_EXTENSIONS, i);

			if (strcmp(thisext, extension) == 0)
			{
				return 1;
			}
		}

		return 0;
	}
	#endif

	/* Try the old way with glGetString(GL_EXTENSIONS) ... */
	extensions = (const char *) glGetString(GL_EXTENSIONS);

	if (!extensions)
	{
		return 0;
	}

	/*
	 * It takes a bit of care to be fool-proof about parsing the OpenGL
	 * extensions string. Don't be fooled by sub-strings, etc.
	 */
	start = extensions;

	for (;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return 1;

		start = terminator;
	}

	return 0;
}

/*	other functions	*/
unsigned int
	SOIL_internal_create_OGL_texture
	(
		const unsigned char *const data,
		int *width, int *height, int channels,
		unsigned int reuse_texture_ID,
		unsigned int flags,
		unsigned int opengl_texture_type,
		unsigned int opengl_texture_target,
		unsigned int texture_check_size_enum
	);

/*	and the code magic begins here [8^)	*/
unsigned int
	SOIL_load_OGL_texture
	(
		const char *filename,
		int force_channels,
		unsigned int reuse_texture_ID,
        unsigned int flags,
        int *width,
        int *height
	)
{
	/*	variables	*/
	unsigned char* img;
    int channels;
	unsigned int tex_id;
	/*	does the user want direct uploading of the image as a DDS file?	*/
	if( flags & SOIL_FLAG_DDS_LOAD_DIRECT )
	{
		/*	1st try direct loading of the image as a DDS file
			note: direct uploading will only load what is in the
			DDS file, no MIPmaps will be generated, the image will
			not be flipped, etc.	*/
		tex_id = SOIL_direct_load_DDS( filename, reuse_texture_ID, flags, 0 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if( flags & SOIL_FLAG_PVR_LOAD_DIRECT )
	{
		tex_id = SOIL_direct_load_PVR( filename, reuse_texture_ID, flags, 0 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if( flags & SOIL_FLAG_ETC1_LOAD_DIRECT )
	{
		tex_id = SOIL_direct_load_ETC1( filename, reuse_texture_ID, flags );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	/*	try to load the image	*/
    img = SOIL_load_image( filename, width, height, &channels, force_channels );
	/*	channels holds the original number of channels, which may have been forced	*/
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/*	OK, make it a texture!	*/
	tex_id = SOIL_internal_create_OGL_texture(
            img, width, height, channels,
			reuse_texture_ID, flags,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE );
	/*	and nuke the image data	*/
	SOIL_free_image_data( img );
	/*	and return the handle, such as it is	*/
	return tex_id;
}

unsigned int
	SOIL_load_OGL_HDR_texture
	(
		const char *filename,
		int fake_HDR_format,
		int rescale_to_max,
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	variables	*/
	unsigned char* img = NULL;
	int width, height, channels;
	unsigned int tex_id;
	/*	no direct uploading of the image as a DDS file	*/
	/* error check */
	if( (fake_HDR_format != SOIL_HDR_RGBE) &&
		(fake_HDR_format != SOIL_HDR_RGBdivA) &&
		(fake_HDR_format != SOIL_HDR_RGBdivA2) )
	{
		result_string_pointer = "Invalid fake HDR format specified";
		return 0;
	}

	/* check if the image is HDR */
	if ( stbi_is_hdr( filename ) )
	{
		/*	try to load the image (only the HDR type) */
		img = stbi_load( filename, &width, &height, &channels, 4 );
	}

	/*	channels holds the original number of channels, which may have been forced	*/
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/* the load worked, do I need to convert it? */
	if( fake_HDR_format == SOIL_HDR_RGBdivA )
	{
		RGBE_to_RGBdivA( img, width, height, rescale_to_max );
	} else if( fake_HDR_format == SOIL_HDR_RGBdivA2 )
	{
		RGBE_to_RGBdivA2( img, width, height, rescale_to_max );
	}
	/*	OK, make it a texture!	*/
	tex_id = SOIL_internal_create_OGL_texture(
			img, &width, &height, channels,
			reuse_texture_ID, flags,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE );
	/*	and nuke the image data	*/
	SOIL_free_image_data( img );
	/*	and return the handle, such as it is	*/
	return tex_id;
}

unsigned int
	SOIL_load_OGL_texture_from_memory
	(
		const unsigned char *const buffer,
        unsigned int buffer_length,
		int force_channels,
		unsigned int reuse_texture_ID,
        unsigned int flags,
        int * width,
        int * height
	)
{
	/*	variables	*/
	unsigned char* img;
    int channels;
	unsigned int tex_id;
	/*	does the user want direct uploading of the image as a DDS file?	*/
	if( flags & SOIL_FLAG_DDS_LOAD_DIRECT )
	{
		/*	1st try direct loading of the image as a DDS file
			note: direct uploading will only load what is in the
			DDS file, no MIPmaps will be generated, the image will
			not be flipped, etc.	*/
		tex_id = SOIL_direct_load_DDS_from_memory(
				buffer, buffer_length,
				reuse_texture_ID, flags, 0 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if( flags & SOIL_FLAG_PVR_LOAD_DIRECT )
	{
		tex_id = SOIL_direct_load_PVR_from_memory(
				buffer, buffer_length,
				reuse_texture_ID, flags, 0 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if( flags & SOIL_FLAG_ETC1_LOAD_DIRECT )
	{
		tex_id = SOIL_direct_load_ETC1_from_memory(
				buffer, buffer_length,
				reuse_texture_ID, flags );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	/*	try to load the image	*/
	img = SOIL_load_image_from_memory(
					buffer, buffer_length,
                    width, height, &channels,
					force_channels );
	/*	channels holds the original number of channels, which may have been forced	*/
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/*	OK, make it a texture!	*/
	tex_id = SOIL_internal_create_OGL_texture(
            img, width, height, channels,
			reuse_texture_ID, flags,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE );
	/*	and nuke the image data	*/
	SOIL_free_image_data( img );
	/*	and return the handle, such as it is	*/
	return tex_id;
}

unsigned int
	SOIL_load_OGL_cubemap
	(
		const char *x_pos_file,
		const char *x_neg_file,
		const char *y_pos_file,
		const char *y_neg_file,
		const char *z_pos_file,
		const char *z_neg_file,
		int force_channels,
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	variables	*/
	unsigned char* img;
	int width, height, channels;
	unsigned int tex_id;
	/*	error checking	*/
	if( (x_pos_file == NULL) ||
		(x_neg_file == NULL) ||
		(y_pos_file == NULL) ||
		(y_neg_file == NULL) ||
		(z_pos_file == NULL) ||
		(z_neg_file == NULL) )
	{
		result_string_pointer = "Invalid cube map files list";
		return 0;
	}
	/*	capability checking	*/
	if( query_cubemap_capability() != SOIL_CAPABILITY_PRESENT )
	{
		result_string_pointer = "No cube map capability present";
		return 0;
	}
	/*	1st face: try to load the image	*/
	img = SOIL_load_image( x_pos_file, &width, &height, &channels, force_channels );
	/*	channels holds the original number of channels, which may have been forced	*/
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/*	upload the texture, and create a texture ID if necessary	*/
	tex_id = SOIL_internal_create_OGL_texture(
			img, &width, &height, channels,
			reuse_texture_ID, flags,
			SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_X,
			SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
	/*	and nuke the image data	*/
	SOIL_free_image_data( img );
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image( x_neg_file, &width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image( y_pos_file, &width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image( y_neg_file, &width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image( z_pos_file, &width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image( z_neg_file, &width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	and return the handle, such as it is	*/
	return tex_id;
}

unsigned int
	SOIL_load_OGL_cubemap_from_memory
	(
		const unsigned char *const x_pos_buffer,
		int x_pos_buffer_length,
		const unsigned char *const x_neg_buffer,
		int x_neg_buffer_length,
		const unsigned char *const y_pos_buffer,
		int y_pos_buffer_length,
		const unsigned char *const y_neg_buffer,
		int y_neg_buffer_length,
		const unsigned char *const z_pos_buffer,
		int z_pos_buffer_length,
		const unsigned char *const z_neg_buffer,
		int z_neg_buffer_length,
		int force_channels,
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	variables	*/
	unsigned char* img;
	int width, height, channels;
	unsigned int tex_id;
	/*	error checking	*/
	if( (x_pos_buffer == NULL) ||
		(x_neg_buffer == NULL) ||
		(y_pos_buffer == NULL) ||
		(y_neg_buffer == NULL) ||
		(z_pos_buffer == NULL) ||
		(z_neg_buffer == NULL) )
	{
		result_string_pointer = "Invalid cube map buffers list";
		return 0;
	}
	/*	capability checking	*/
	if( query_cubemap_capability() != SOIL_CAPABILITY_PRESENT )
	{
		result_string_pointer = "No cube map capability present";
		return 0;
	}
	/*	1st face: try to load the image	*/
	img = SOIL_load_image_from_memory(
			x_pos_buffer, x_pos_buffer_length,
			&width, &height, &channels, force_channels );
	/*	channels holds the original number of channels, which may have been forced	*/
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/*	upload the texture, and create a texture ID if necessary	*/
	tex_id = SOIL_internal_create_OGL_texture(
			img, &width, &height, channels,
			reuse_texture_ID, flags,
			SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_X,
			SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
	/*	and nuke the image data	*/
	SOIL_free_image_data( img );
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image_from_memory(
				x_neg_buffer, x_neg_buffer_length,
				&width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image_from_memory(
				y_pos_buffer, y_pos_buffer_length,
				&width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image_from_memory(
				y_neg_buffer, y_neg_buffer_length,
				&width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image_from_memory(
				z_pos_buffer, z_pos_buffer_length,
				&width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	continue?	*/
	if( tex_id != 0 )
	{
		/*	1st face: try to load the image	*/
		img = SOIL_load_image_from_memory(
				z_neg_buffer, z_neg_buffer_length,
				&width, &height, &channels, force_channels );
		/*	channels holds the original number of channels, which may have been forced	*/
		if( (force_channels >= 1) && (force_channels <= 4) )
		{
			channels = force_channels;
		}
		if( NULL == img )
		{
			/*	image loading failed	*/
			result_string_pointer = stbi_failure_reason();
			return 0;
		}
		/*	upload the texture, but reuse the assigned texture ID	*/
		tex_id = SOIL_internal_create_OGL_texture(
				img, &width, &height, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
		/*	and nuke the image data	*/
		SOIL_free_image_data( img );
	}
	/*	and return the handle, such as it is	*/
	return tex_id;
}

unsigned int
	SOIL_load_OGL_single_cubemap
	(
		const char *filename,
		const char face_order[6],
		int force_channels,
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	variables	*/
	unsigned char* img;
	int width, height, channels, i;
	unsigned int tex_id = 0;
	/*	error checking	*/
	if( filename == NULL )
	{
		result_string_pointer = "Invalid single cube map file name";
		return 0;
	}
	/*	does the user want direct uploading of the image as a DDS file?	*/
	if( flags & SOIL_FLAG_DDS_LOAD_DIRECT )
	{
		/*	1st try direct loading of the image as a DDS file
			note: direct uploading will only load what is in the
			DDS file, no MIPmaps will be generated, the image will
			not be flipped, etc.	*/
		tex_id = SOIL_direct_load_DDS( filename, reuse_texture_ID, flags, 1 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if ( flags & SOIL_FLAG_PVR_LOAD_DIRECT )
	{
		tex_id = SOIL_direct_load_PVR( filename, reuse_texture_ID, flags, 1 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if ( flags & SOIL_FLAG_ETC1_LOAD_DIRECT )
	{
		return 0;
	}

	/*	face order checking	*/
	for( i = 0; i < 6; ++i )
	{
		if( (face_order[i] != 'N') &&
			(face_order[i] != 'S') &&
			(face_order[i] != 'W') &&
			(face_order[i] != 'E') &&
			(face_order[i] != 'U') &&
			(face_order[i] != 'D') )
		{
			result_string_pointer = "Invalid single cube map face order";
			return 0;
		};
	}
	/*	capability checking	*/
	if( query_cubemap_capability() != SOIL_CAPABILITY_PRESENT )
	{
		result_string_pointer = "No cube map capability present";
		return 0;
	}
	/*	1st off, try to load the full image	*/
	img = SOIL_load_image( filename, &width, &height, &channels, force_channels );
	/*	channels holds the original number of channels, which may have been forced	*/
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/*	now, does this image have the right dimensions?	*/
	if( (width != 6*height) &&
		(6*width != height) )
	{
		SOIL_free_image_data( img );
		result_string_pointer = "Single cubemap image must have a 6:1 ratio";
		return 0;
	}
	/*	try the image split and create	*/
	tex_id = SOIL_create_OGL_single_cubemap(
			img, width, height, channels,
			face_order, reuse_texture_ID, flags
			);
	/*	nuke the temporary image data and return the texture handle	*/
	SOIL_free_image_data( img );
	return tex_id;
}

unsigned int
	SOIL_load_OGL_single_cubemap_from_memory
	(
		const unsigned char *const buffer,
		int buffer_length,
		const char face_order[6],
		int force_channels,
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	variables	*/
	unsigned char* img;
	int width, height, channels, i;
	unsigned int tex_id = 0;
	/*	error checking	*/
	if( buffer == NULL )
	{
		result_string_pointer = "Invalid single cube map buffer";
		return 0;
	}
	/*	does the user want direct uploading of the image as a DDS file?	*/
	if( flags & SOIL_FLAG_DDS_LOAD_DIRECT )
	{
		/*	1st try direct loading of the image as a DDS file
			note: direct uploading will only load what is in the
			DDS file, no MIPmaps will be generated, the image will
			not be flipped, etc.	*/
		tex_id = SOIL_direct_load_DDS_from_memory(
				buffer, buffer_length,
				reuse_texture_ID, flags, 1 );
		if( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if ( flags & SOIL_FLAG_PVR_LOAD_DIRECT )
	{
		tex_id = SOIL_direct_load_PVR_from_memory(
				buffer, buffer_length,
				reuse_texture_ID, flags, 1 );
		if ( tex_id )
		{
			/*	hey, it worked!!	*/
			return tex_id;
		}
	}

	if ( flags & SOIL_FLAG_ETC1_LOAD_DIRECT )
	{
		return 0;
	}

	/*	face order checking	*/
	for( i = 0; i < 6; ++i )
	{
		if( (face_order[i] != 'N') &&
			(face_order[i] != 'S') &&
			(face_order[i] != 'W') &&
			(face_order[i] != 'E') &&
			(face_order[i] != 'U') &&
			(face_order[i] != 'D') )
		{
			result_string_pointer = "Invalid single cube map face order";
			return 0;
		};
	}
	/*	capability checking	*/
	if( query_cubemap_capability() != SOIL_CAPABILITY_PRESENT )
	{
		result_string_pointer = "No cube map capability present";
		return 0;
	}
	/*	1st off, try to load the full image	*/
	img = SOIL_load_image_from_memory(
			buffer, buffer_length,
			&width, &height, &channels,
			force_channels );
	/*	channels holds the original number of channels, which may have been forced	*/
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}
	if( NULL == img )
	{
		/*	image loading failed	*/
		result_string_pointer = stbi_failure_reason();
		return 0;
	}
	/*	now, does this image have the right dimensions?	*/
	if( (width != 6*height) &&
		(6*width != height) )
	{
		SOIL_free_image_data( img );
		result_string_pointer = "Single cubemap image must have a 6:1 ratio";
		return 0;
	}
	/*	try the image split and create	*/
	tex_id = SOIL_create_OGL_single_cubemap(
			img, width, height, channels,
			face_order, reuse_texture_ID, flags
			);
	/*	nuke the temporary image data and return the texture handle	*/
	SOIL_free_image_data( img );
	return tex_id;
}

unsigned int
	SOIL_create_OGL_single_cubemap
	(
		const unsigned char *const data,
		int width, int height, int channels,
		const char face_order[6],
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	variables	*/
	unsigned char* sub_img;
	int dw, dh, sz, i;
	unsigned int tex_id;
	/*	error checking	*/
	if( data == NULL )
	{
		result_string_pointer = "Invalid single cube map image data";
		return 0;
	}
	/*	face order checking	*/
	for( i = 0; i < 6; ++i )
	{
		if( (face_order[i] != 'N') &&
			(face_order[i] != 'S') &&
			(face_order[i] != 'W') &&
			(face_order[i] != 'E') &&
			(face_order[i] != 'U') &&
			(face_order[i] != 'D') )
		{
			result_string_pointer = "Invalid single cube map face order";
			return 0;
		};
	}
	/*	capability checking	*/
	if( query_cubemap_capability() != SOIL_CAPABILITY_PRESENT )
	{
		result_string_pointer = "No cube map capability present";
		return 0;
	}
	/*	now, does this image have the right dimensions?	*/
	if( (width != 6*height) &&
		(6*width != height) )
	{
		result_string_pointer = "Single cubemap image must have a 6:1 ratio";
		return 0;
	}
	/*	which way am I stepping?	*/
	if( width > height )
	{
		dw = height;
		dh = 0;
	} else
	{
		dw = 0;
		dh = width;
	}
	sz = dw+dh;
	sub_img = (unsigned char *)malloc( sz*sz*channels );
	/*	do the splitting and uploading	*/
	tex_id = reuse_texture_ID;
	for( i = 0; i < 6; ++i )
	{
		int x, y, idx = 0;
		unsigned int cubemap_target = 0;
		/*	copy in the sub-image	*/
		for( y = i*dh; y < i*dh+sz; ++y )
		{
			for( x = i*dw*channels; x < (i*dw+sz)*channels; ++x )
			{
				sub_img[idx++] = data[y*width*channels+x];
			}
		}
		/*	what is my texture target?
			remember, this coordinate system is
			LHS if viewed from inside the cube!	*/
		switch( face_order[i] )
		{
		case 'N':
			cubemap_target = SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			break;
		case 'S':
			cubemap_target = SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			break;
		case 'W':
			cubemap_target = SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			break;
		case 'E':
			cubemap_target = SOIL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;
		case 'U':
			cubemap_target = SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			break;
		case 'D':
			cubemap_target = SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			break;
		}
		/*	upload it as a texture	*/
		tex_id = SOIL_internal_create_OGL_texture(
				sub_img, &sz, &sz, channels,
				tex_id, flags,
				SOIL_TEXTURE_CUBE_MAP,
				cubemap_target,
				SOIL_MAX_CUBE_MAP_TEXTURE_SIZE );
	}
	/*	and nuke the image and sub-image data	*/
	SOIL_free_image_data( sub_img );
	/*	and return the handle, such as it is	*/
	return tex_id;
}

unsigned int
	SOIL_create_OGL_texture
	(
		const unsigned char *const data,
		int *width, int *height, int channels,
		unsigned int reuse_texture_ID,
		unsigned int flags
	)
{
	/*	wrapper function for 2D textures	*/
	return SOIL_internal_create_OGL_texture(
				data, width, height, channels,
				reuse_texture_ID, flags,
				GL_TEXTURE_2D, GL_TEXTURE_2D,
				GL_MAX_TEXTURE_SIZE );
}

#if SOIL_CHECK_FOR_GL_ERRORS
void check_for_GL_errors( const char *calling_location )
{
	/*	check for errors	*/
	GLenum err_code = glGetError();
	while( GL_NO_ERROR != err_code )
	{
		printf( "OpenGL Error @ %s: %i", calling_location, err_code );
		err_code = glGetError();
	}
}
#else
void check_for_GL_errors( const char *calling_location )
{
	/*	no check for errors	*/
}
#endif

static void createMipmaps(const unsigned char *const img,
		int width, int height, int channels,
		unsigned int flags,
		unsigned int opengl_texture_target,
		unsigned int internal_texture_format,
		unsigned int original_texture_format,
		int DXT_mode)
{
	if ( ( flags & SOIL_FLAG_GL_MIPMAPS ) && query_gen_mipmap_capability() == SOIL_CAPABILITY_PRESENT )
	{
		soilGlGenerateMipmap(opengl_texture_target);
	}
	else
	{
		int MIPlevel = 1;
		int MIPwidth = (width+1) / 2;
		int MIPheight = (height+1) / 2;
		unsigned char *resampled = (unsigned char*)malloc( channels*MIPwidth*MIPheight );

		while( ((1<<MIPlevel) <= width) || ((1<<MIPlevel) <= height) )
		{
			/*	do this MIPmap level	*/
			mipmap_image(
					img, width, height, channels,
					resampled,
					(1 << MIPlevel), (1 << MIPlevel) );

			/*  upload the MIPmaps	*/
			if( DXT_mode == SOIL_CAPABILITY_PRESENT )
			{
				/*	user wants me to do the DXT conversion!	*/
				int DDS_size;
				unsigned char *DDS_data = NULL;
				if( (channels & 1) == 1 )
				{
					/*	RGB, use DXT1	*/
					DDS_data = convert_image_to_DXT1(
							resampled, MIPwidth, MIPheight, channels, &DDS_size );
				} else
				{
					/*	RGBA, use DXT5	*/
					DDS_data = convert_image_to_DXT5(
							resampled, MIPwidth, MIPheight, channels, &DDS_size );
				}
				if( DDS_data )
				{
					soilGlCompressedTexImage2D(
						opengl_texture_target, MIPlevel,
						internal_texture_format, MIPwidth, MIPheight, 0,
						DDS_size, DDS_data );
					check_for_GL_errors( "glCompressedTexImage2D" );
					SOIL_free_image_data( DDS_data );
				} else
				{
					/*	my compression failed, try the OpenGL driver's version	*/
					glTexImage2D(
						opengl_texture_target, MIPlevel,
						internal_texture_format, MIPwidth, MIPheight, 0,
						original_texture_format, GL_UNSIGNED_BYTE, resampled );
					check_for_GL_errors( "glTexImage2D" );
				}
			} else
			{
				/*	user want OpenGL to do all the work!	*/
				glTexImage2D(
					opengl_texture_target, MIPlevel,
					internal_texture_format, MIPwidth, MIPheight, 0,
					original_texture_format, GL_UNSIGNED_BYTE, resampled );
				check_for_GL_errors( "glTexImage2D" );
			}
			/*	prep for the next level	*/
			++MIPlevel;
			MIPwidth = (MIPwidth + 1) / 2;
			MIPheight = (MIPheight + 1) / 2;
		}

		SOIL_free_image_data( resampled );
	}
}

unsigned int
	SOIL_internal_create_OGL_texture
	(
		const unsigned char *const data,
		int *width, int *height, int channels,
		unsigned int reuse_texture_ID,
		unsigned int flags,
		unsigned int opengl_texture_type,
		unsigned int opengl_texture_target,
		unsigned int texture_check_size_enum
	)
{
	/*	variables	*/
	unsigned char* img = NULL;
	unsigned int tex_id;
	unsigned int internal_texture_format = 0, original_texture_format = 0;
	int DXT_mode = SOIL_CAPABILITY_UNKNOWN;
	int max_supported_size;
	int iwidth = *width;
	int iheight = *height;
	int needCopy;
	GLint unpack_aligment;

	/*	how large of a texture can this OpenGL implementation handle?	*/
	/*	texture_check_size_enum will be GL_MAX_TEXTURE_SIZE or SOIL_MAX_CUBE_MAP_TEXTURE_SIZE	*/
	glGetIntegerv( texture_check_size_enum, &max_supported_size );

	/*	If the user wants to use the texture rectangle I kill a few flags	*/
	if( flags & SOIL_FLAG_TEXTURE_RECTANGLE )
	{
		/*	well, the user asked for it, can we do that?	*/
		if( query_tex_rectangle_capability() == SOIL_CAPABILITY_PRESENT )
		{
			/*	only allow this if the user in _NOT_ trying to do a cubemap!	*/
			if( opengl_texture_type == GL_TEXTURE_2D )
			{
				/*	clean out the flags that cannot be used with texture rectangles	*/
				flags &= ~(
						SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS |
						SOIL_FLAG_TEXTURE_REPEATS
					);
				/*	and change my target	*/
				opengl_texture_target = SOIL_TEXTURE_RECTANGLE_ARB;
				opengl_texture_type = SOIL_TEXTURE_RECTANGLE_ARB;
			} else
			{
				/*	not allowed for any other uses (yes, I'm looking at you, cubemaps!)	*/
				flags &= ~SOIL_FLAG_TEXTURE_RECTANGLE;
			}

		} else
		{
			/*	can't do it, and that is a breakable offense (uv coords use pixels instead of [0,1]!)	*/
			result_string_pointer = "Texture Rectangle extension unsupported";
			return 0;
		}
	}

	/*	if the user can't support NPOT textures, make sure we force the POT option	*/
	if( (query_NPOT_capability() == SOIL_CAPABILITY_NONE) &&
		!(flags & SOIL_FLAG_TEXTURE_RECTANGLE) )
	{
		/*	add in the POT flag */
		flags |= SOIL_FLAG_POWER_OF_TWO;
	}

	needCopy = ( ( flags & SOIL_FLAG_INVERT_Y ) ||
				 ( flags & SOIL_FLAG_NTSC_SAFE_RGB ) ||
				 ( flags & SOIL_FLAG_MULTIPLY_ALPHA ) ||
				 ( flags & SOIL_FLAG_CoCg_Y )
				);

	/*	create a copy the image data only if needed */
	if ( needCopy ) {
		img = (unsigned char*)malloc( iwidth*iheight*channels );
		memcpy( img, data, iwidth*iheight*channels );
	}

	/*	does the user want me to invert the image?	*/
	if( flags & SOIL_FLAG_INVERT_Y )
	{
		int i, j;
		for( j = 0; j*2 < iheight; ++j )
		{
			int index1 = j * iwidth * channels;
			int index2 = (iheight - 1 - j) * iwidth * channels;
			for( i = iwidth * channels; i > 0; --i )
			{
				unsigned char temp = img[index1];
				img[index1] = img[index2];
				img[index2] = temp;
				++index1;
				++index2;
			}
		}
	}
	/*	does the user want me to scale the colors into the NTSC safe RGB range?	*/
	if( flags & SOIL_FLAG_NTSC_SAFE_RGB )
	{
		scale_image_RGB_to_NTSC_safe( img, iwidth, iheight, channels );
	}
	/*	does the user want me to convert from straight to pre-multiplied alpha?
		(and do we even _have_ alpha?)	*/
	if( flags & SOIL_FLAG_MULTIPLY_ALPHA )
	{
		int i;
		switch( channels )
		{
		case 2:
			for( i = 0; i < 2*iwidth*iheight; i += 2 )
			{
				img[i] = (img[i] * img[i+1] + 128) >> 8;
			}
			break;
		case 4:
			for( i = 0; i < 4*iwidth*iheight; i += 4 )
			{
				img[i+0] = (img[i+0] * img[i+3] + 128) >> 8;
				img[i+1] = (img[i+1] * img[i+3] + 128) >> 8;
				img[i+2] = (img[i+2] * img[i+3] + 128) >> 8;
			}
			break;
		default:
			/*	no other number of channels contains alpha data	*/
			break;
		}
	}

	/*	do I need to make it a power of 2?	*/
	if(
		( ( flags & SOIL_FLAG_POWER_OF_TWO) && ( !SOIL_IS_POW2(iwidth) || !SOIL_IS_POW2(iheight) ) ) ||	/*	user asked for it and the texture is not power of 2	*/
		( (flags & SOIL_FLAG_MIPMAPS)&& !( ( flags & SOIL_FLAG_GL_MIPMAPS ) &&
										   query_gen_mipmap_capability() == SOIL_CAPABILITY_PRESENT &&
										   query_NPOT_capability() == SOIL_CAPABILITY_PRESENT ) ) ||	/*	need it for the MIP-maps when mipmaps required
																											and not GL mipmaps required and supported	*/
		(iwidth > max_supported_size) ||		/*	it's too big, (make sure it's	*/
		(iheight > max_supported_size) )		/*	2^n for later down-sampling)	*/
	{
		int new_width = 1;
		int new_height = 1;
		while( new_width < iwidth )
		{
			new_width *= 2;
		}
		while( new_height < iheight )
		{
			new_height *= 2;
		}
		/*	still?	*/
		if( (new_width != iwidth) || (new_height != iheight) )
		{
			/*	yep, resize	*/
			unsigned char *resampled = (unsigned char*)malloc( channels*new_width*new_height );
			up_scale_image(
					NULL != img ? img : data, iwidth, iheight, channels,
					resampled, new_width, new_height );

			/*	nuke the old guy ( if a copy exists ), then point it at the new guy	*/
			SOIL_free_image_data( img );
			img = resampled;
			*width = new_width;
			*height = new_height;
			iwidth = new_width;
			iheight = new_height;
		}
	}
	/*	now, if it is too large...	*/
	if( (iwidth > max_supported_size) || (iheight > max_supported_size) )
	{
		/*	I've already made it a power of two, so simply use the MIPmapping
			code to reduce its size to the allowable maximum.	*/
		unsigned char *resampled;
		int reduce_block_x = 1, reduce_block_y = 1;
		int new_width, new_height;
		if( iwidth > max_supported_size )
		{
			reduce_block_x = iwidth / max_supported_size;
		}
		if( iheight > max_supported_size )
		{
			reduce_block_y = iheight / max_supported_size;
		}
		new_width = iwidth / reduce_block_x;
		new_height = iheight / reduce_block_y;
		resampled = (unsigned char*)malloc( channels*new_width*new_height );
		/*	perform the actual reduction	*/
		mipmap_image( NULL != img ? img : data, iwidth, iheight, channels,
						resampled, reduce_block_x, reduce_block_y );
		/*	nuke the old guy, then point it at the new guy	*/
		SOIL_free_image_data( img );
		img = resampled;
		*width = new_width;
		*height = new_height;
		iwidth = new_width;
		iheight = new_height;
	}
	/*	does the user want us to use YCoCg color space?	*/
	if( flags & SOIL_FLAG_CoCg_Y )
	{
		/*	this will only work with RGB and RGBA images */
		convert_RGB_to_YCoCg( img, iwidth, iheight, channels );
	}
	/*	create the OpenGL texture ID handle
		(note: allowing a forced texture ID lets me reload a texture)	*/
	tex_id = reuse_texture_ID;
	if( tex_id == 0 )
	{
		glGenTextures( 1, &tex_id );
	}
	check_for_GL_errors( "glGenTextures" );
	/* Note: sometimes glGenTextures fails (usually no OpenGL context)	*/
	if( tex_id )
	{
		/*	and what type am I using as the internal texture format?	*/
		switch( channels )
		{
		case 1:
			original_texture_format = GL_LUMINANCE;
			break;
		case 2:
			original_texture_format = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			original_texture_format = GL_RGB;
			break;
		case 4:
			original_texture_format = GL_RGBA;
			break;
		}
		internal_texture_format = original_texture_format;
		/*	does the user want me to, and can I, save as DXT?	*/
		if( flags & SOIL_FLAG_COMPRESS_TO_DXT )
		{
			DXT_mode = query_DXT_capability();
			if( DXT_mode == SOIL_CAPABILITY_PRESENT )
			{
				/*	I can use DXT, whether I compress it or OpenGL does	*/
				if( (channels & 1) == 1 )
				{
					/*	1 or 3 channels = DXT1	*/
					internal_texture_format = SOIL_RGB_S3TC_DXT1;
				} else
				{
					/*	2 or 4 channels = DXT5	*/
					internal_texture_format = SOIL_RGBA_S3TC_DXT5;
				}
			}
		}
		/*  bind an OpenGL texture ID	*/
		glBindTexture( opengl_texture_type, tex_id );
		check_for_GL_errors( "glBindTexture" );

		/* set the unpack aligment */
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
		if ( 1 != unpack_aligment )
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		}

		/*  upload the main image	*/
		if( DXT_mode == SOIL_CAPABILITY_PRESENT )
		{
			/*	user wants me to do the DXT conversion!	*/
			int DDS_size;
			unsigned char *DDS_data = NULL;
			if( (channels & 1) == 1 )
			{
				/*	RGB, use DXT1	*/
				DDS_data = convert_image_to_DXT1( NULL != img ? img : data, iwidth, iheight, channels, &DDS_size );
			} else
			{
				/*	RGBA, use DXT5	*/
				DDS_data = convert_image_to_DXT5( NULL != img ? img : data, iwidth, iheight, channels, &DDS_size );
			}
			if( DDS_data )
			{
				soilGlCompressedTexImage2D(
					opengl_texture_target, 0,
					internal_texture_format, iwidth, iheight, 0,
					DDS_size, DDS_data );
				check_for_GL_errors( "glCompressedTexImage2D" );
				SOIL_free_image_data( DDS_data );
				/*	printf( "Internal DXT compressor\n" );	*/
			} else
			{
				/*	my compression failed, try the OpenGL driver's version	*/
				glTexImage2D(
					opengl_texture_target, 0,
					internal_texture_format, iwidth, iheight, 0,
					original_texture_format, GL_UNSIGNED_BYTE, NULL != img ? img : data );
				check_for_GL_errors( "glTexImage2D" );
				/*	printf( "OpenGL DXT compressor\n" );	*/
			}
		} else
		{
			/*	user want OpenGL to do all the work!	*/
			glTexImage2D(
				opengl_texture_target, 0,
				internal_texture_format, iwidth, iheight, 0,
				original_texture_format, GL_UNSIGNED_BYTE, NULL != img ? img : data );

			check_for_GL_errors( "glTexImage2D" );
			/*printf( "OpenGL DXT compressor\n" );	*/
		}

		/*	are any MIPmaps desired?	*/
		if( flags & SOIL_FLAG_MIPMAPS || flags & SOIL_FLAG_GL_MIPMAPS )
		{
			createMipmaps( NULL != img ? img : data, iwidth, iheight, channels, flags, opengl_texture_target, internal_texture_format, original_texture_format, DXT_mode );

			/*	instruct OpenGL to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			check_for_GL_errors( "GL_TEXTURE_MIN/MAG_FILTER" );
		} else
		{
			/*	instruct OpenGL _NOT_ to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			check_for_GL_errors( "GL_TEXTURE_MIN/MAG_FILTER" );
		}

		/* recover the unpack aligment */
		if ( 1 != unpack_aligment )
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_aligment);
		}

		/*	does the user want clamping, or wrapping?	*/
		if( flags & SOIL_FLAG_TEXTURE_REPEATS )
		{
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT );
			if( opengl_texture_type == SOIL_TEXTURE_CUBE_MAP )
			{
				/*	SOIL_TEXTURE_WRAP_R is invalid if cubemaps aren't supported	*/
				glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT );
			}
			check_for_GL_errors( "GL_TEXTURE_WRAP_*" );
		} else
		{
			unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;
			/* unsigned int clamp_mode = GL_CLAMP; */
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode );
			if( opengl_texture_type == SOIL_TEXTURE_CUBE_MAP )
			{
				/*	SOIL_TEXTURE_WRAP_R is invalid if cubemaps aren't supported	*/
				glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode );
			}
			check_for_GL_errors( "GL_TEXTURE_WRAP_*" );
		}
		/*	done	*/
		result_string_pointer = "Image loaded as an OpenGL texture";
	} else
	{
		/*	failed	*/
		result_string_pointer = "Failed to generate an OpenGL texture name; missing OpenGL context?";
	}

	SOIL_free_image_data( img );

	return tex_id;
}

int
	SOIL_save_screenshot
	(
		const char *filename,
		int image_type,
		int x, int y,
		int width, int height
	)
{
	unsigned char *pixel_data;
	int i, j;
	int save_result;

	/*	error checks	*/
	if( (width < 1) || (height < 1) )
	{
		result_string_pointer = "Invalid screenshot dimensions";
		return 0;
	}
	if( (x < 0) || (y < 0) )
	{
		result_string_pointer = "Invalid screenshot location";
		return 0;
	}
	if( filename == NULL )
	{
		result_string_pointer = "Invalid screenshot filename";
		return 0;
	}

	/*  Get the data from OpenGL	*/
	pixel_data = (unsigned char*)malloc( 3*width*height );
	glReadPixels (x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixel_data);

	/*	invert the image	*/
	for( j = 0; j*2 < height; ++j )
	{
		int index1 = j * width * 3;
		int index2 = (height - 1 - j) * width * 3;
		for( i = width * 3; i > 0; --i )
		{
			unsigned char temp = pixel_data[index1];
			pixel_data[index1] = pixel_data[index2];
			pixel_data[index2] = temp;
			++index1;
			++index2;
		}
	}

	/*	save the image	*/
	save_result = SOIL_save_image( filename, image_type, width, height, 3, pixel_data);

	/*	And free the memory	*/
	SOIL_free_image_data( pixel_data );
	return save_result;
}

unsigned char*
	SOIL_load_image
	(
		const char *filename,
		int *width, int *height, int *channels,
		int force_channels
	)
{
	unsigned char *result = stbi_load( filename,
			width, height, channels, force_channels );
	if( result == NULL )
	{
		result_string_pointer = stbi_failure_reason();
	} else
	{
		result_string_pointer = "Image loaded";
	}
	return result;
}

unsigned char*
	SOIL_load_image_from_memory
	(
		const unsigned char *const buffer,
		int buffer_length,
		int *width, int *height, int *channels,
		int force_channels
	)
{
	unsigned char *result = stbi_load_from_memory(
				buffer, buffer_length,
				width, height, channels,
				force_channels );
	if( result == NULL )
	{
		result_string_pointer = stbi_failure_reason();
	} else
	{
		result_string_pointer = "Image loaded from memory";
	}
	return result;
}

int
	SOIL_save_image
	(
		const char *filename,
		int image_type,
		int width, int height, int channels,
		const unsigned char *const data
	)
{
	int save_result;

	/*	error check	*/
	if( (width < 1) || (height < 1) ||
		(channels < 1) || (channels > 4) ||
		(data == NULL) ||
		(filename == NULL) )
	{
		return 0;
	}
	if( image_type == SOIL_SAVE_TYPE_BMP )
	{
		save_result = stbi_write_bmp( filename,
				width, height, channels, (void*)data );
	} else
	if( image_type == SOIL_SAVE_TYPE_TGA )
	{
		save_result = stbi_write_tga( filename,
				width, height, channels, (void*)data );
	} else
	if( image_type == SOIL_SAVE_TYPE_DDS )
	{
		save_result = save_image_as_DDS( filename,
				width, height, channels, (const unsigned char *const)data );
	} else
	if( image_type == SOIL_SAVE_TYPE_PNG )
	{
		save_result = stbi_write_png( filename,
				width, height, channels, (const unsigned char *const)data, 0 );
	} else
	{
		save_result = 0;
	}

	if( save_result == 0 )
	{
		result_string_pointer = "Saving the image failed";
	} else
	{
		result_string_pointer = "Image saved";
	}
	return save_result;
}

void
	SOIL_free_image_data
	(
		unsigned char *img_data
	)
{
	if ( img_data )
		free( (void*)img_data );
}

const char*
	SOIL_last_result
	(
		void
	)
{
	return result_string_pointer;
}

unsigned int SOIL_direct_load_DDS_from_memory(
		const unsigned char *const buffer,
		unsigned int buffer_length,
		unsigned int reuse_texture_ID,
		int flags,
		int loading_as_cubemap )
{
	/*	variables	*/
	DDS_header header;
	unsigned int buffer_index = 0;
	unsigned int tex_ID = 0;
	/*	file reading variables	*/
	unsigned int S3TC_type = 0;
	unsigned char *DDS_data;
	unsigned int DDS_main_size;
	unsigned int DDS_full_size;
	unsigned int width, height;
	int mipmaps, cubemap, uncompressed, block_size = 16;
	unsigned int flag;
	unsigned int cf_target, ogl_target_start, ogl_target_end;
	unsigned int opengl_texture_type;
	int i;
	/*	1st off, does the filename even exist?	*/
	if( NULL == buffer )
	{
		/*	we can't do it!	*/
		result_string_pointer = "NULL buffer";
		return 0;
	}
	if( buffer_length < sizeof( DDS_header ) )
	{
		/*	we can't do it!	*/
		result_string_pointer = "DDS file was too small to contain the DDS header";
		return 0;
	}
	/*	try reading in the header	*/
	memcpy ( (void*)(&header), (const void *)buffer, sizeof( DDS_header ) );
	buffer_index = sizeof( DDS_header );
	/*	guilty until proven innocent	*/
	result_string_pointer = "Failed to read a known DDS header";
	/*	validate the header (warning, "goto"'s ahead, shield your eyes!!)	*/
	flag = ('D'<<0)|('D'<<8)|('S'<<16)|(' '<<24);
	if( header.dwMagic != flag ) {goto quick_exit;}
	if( header.dwSize != 124 ) {goto quick_exit;}
	/*	I need all of these	*/
	flag = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	if( (header.dwFlags & flag) != flag ) {goto quick_exit;}
	/*	According to the MSDN spec, the dwFlags should contain
		DDSD_LINEARSIZE if it's compressed, or DDSD_PITCH if
		uncompressed.  Some DDS writers do not conform to the
		spec, so I need to make my reader more tolerant	*/
	/*	I need one of these	*/
	flag = DDPF_FOURCC | DDPF_RGB;
	if( (header.sPixelFormat.dwFlags & flag) == 0 ) {goto quick_exit;}
	if( header.sPixelFormat.dwSize != 32 ) {goto quick_exit;}
	if( (header.sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0 ) {goto quick_exit;}
	/*	make sure it is a type we can upload	*/
	if( (header.sPixelFormat.dwFlags & DDPF_FOURCC) &&
		!(
		(header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('1'<<24))) ||
		(header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('3'<<24))) ||
		(header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('5'<<24)))
		) )
	{
		goto quick_exit;
	}
	/*	OK, validated the header, let's load the image data	*/
	result_string_pointer = "DDS header loaded and validated";
	width = header.dwWidth;
	height = header.dwHeight;
	uncompressed = 1 - (header.sPixelFormat.dwFlags & DDPF_FOURCC) / DDPF_FOURCC;
	cubemap = (header.sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) / DDSCAPS2_CUBEMAP;
	if( uncompressed )
	{
		S3TC_type = GL_RGB;
		block_size = 3;
		if( header.sPixelFormat.dwFlags & DDPF_ALPHAPIXELS )
		{
			S3TC_type = GL_RGBA;
			block_size = 4;
		}
		DDS_main_size = width * height * block_size;
	} else
	{
		/*	can we even handle direct uploading to OpenGL DXT compressed images?	*/
		if( query_DXT_capability() != SOIL_CAPABILITY_PRESENT )
		{
			/*	we can't do it!	*/
			result_string_pointer = "Direct upload of S3TC images not supported by the OpenGL driver";
			return 0;
		}
		/*	well, we know it is DXT1/3/5, because we checked above	*/
		switch( (header.sPixelFormat.dwFourCC >> 24) - '0' )
		{
		case 1:
			S3TC_type = SOIL_RGBA_S3TC_DXT1;
			block_size = 8;
			break;
		case 3:
			S3TC_type = SOIL_RGBA_S3TC_DXT3;
			block_size = 16;
			break;
		case 5:
			S3TC_type = SOIL_RGBA_S3TC_DXT5;
			block_size = 16;
			break;
		}
		DDS_main_size = ((width+3)>>2)*((height+3)>>2)*block_size;
	}
	if( cubemap )
	{
		/* does the user want a cubemap?	*/
		if( !loading_as_cubemap )
		{
			/*	we can't do it!	*/
			result_string_pointer = "DDS image was a cubemap";
			return 0;
		}
		/*	can we even handle cubemaps with the OpenGL driver?	*/
		if( query_cubemap_capability() != SOIL_CAPABILITY_PRESENT )
		{
			/*	we can't do it!	*/
			result_string_pointer = "Direct upload of cubemap images not supported by the OpenGL driver";
			return 0;
		}
		ogl_target_start = SOIL_TEXTURE_CUBE_MAP_POSITIVE_X;
		ogl_target_end =   SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		opengl_texture_type = SOIL_TEXTURE_CUBE_MAP;
	} else
	{
		/* does the user want a non-cubemap?	*/
		if( loading_as_cubemap )
		{
			/*	we can't do it!	*/
			result_string_pointer = "DDS image was not a cubemap";
			return 0;
		}
		ogl_target_start = GL_TEXTURE_2D;
		ogl_target_end =   GL_TEXTURE_2D;
		opengl_texture_type = GL_TEXTURE_2D;
	}
	if( (header.sCaps.dwCaps1 & DDSCAPS_MIPMAP) && (header.dwMipMapCount > 1) )
	{
		int shift_offset;
		mipmaps = header.dwMipMapCount - 1;
		DDS_full_size = DDS_main_size;
		if( uncompressed )
		{
			/*	uncompressed DDS, simple MIPmap size calculation	*/
			shift_offset = 0;
		} else
		{
			/*	compressed DDS, MIPmap size calculation is block based	*/
			shift_offset = 2;
		}
		for( i = 1; i <= mipmaps; ++ i )
		{
			int w, h;
			w = width >> (shift_offset + i);
			h = height >> (shift_offset + i);
			if( w < 1 )
			{
				w = 1;
			}
			if( h < 1 )
			{
				h = 1;
			}
			DDS_full_size += w*h*block_size;
		}
	} else
	{
		mipmaps = 0;
		DDS_full_size = DDS_main_size;
	}
	DDS_data = (unsigned char*)malloc( DDS_full_size );
	/*	got the image data RAM, create or use an existing OpenGL texture handle	*/
	tex_ID = reuse_texture_ID;
	if( tex_ID == 0 )
	{
		glGenTextures( 1, &tex_ID );
	}
	/*  bind an OpenGL texture ID	*/
	glBindTexture( opengl_texture_type, tex_ID );
	/*	do this for each face of the cubemap!	*/
	for( cf_target = ogl_target_start; cf_target <= ogl_target_end; ++cf_target )
	{
		if( buffer_index + DDS_full_size <= buffer_length )
		{
			unsigned int byte_offset = DDS_main_size;
			memcpy( (void*)DDS_data, (const void*)(&buffer[buffer_index]), DDS_full_size );
			buffer_index += DDS_full_size;
			/*	upload the main chunk	*/
			if( uncompressed )
			{
				/*	and remember, DXT uncompressed uses BGR(A),
					so swap to RGB(A) for ALL MIPmap levels	*/
				for( i = 0; i < (int)DDS_full_size; i += block_size )
				{
					unsigned char temp = DDS_data[i];
					DDS_data[i] = DDS_data[i+2];
					DDS_data[i+2] = temp;
				}
				glTexImage2D(
					cf_target, 0,
					S3TC_type, width, height, 0,
					S3TC_type, GL_UNSIGNED_BYTE, DDS_data );
			} else
			{
				soilGlCompressedTexImage2D(
					cf_target, 0,
					S3TC_type, width, height, 0,
					DDS_main_size, DDS_data );
			}
			/*	upload the mipmaps, if we have them	*/
			for( i = 1; i <= mipmaps; ++i )
			{
				int w, h, mip_size;
				w = width >> i;
				h = height >> i;
				if( w < 1 )
				{
					w = 1;
				}
				if( h < 1 )
				{
					h = 1;
				}
				/*	upload this mipmap	*/
				if( uncompressed )
				{
					mip_size = w*h*block_size;
					glTexImage2D(
						cf_target, i,
						S3TC_type, w, h, 0,
						S3TC_type, GL_UNSIGNED_BYTE, &DDS_data[byte_offset] );
				} else
				{
					mip_size = ((w+3)/4)*((h+3)/4)*block_size;
					soilGlCompressedTexImage2D(
						cf_target, i,
						S3TC_type, w, h, 0,
						mip_size, &DDS_data[byte_offset] );
				}
				/*	and move to the next mipmap	*/
				byte_offset += mip_size;
			}
			/*	it worked!	*/
			result_string_pointer = "DDS file loaded";
		} else
		{
			glDeleteTextures( 1, & tex_ID );
			tex_ID = 0;
			cf_target = ogl_target_end + 1;
			result_string_pointer = "DDS file was too small for expected image data";
		}
	}/* end reading each face */
	SOIL_free_image_data( DDS_data );
	if( tex_ID )
	{
		/*	did I have MIPmaps?	*/
		if( mipmaps > 0 )
		{
			/*	instruct OpenGL to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		} else
		{
			/*	instruct OpenGL _NOT_ to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		}
		/*	does the user want clamping, or wrapping?	*/
		if( flags & SOIL_FLAG_TEXTURE_REPEATS )
		{
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT );
		} else
		{
			unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;
			/* unsigned int clamp_mode = GL_CLAMP; */
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode );
			glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode );
		}
	}

quick_exit:
	/*	report success or failure	*/
	return tex_ID;
}

unsigned int SOIL_direct_load_DDS(
		const char *filename,
		unsigned int reuse_texture_ID,
		int flags,
		int loading_as_cubemap )
{
	FILE *f;
	unsigned char *buffer;
	size_t buffer_length, bytes_read;
	unsigned int tex_ID = 0;
	/*	error checks	*/
	if( NULL == filename )
	{
		result_string_pointer = "NULL filename";
		return 0;
	}
	f = fopen( filename, "rb" );
	if( NULL == f )
	{
		/*	the file doesn't seem to exist (or be open-able)	*/
		result_string_pointer = "Can not find DDS file";
		return 0;
	}
	fseek( f, 0, SEEK_END );
	buffer_length = ftell( f );
	fseek( f, 0, SEEK_SET );
	buffer = (unsigned char *) malloc( buffer_length );
	if( NULL == buffer )
	{
		result_string_pointer = "malloc failed";
		fclose( f );
		return 0;
	}
	bytes_read = fread( (void*)buffer, 1, buffer_length, f );
	fclose( f );
	if( bytes_read < buffer_length )
	{
		/*	huh?	*/
		buffer_length = bytes_read;
	}
	/*	now try to do the loading	*/
	tex_ID = SOIL_direct_load_DDS_from_memory(
		(const unsigned char *const)buffer, (int)buffer_length,
		reuse_texture_ID, flags, loading_as_cubemap );
	SOIL_free_image_data( buffer );
	return tex_ID;
}

unsigned int SOIL_direct_load_PVR_from_memory(
		const unsigned char *const buffer,
		int buffer_length,
		unsigned int reuse_texture_ID,
		int flags,
		int loading_as_cubemap )
{
	PVR_Texture_Header* header = (PVR_Texture_Header*)buffer;
	int num_surfs = 1;
	GLuint tex_ID = 0;
	GLenum PVR_format = 0;
	GLenum PVR_type = GL_RGB;
	unsigned int opengl_texture_type = loading_as_cubemap ? SOIL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
	int is_PVRTC_supported = query_PVR_capability() == SOIL_CAPABILITY_PRESENT;
	int is_BGRA8888_supported  = query_BGRA8888_capability() == SOIL_CAPABILITY_PRESENT;
	int is_compressed_format_supported = 0;
	int is_compressed_format = 0;
	int mipmaps = 0;
	int i;
	GLint unpack_aligment;

	// Check the header size
	if ( header->dwHeaderSize != sizeof(PVR_Texture_Header) ) {
		if ( header->dwHeaderSize == PVRTEX_V1_HEADER_SIZE ) {
			result_string_pointer = "this is an old pvr ( update the PVR file )";

			if ( loading_as_cubemap ) {
				if( header->dwpfFlags & PVRTEX_CUBEMAP ) {
					num_surfs = 6;
				} else {
					result_string_pointer = "tried to load a non-cubemap PVR as cubemap";
					return 0;
				}
			}
		} else {
			result_string_pointer = "invalid PVR header";

			return 0;
		}
	} else {
		if ( loading_as_cubemap ) {
			// Header V2
			if( header->dwNumSurfs < 1 ) {
				if( header->dwpfFlags & PVRTEX_CUBEMAP ) {
					num_surfs = 6;
				} else {
					result_string_pointer = "tried to load a non-cubemap PVR as cubemap";
					return 0;
				}
			} else {
				num_surfs = header->dwNumSurfs;
			}
		}
	}

	// Check the magic identifier
	if ( header->dwPVR != PVRTEX_IDENTIFIER ) {
		result_string_pointer = "invalid PVR header";
		return 0;
	}

	/* Only accept untwiddled data UNLESS texture format is PVRTC */
	if ( ((header->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
		&& ((header->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC2)
		&& ((header->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC4) )
	{
		// We need to load untwiddled textures -- hw will twiddle for us.
		result_string_pointer = "pvr is not compressed ( untwiddled texture )";
		return 0;
	}

	switch( header->dwpfFlags & PVRTEX_PIXELTYPE )
	{
		case OGL_RGBA_4444:
			PVR_format = GL_UNSIGNED_SHORT_4_4_4_4;
			PVR_type = GL_RGBA;
			break;
		case OGL_RGBA_5551:
			PVR_format = GL_UNSIGNED_SHORT_5_5_5_1;
			PVR_type = GL_RGBA;
			break;
		case OGL_RGBA_8888:
			PVR_format = GL_UNSIGNED_BYTE;
			PVR_type = GL_RGBA;
			break;
		case OGL_RGB_565:
			PVR_format = GL_UNSIGNED_SHORT_5_6_5;
			PVR_type = GL_RGB;
			break;
		case OGL_RGB_555:
			result_string_pointer = "failed: pixel type OGL_RGB_555 not supported.";
			return 0;
		case OGL_RGB_888:
			PVR_format = GL_UNSIGNED_BYTE;
			PVR_type = GL_RGB;
			break;
		case OGL_I_8:
			PVR_format = GL_UNSIGNED_BYTE;
			PVR_type = GL_LUMINANCE;
			break;
		case OGL_AI_88:
			PVR_format = GL_UNSIGNED_BYTE;
			PVR_type = GL_LUMINANCE_ALPHA;
			break;
		case MGLPT_PVRTC2:
		case OGL_PVRTC2:
			if(is_PVRTC_supported) {
				is_compressed_format_supported = is_compressed_format = 1;
				PVR_format = header->dwAlphaBitMask==0 ? SOIL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : SOIL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
			} else {
				result_string_pointer = "error: PVRTC2 not supported.Decompress the texture first.";
				return 0;
			}
			break;
		case MGLPT_PVRTC4:
		case OGL_PVRTC4:
			if(is_PVRTC_supported) {
				is_compressed_format_supported = is_compressed_format = 1;
				PVR_format = header->dwAlphaBitMask==0 ? SOIL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : SOIL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
			} else {
				result_string_pointer = "error: PVRTC4 not supported. Decompress the texture first.";
				return 0;
			}
			break;
		case OGL_BGRA_8888:
			if(is_BGRA8888_supported) {
				PVR_format = GL_UNSIGNED_BYTE;
				PVR_type   = GL_BGRA;
				break;
			} else {
				result_string_pointer = "Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported.";
				return 0;
			}
		default:											// NOT SUPPORTED
			result_string_pointer = "failed: pixel type not supported.";
			return 0;
	}

	#ifdef SOIL_GLES1
	//  check that this data is cube map data or not.
	if( loading_as_cubemap ) {
		result_string_pointer = "cube map textures are not available in GLES1.x.";
		return 0;
	}
	#endif

	// load the texture up
	tex_ID = reuse_texture_ID;
	if( tex_ID == 0 )
	{
		glGenTextures( 1, &tex_ID );
	}

	glBindTexture( opengl_texture_type, tex_ID );

	if( glGetError() ) {
		result_string_pointer = "failed: glBindTexture() failed.";
		return 0;
	}

	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
	if ( 1 != unpack_aligment )
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);				// Never have row-aligned in headers
	}

	#define _MAX( a, b ) (( a <= b )? b : a)
	for(i=0; i<num_surfs; i++) {
		char *texture_ptr = (char*)buffer + header->dwHeaderSize + header->dwTextureDataSize * i;
		char *cur_texture_ptr = 0;
		int	mipmap_level;
		unsigned int width= header->dwWidth, height = header->dwHeight;
		unsigned int compressed_image_size = 0;

		mipmaps = ( ( flags & SOIL_FLAG_MIPMAPS ) && (header->dwpfFlags & PVRTEX_MIPMAP) ) ? header->dwMipMapCount : 0;

		for(mipmap_level = 0; mipmap_level <= mipmaps; width = _MAX(width/2, (unsigned int)1), height = _MAX(height/2, (unsigned int)1), mipmap_level++ ) {
			// Do Alpha-swap if needed
			cur_texture_ptr = texture_ptr;

			// Load the Texture
			/* If the texture is PVRTC then use GLCompressedTexImage2D */
			if( is_compressed_format ) {
				/* Calculate how many bytes this MIP level occupies */
				if ((header->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2) {
					compressed_image_size = ( _MAX(width, PVRTC2_MIN_TEXWIDTH) * _MAX(height, PVRTC2_MIN_TEXHEIGHT) * header->dwBitCount + 7 ) / 8;
				} else {// PVRTC4 case
					compressed_image_size = ( _MAX(width, PVRTC4_MIN_TEXWIDTH) * _MAX(height, PVRTC4_MIN_TEXHEIGHT) * header->dwBitCount + 7 ) / 8;
				}

				if ( is_compressed_format_supported ) {
					/* Load compressed texture data at selected MIP level */
					if ( loading_as_cubemap ) {
						soilGlCompressedTexImage2D( SOIL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipmap_level, PVR_format, width, height, 0, compressed_image_size, cur_texture_ptr );
					} else {
						soilGlCompressedTexImage2D( opengl_texture_type, mipmap_level, PVR_format, width, height, 0, compressed_image_size, cur_texture_ptr );
					}
				} else {
					result_string_pointer = "failed: GPU doesnt support compressed textures";
				}
			} else {
				/* Load uncompressed texture data at selected MIP level */
				if ( loading_as_cubemap ) {
					glTexImage2D( SOIL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipmap_level, PVR_type, width, height, 0, PVR_type, PVR_format, cur_texture_ptr );
				} else {
					glTexImage2D( opengl_texture_type, mipmap_level, PVR_type, width, height, 0, PVR_type, PVR_format, cur_texture_ptr );
				}
			}

			if( glGetError() ) {
				result_string_pointer = "failed: glCompressedTexImage2D() failed.";
				if ( 1 != unpack_aligment )
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_aligment);
				}
				return 0;
			}

			// offset the texture pointer by one mip-map level
			/* PVRTC case */
			if ( is_compressed_format ) {
				texture_ptr += compressed_image_size;
			} else {
				/* New formula that takes into account bit counts inferior to 8 (e.g. 1 bpp) */
				texture_ptr += (width * height * header->dwBitCount + 7) / 8;
			}
		}
	}
	#undef _MAX

	if ( 1 != unpack_aligment )
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_aligment);
	}

	if( tex_ID )
	{
		/*	did I have MIPmaps?	*/
		if( mipmaps )
		{
			/*	instruct OpenGL to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		} else
		{
			/*	instruct OpenGL _NOT_ to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		}

		/*	does the user want clamping, or wrapping?	*/
		if( flags & SOIL_FLAG_TEXTURE_REPEATS )
		{
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT );
		} else
		{
			unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;
			/* unsigned int clamp_mode = GL_CLAMP; */
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode );
			glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode );
		}
	}

	return tex_ID;
}

unsigned int SOIL_direct_load_PVR(
		const char *filename,
		unsigned int reuse_texture_ID,
		int flags,
		int loading_as_cubemap )
{
	FILE *f;
	unsigned char *buffer;
	size_t buffer_length, bytes_read;
	unsigned int tex_ID = 0;
	/*	error checks	*/
	if( NULL == filename )
	{
		result_string_pointer = "NULL filename";
		return 0;
	}
	f = fopen( filename, "rb" );
	if( NULL == f )
	{
		/*	the file doesn't seem to exist (or be open-able)	*/
		result_string_pointer = "Can not find PVR file";
		return 0;
	}
	fseek( f, 0, SEEK_END );
	buffer_length = ftell( f );
	fseek( f, 0, SEEK_SET );
	buffer = (unsigned char *) malloc( buffer_length );
	if( NULL == buffer )
	{
		result_string_pointer = "malloc failed";
		fclose( f );
		return 0;
	}
	bytes_read = fread( (void*)buffer, 1, buffer_length, f );
	fclose( f );
	if( bytes_read < buffer_length )
	{
		/*	huh?	*/
		buffer_length = bytes_read;
	}
	/*	now try to do the loading	*/
	tex_ID = SOIL_direct_load_PVR_from_memory(
		(const unsigned char *const)buffer, (int)buffer_length,
		reuse_texture_ID, flags, loading_as_cubemap );
	SOIL_free_image_data( buffer );
	return tex_ID;
}

unsigned int SOIL_direct_load_ETC1_from_memory(
		const unsigned char *const buffer,
		int buffer_length,
		unsigned int reuse_texture_ID,
		int flags )
{
	GLuint tex_ID = 0;
	PKMHeader* header = (PKMHeader*)buffer;
	unsigned int opengl_texture_type = GL_TEXTURE_2D;
	unsigned int width;
	unsigned int height;
	unsigned long compressed_image_size = buffer_length - PKM_HEADER_SIZE;
	char *texture_ptr = (char*)buffer + PKM_HEADER_SIZE;
	GLint unpack_aligment;

	if ( query_ETC1_capability() != SOIL_CAPABILITY_PRESENT ) {
		result_string_pointer = "error: ETC1 not supported. Decompress the texture first.";
		return 0;
	}

	if ( 0 != strcmp( header->aName, "PKM 10" ) ) {
		result_string_pointer = "error: PKM 10 header not found.";
		return 0;
	}

	width = (header->iWidthMSB << 8) | header->iWidthLSB;
	height = (header->iHeightMSB << 8) | header->iHeightLSB;
	compressed_image_size = (((width + 3) & ~3) * ((height + 3) & ~3)) >> 1;

	// load the texture up
	tex_ID = reuse_texture_ID;
	if( tex_ID == 0 )
	{
		glGenTextures( 1, &tex_ID );
	}

	glBindTexture( opengl_texture_type, tex_ID );

	if( glGetError() ) {
		result_string_pointer = "failed: glBindTexture() failed.";
		return 0;
	}

	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_aligment);
	if ( 1 != unpack_aligment )
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);				// Never have row-aligned in headers
	}

	soilGlCompressedTexImage2D( opengl_texture_type, 0, SOIL_GL_ETC1_RGB8_OES, width, height, 0, compressed_image_size, texture_ptr );

	if( glGetError() ) {
		result_string_pointer = "failed: glCompressedTexImage2D() failed.";

		if ( 1 != unpack_aligment )
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_aligment);
		}
		return 0;
	}

	if ( 1 != unpack_aligment )
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_aligment);
	}

	if( tex_ID )
	{
		/* No MIPmaps for ETC1 */
		glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		/*	does the user want clamping, or wrapping?	*/
		if( flags & SOIL_FLAG_TEXTURE_REPEATS )
		{
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT );
		} else
		{
			unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;
			/* unsigned int clamp_mode = GL_CLAMP; */
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode );
			glTexParameteri( opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode );
		}
	}

	return tex_ID;
}

unsigned int SOIL_direct_load_ETC1(const char *filename,
		unsigned int reuse_texture_ID,
		int flags )
{
	FILE *f;
	unsigned char *buffer;
	size_t buffer_length, bytes_read;
	unsigned int tex_ID = 0;
	/*	error checks	*/
	if( NULL == filename )
	{
		result_string_pointer = "NULL filename";
		return 0;
	}
	f = fopen( filename, "rb" );
	if( NULL == f )
	{
		/*	the file doesn't seem to exist (or be open-able)	*/
		result_string_pointer = "Can not find PVR file";
		return 0;
	}
	fseek( f, 0, SEEK_END );
	buffer_length = ftell( f );
	fseek( f, 0, SEEK_SET );
	buffer = (unsigned char *) malloc( buffer_length );
	if( NULL == buffer )
	{
		result_string_pointer = "malloc failed";
		fclose( f );
		return 0;
	}
	bytes_read = fread( (void*)buffer, 1, buffer_length, f );
	fclose( f );
	if( bytes_read < buffer_length )
	{
		/*	huh?	*/
		buffer_length = bytes_read;
	}
	/*	now try to do the loading	*/
	tex_ID = SOIL_direct_load_ETC1_from_memory(
		(const unsigned char *const)buffer, (int)buffer_length,
		reuse_texture_ID, flags );
	SOIL_free_image_data( buffer );
	return tex_ID;
}

int query_NPOT_capability( void )
{
	/*	check for the capability	*/
	if( has_NPOT_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if(
			(0 == SOIL_GL_ExtensionSupported(
				"GL_ARB_texture_non_power_of_two" ) )
		&&
			(0 == SOIL_GL_ExtensionSupported(
				"GL_OES_texture_npot" ) )
			)
		{
			/*	not there, flag the failure	*/
			has_NPOT_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			/*	it's there!	*/
			has_NPOT_capability = SOIL_CAPABILITY_PRESENT;
		}
	}
	/*	let the user know if we can do non-power-of-two textures or not	*/
	return has_NPOT_capability;
}

int query_tex_rectangle_capability( void )
{
	/*	check for the capability	*/
	if( has_tex_rectangle_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if(
			(0 == SOIL_GL_ExtensionSupported(
				"GL_ARB_texture_rectangle" ) )
		&&
			(0 == SOIL_GL_ExtensionSupported(
				"GL_EXT_texture_rectangle" ) )
		&&
			(0 == SOIL_GL_ExtensionSupported(
				"GL_NV_texture_rectangle" ) )
			)
		{
			/*	not there, flag the failure	*/
			has_tex_rectangle_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			/*	it's there!	*/
			has_tex_rectangle_capability = SOIL_CAPABILITY_PRESENT;
		}
	}
	/*	let the user know if we can do texture rectangles or not	*/
	return has_tex_rectangle_capability;
}

int query_cubemap_capability( void )
{
	/*	check for the capability	*/
	if( has_cubemap_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if(
			(0 == SOIL_GL_ExtensionSupported(
				"GL_ARB_texture_cube_map" ) )
		&&
			(0 == SOIL_GL_ExtensionSupported(
				"GL_EXT_texture_cube_map" ) )
			)
		{
			/*	not there, flag the failure	*/
			has_cubemap_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			/*	it's there!	*/
			has_cubemap_capability = SOIL_CAPABILITY_PRESENT;
		}
	}
	/*	let the user know if we can do cubemaps or not	*/
	return has_cubemap_capability;
}

static P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC get_glCompressedTexImage2D_addr()
{
	/*	and find the address of the extension function	*/
	P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC ext_addr = NULL;
	
#if defined( SOIL_PLATFORM_WIN32 ) || defined( SOIL_PLATFORM_OSX ) || defined( SOIL_X11_PLATFORM )
	ext_addr = (P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC)SOIL_GL_GetProcAddress( "glCompressedTexImage2D" );
#else
	ext_addr = (P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC)&glCompressedTexImage2D;
#endif
	
	return ext_addr;
}

int query_DXT_capability( void )
{
	/*	check for the capability	*/
	if( has_DXT_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if (	0 == SOIL_GL_ExtensionSupported(
					"GL_EXT_texture_compression_s3tc" )  &&
				0 == SOIL_GL_ExtensionSupported(
					"WEBGL_compressed_texture_s3tc ") &&
				0 == SOIL_GL_ExtensionSupported(
					"WEBKIT_WEBGL_compressed_texture_s3tc") &&
				0 == SOIL_GL_ExtensionSupported(
					"MOZ_WEBGL_compressed_texture_s3tc"
				)
			)
		{
			/*	not there, flag the failure	*/
			has_DXT_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC ext_addr = get_glCompressedTexImage2D_addr();

			/*	Flag it so no checks needed later	*/
			if( NULL == ext_addr )
			{
				/*	hmm, not good!!  This should not happen, but does on my
					laptop's VIA chipset.  The GL_EXT_texture_compression_s3tc
					spec requires that ARB_texture_compression be present too.
					this means I can upload and have the OpenGL drive do the
					conversion, but I can't use my own routines or load DDS files
					from disk and upload them directly [8^(	*/
				has_DXT_capability = SOIL_CAPABILITY_NONE;
			} else
			{
				/*	all's well!	*/
				soilGlCompressedTexImage2D = ext_addr;
				has_DXT_capability = SOIL_CAPABILITY_PRESENT;
			}
		}
	}
	/*	let the user know if we can do DXT or not	*/
	return has_DXT_capability;
}

int query_PVR_capability( void )
{
	/*	check for the capability	*/
	if( has_PVR_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if (0 == SOIL_GL_ExtensionSupported(
				"GL_IMG_texture_compression_pvrtc" ) )
		{
			/*	not there, flag the failure	*/
			has_PVR_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			if ( NULL == soilGlCompressedTexImage2D ) {
				soilGlCompressedTexImage2D = get_glCompressedTexImage2D_addr();
			}
			
			/*	it's there!	*/
			has_PVR_capability = SOIL_CAPABILITY_PRESENT;
		}
	}
	/*	let the user know if we can do cubemaps or not	*/
	return has_PVR_capability;
}

int query_BGRA8888_capability( void )
{
	/*	check for the capability	*/
	if( has_BGRA8888_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if (0 == SOIL_GL_ExtensionSupported(
				"GL_IMG_texture_format_BGRA8888" ) )
		{
			/*	not there, flag the failure	*/
			has_BGRA8888_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			/*	it's there!	*/
			has_BGRA8888_capability = SOIL_CAPABILITY_PRESENT;
		}
	}
	/*	let the user know if we can do cubemaps or not	*/
	return has_BGRA8888_capability;
}

int query_ETC1_capability( void )
{
	/*	check for the capability	*/
	if( has_ETC1_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		/*	we haven't yet checked for the capability, do so	*/
		if (0 == SOIL_GL_ExtensionSupported(
				"GL_OES_compressed_ETC1_RGB8_texture" ) )
		{
			/*	not there, flag the failure	*/
			has_ETC1_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			if ( NULL == soilGlCompressedTexImage2D ) {
				soilGlCompressedTexImage2D = get_glCompressedTexImage2D_addr();
			}

			/*	it's there!	*/
			has_ETC1_capability = SOIL_CAPABILITY_PRESENT;
		}
	}
	/*	let the user know if we can do cubemaps or not	*/
	return has_ETC1_capability;
}

int query_gen_mipmap_capability( void )
{
	/* check for the capability   */
	P_SOIL_GLGENERATEMIPMAPPROC ext_addr = NULL;

	if( has_gen_mipmap_capability == SOIL_CAPABILITY_UNKNOWN )
	{
		if (	0 == SOIL_GL_ExtensionSupported(
					"GL_ARB_framebuffer_object" )
			&&
				0 == SOIL_GL_ExtensionSupported(
					"GL_EXT_framebuffer_object" )
			&&  0 == SOIL_GL_ExtensionSupported(
					"GL_OES_framebuffer_object" )
			)
		{
			/* not there, flag the failure */
			has_gen_mipmap_capability = SOIL_CAPABILITY_NONE;
		}
		else
		{
			#if !defined( SOIL_GLES1 ) && !defined( SOIL_GLES2 )

			ext_addr = (P_SOIL_GLGENERATEMIPMAPPROC)SOIL_GL_GetProcAddress("glGenerateMipmap");

			if(ext_addr == NULL)
			{
				ext_addr = (P_SOIL_GLGENERATEMIPMAPPROC)SOIL_GL_GetProcAddress("glGenerateMipmapEXT");
			}

			#elif defined( SOIL_GLES2 )
				ext_addr = 	&glGenerateMipmap;
			#else /** SOIL_GLES1 */
				ext_addr = &glGenerateMipmapOES;
			#endif
		}

		if(ext_addr == NULL)
		{
			/* this should never happen */
			has_gen_mipmap_capability = SOIL_CAPABILITY_NONE;
		} else
		{
			/* it's there! */
			has_gen_mipmap_capability = SOIL_CAPABILITY_PRESENT;
			soilGlGenerateMipmap = ext_addr;
		}
	}

	return has_gen_mipmap_capability;
}
