#ifndef PVR_HELPER_H
#define PVR_HELPER_H

// Taken from PowerVR SDK

/*!***************************************************************************
 Describes the header of a PVR header-texture
 *****************************************************************************/
typedef struct
{
	unsigned int dwHeaderSize;			/*!< size of the structure */
	unsigned int dwHeight;				/*!< height of surface to be created */
	unsigned int dwWidth;				/*!< width of input surface */
	unsigned int dwMipMapCount;			/*!< number of mip-map levels requested */
	unsigned int dwpfFlags;				/*!< pixel format flags */
	unsigned int dwTextureDataSize;		/*!< Total size in bytes */
	unsigned int dwBitCount;			/*!< number of bits per pixel  */
	unsigned int dwRBitMask;			/*!< mask for red bit */
	unsigned int dwGBitMask;			/*!< mask for green bits */
	unsigned int dwBBitMask;			/*!< mask for blue bits */
	unsigned int dwAlphaBitMask;		/*!< mask for alpha channel */
	unsigned int dwPVR;					/*!< magic number identifying pvr file */
	unsigned int dwNumSurfs;			/*!< the number of surfaces present in the pvr */
} PVR_Texture_Header;

/*****************************************************************************
 * ENUMS
 *****************************************************************************/

enum PixelType
{
	MGLPT_ARGB_4444 = 0x00,
	MGLPT_ARGB_1555,
	MGLPT_RGB_565,
	MGLPT_RGB_555,
	MGLPT_RGB_888,
	MGLPT_ARGB_8888,
	MGLPT_ARGB_8332,
	MGLPT_I_8,
	MGLPT_AI_88,
	MGLPT_1_BPP,
	MGLPT_VY1UY0,
	MGLPT_Y1VY0U,
	MGLPT_PVRTC2,
	MGLPT_PVRTC4,
	MGLPT_PVRTC2_2,
	MGLPT_PVRTC2_4,
	
	OGL_RGBA_4444= 0x10,
	OGL_RGBA_5551,
	OGL_RGBA_8888,
	OGL_RGB_565,
	OGL_RGB_555,
	OGL_RGB_888,
	OGL_I_8,
	OGL_AI_88,
	OGL_PVRTC2,
	OGL_PVRTC4,
	
	// OGL_BGRA_8888 extension
	OGL_BGRA_8888,
	
	D3D_DXT1 = 0x20,
	D3D_DXT2,
	D3D_DXT3,
	D3D_DXT4,
	D3D_DXT5,
	
	D3D_RGB_332,
	D3D_AI_44,
	D3D_LVU_655,
	D3D_XLVU_8888,
	D3D_QWVU_8888,
	
	//10 bits per channel
	D3D_ABGR_2101010,
	D3D_ARGB_2101010,
	D3D_AWVU_2101010,
	
	//16 bits per channel
	D3D_GR_1616,
	D3D_VU_1616,
	D3D_ABGR_16161616,
	
	//HDR formats
	D3D_R16F,
	D3D_GR_1616F,
	D3D_ABGR_16161616F,
	
	//32 bits per channel
	D3D_R32F,
	D3D_GR_3232F,
	D3D_ABGR_32323232F,
	
	// Ericsson
	ETC_RGB_4BPP,
	ETC_RGBA_EXPLICIT,
	ETC_RGBA_INTERPOLATED,
	
	// DX10
	
	
	ePT_DX10_R32G32B32A32_FLOAT= 0x50,
	ePT_DX10_R32G32B32A32_UINT ,
	ePT_DX10_R32G32B32A32_SINT,
	
	ePT_DX10_R32G32B32_FLOAT,
	ePT_DX10_R32G32B32_UINT,
	ePT_DX10_R32G32B32_SINT,
	
	ePT_DX10_R16G16B16A16_FLOAT ,
	ePT_DX10_R16G16B16A16_UNORM,
	ePT_DX10_R16G16B16A16_UINT ,
	ePT_DX10_R16G16B16A16_SNORM ,
	ePT_DX10_R16G16B16A16_SINT ,
	
	ePT_DX10_R32G32_FLOAT ,
	ePT_DX10_R32G32_UINT ,
	ePT_DX10_R32G32_SINT ,
	
	ePT_DX10_R10G10B10A2_UNORM ,
	ePT_DX10_R10G10B10A2_UINT ,
	
	ePT_DX10_R11G11B10_FLOAT ,
	
	ePT_DX10_R8G8B8A8_UNORM ,
	ePT_DX10_R8G8B8A8_UNORM_SRGB ,
	ePT_DX10_R8G8B8A8_UINT ,
	ePT_DX10_R8G8B8A8_SNORM ,
	ePT_DX10_R8G8B8A8_SINT ,
	
	ePT_DX10_R16G16_FLOAT ,
	ePT_DX10_R16G16_UNORM ,
	ePT_DX10_R16G16_UINT ,
	ePT_DX10_R16G16_SNORM ,
	ePT_DX10_R16G16_SINT ,
	
	ePT_DX10_R32_FLOAT ,
	ePT_DX10_R32_UINT ,
	ePT_DX10_R32_SINT ,
	
	ePT_DX10_R8G8_UNORM ,
	ePT_DX10_R8G8_UINT ,
	ePT_DX10_R8G8_SNORM ,
	ePT_DX10_R8G8_SINT ,
	
	ePT_DX10_R16_FLOAT ,
	ePT_DX10_R16_UNORM ,
	ePT_DX10_R16_UINT ,
	ePT_DX10_R16_SNORM ,
	ePT_DX10_R16_SINT ,
	
	ePT_DX10_R8_UNORM,
	ePT_DX10_R8_UINT,
	ePT_DX10_R8_SNORM,
	ePT_DX10_R8_SINT,
	
	ePT_DX10_A8_UNORM,
	ePT_DX10_R1_UNORM,
	ePT_DX10_R9G9B9E5_SHAREDEXP,
	ePT_DX10_R8G8_B8G8_UNORM,
	ePT_DX10_G8R8_G8B8_UNORM,
	
	ePT_DX10_BC1_UNORM,
	ePT_DX10_BC1_UNORM_SRGB,
	
	ePT_DX10_BC2_UNORM,
	ePT_DX10_BC2_UNORM_SRGB,
	
	ePT_DX10_BC3_UNORM,
	ePT_DX10_BC3_UNORM_SRGB,
	
	ePT_DX10_BC4_UNORM,
	ePT_DX10_BC4_SNORM,
	
	ePT_DX10_BC5_UNORM,
	ePT_DX10_BC5_SNORM,
	
	//ePT_DX10_B5G6R5_UNORM,			// defined but obsolete - won't actually load in DX10
	//ePT_DX10_B5G5R5A1_UNORM,
	//ePT_DX10_B8G8R8A8_UNORM,
	//ePT_DX10_B8G8R8X8_UNORM,
	
	// OpenVG
	
	/* RGB{A,X} channel ordering */
	ePT_VG_sRGBX_8888  = 0x90,
	ePT_VG_sRGBA_8888,
	ePT_VG_sRGBA_8888_PRE,
	ePT_VG_sRGB_565,
	ePT_VG_sRGBA_5551,
	ePT_VG_sRGBA_4444,
	ePT_VG_sL_8,
	ePT_VG_lRGBX_8888,
	ePT_VG_lRGBA_8888,
	ePT_VG_lRGBA_8888_PRE,
	ePT_VG_lL_8,
	ePT_VG_A_8,
	ePT_VG_BW_1,
	
	/* {A,X}RGB channel ordering */
	ePT_VG_sXRGB_8888,
	ePT_VG_sARGB_8888,
	ePT_VG_sARGB_8888_PRE,
	ePT_VG_sARGB_1555,
	ePT_VG_sARGB_4444,
	ePT_VG_lXRGB_8888,
	ePT_VG_lARGB_8888,
	ePT_VG_lARGB_8888_PRE,
	
	/* BGR{A,X} channel ordering */
	ePT_VG_sBGRX_8888,
	ePT_VG_sBGRA_8888,
	ePT_VG_sBGRA_8888_PRE,
	ePT_VG_sBGR_565,
	ePT_VG_sBGRA_5551,
	ePT_VG_sBGRA_4444,
	ePT_VG_lBGRX_8888,
	ePT_VG_lBGRA_8888,
	ePT_VG_lBGRA_8888_PRE,
	
	/* {A,X}BGR channel ordering */
	ePT_VG_sXBGR_8888,
	ePT_VG_sABGR_8888 ,
	ePT_VG_sABGR_8888_PRE,
	ePT_VG_sABGR_1555,
	ePT_VG_sABGR_4444,
	ePT_VG_lXBGR_8888,
	ePT_VG_lABGR_8888,
	ePT_VG_lABGR_8888_PRE,
	
	// max cap for iterating
	END_OF_PIXEL_TYPES,
	
	MGLPT_NOTYPE = 0xff
	
};

/*****************************************************************************
 * constants
 *****************************************************************************/

#define PVRTEX_MIPMAP		(1<<8)		// has mip map levels
#define PVRTEX_TWIDDLE		(1<<9)		// is twiddled
#define PVRTEX_BUMPMAP		(1<<10)		// has normals encoded for a bump map
#define PVRTEX_TILING		(1<<11)		// is bordered for tiled pvr
#define PVRTEX_CUBEMAP		(1<<12)		// is a cubemap/skybox
#define PVRTEX_FALSEMIPCOL	(1<<13)		//
#define PVRTEX_VOLUME		(1<<14)
#define PVRTEX_PIXELTYPE	0xff			// pixel type is always in the last 16bits of the flags
#define PVRTEX_IDENTIFIER	0x21525650	// the pvr identifier is the characters 'P','V','R'

#define PVRTEX_V1_HEADER_SIZE 44			// old header size was 44 for identification purposes

#define PVRTC2_MIN_TEXWIDTH		16
#define PVRTC2_MIN_TEXHEIGHT	8
#define PVRTC4_MIN_TEXWIDTH		8
#define PVRTC4_MIN_TEXHEIGHT	8
#define ETC_MIN_TEXWIDTH		4
#define ETC_MIN_TEXHEIGHT		4
#define DXT_MIN_TEXWIDTH		4
#define DXT_MIN_TEXHEIGHT		4

#endif
