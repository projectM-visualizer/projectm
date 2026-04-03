
#include "wfETC.h"

// specification: http://www.khronos.org/registry/gles/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt

#ifdef WF_LITTLE_ENDIAN
	#define WF_ETC1_COLOR_OFFSET( offset ) ( (64-(offset)) - (4-((offset)%8))*2 )
	#define WF_ETC1_PIXEL_OFFSET( offset ) ( (32-(offset)) - (4-((offset)%8))*2 )
#else
	#define WF_ETC1_COLOR_OFFSET( offset ) (offset-32)
	#define WF_ETC1_PIXEL_OFFSET( offset ) offset
#endif

#define WF_INLINE
#define WF_ASSUME( expr )

#ifndef WF_EXPECT
	#if defined __GNUC__ && __GNUC__
		#define WF_EXPECT( expr, val ) __builtin_expect( expr, val )
	#else
		#define WF_EXPECT( expr, val ) expr
	#endif
#endif

// this table is rearranged from the specification so we do not have to add any logic to index into it
const int32_t wfETC_IntensityTables[8][4] = 
{
	{  2,   8,  -2, -8   },
	{  5,  17,  -5, -17  },
	{  9,  29,  -9, -29  },
	{ 13,  42, -13, -42  },
	{ 18,  60, -18, -60  },
	{ 24,  80, -24, -80  },
	{ 33, 106, -33, -106 },
	{ 47, 183, -47, -183 }
};

WF_INLINE
int32_t wfETC_ClampColor( const int32_t x )
{
	if( x < 0   ) { return 0; }
	if( x > 255 ) { return 255; }
	return x;
}

#define WF_ETC1_BUILD_COLOR( dst, blockIdx, colorIdx, intensityTable, baseColor ) \
	dst[ blockIdx ][ colorIdx ] = WF_ETC_RGBA( \
		wfETC_ClampColor( intensityTable[ blockIdx ][ colorIdx ] + baseColor[ blockIdx ][ 0 ] ), \
		wfETC_ClampColor( intensityTable[ blockIdx ][ colorIdx ] + baseColor[ blockIdx ][ 1 ] ), \
		wfETC_ClampColor( intensityTable[ blockIdx ][ colorIdx ] + baseColor[ blockIdx ][ 2 ] ), \
		255 \
	);

typedef struct _wfETC1_Block
{
	int32_t baseColorsAndFlags;
	int32_t pixels;
} wfETC1_Block;

#define WF_ETC1_CHECK_DIFF_BIT( block ) ( block->baseColorsAndFlags & (1<<WF_ETC1_COLOR_OFFSET(33)) )
#define WF_ETC1_CHECK_FLIP_BIT( block ) ( block->baseColorsAndFlags & (1<<WF_ETC1_COLOR_OFFSET(32)) )

WF_INLINE
int32_t wfETC1_ReadColor4( const wfETC1_Block* block, const uint32_t offset )
{
	const uint32_t bitOffset = WF_ETC1_COLOR_OFFSET(offset);
	const int32_t color = ( block->baseColorsAndFlags >> bitOffset ) & 0xf;
	return color | (color<<4);
}

const int32_t wfETC1_Color3IdxLUT[] = { 0, 1, 2, 3, -4, -3, -2, -1 };

WF_INLINE
void wfETC1_ReadColor53( const wfETC1_Block* block, const uint32_t offset3, int32_t* WF_RESTRICT dst5, int32_t* WF_RESTRICT dst3 )
{
	// read the 5 bit color and expand to 8 bit
	{
		const uint32_t bitOffset = WF_ETC1_COLOR_OFFSET((offset3+3));
		const int32_t color = ( block->baseColorsAndFlags >> (bitOffset-3) ) & 0xf8;
		*dst5 = color | (color>>5);
	}

	// read the 3 bit color and expand to 8 bit
	{
		const uint32_t bitOffset = WF_ETC1_COLOR_OFFSET(offset3);
		const int32_t offset = wfETC1_Color3IdxLUT[ ( block->baseColorsAndFlags >> bitOffset ) & 0x7 ];
		int32_t color = (*dst5>>3) + offset;
		color <<= 3;
		color |= (color>>5) & 0x7;
		*dst3 = color;
	}
}

WF_INLINE
int32_t wfETC1_ReadPixel( const wfETC1_Block* block, const uint32_t offset )
{
	return
		(  (block->pixels>>WF_ETC1_PIXEL_OFFSET(offset)) & 0x1  )
		|
		(  ( (block->pixels>>WF_ETC1_PIXEL_OFFSET(16+offset)) & 0x1 ) << 1 )
	;
}

void wfETC1_DecodeBlock( const void* WF_RESTRICT src, void* WF_RESTRICT pDst, const uint32_t dstStride )
{
	const wfETC1_Block* WF_RESTRICT block = ( wfETC1_Block* )src;
	int32_t* WF_RESTRICT dst = (int32_t*)pDst;

	int32_t baseColors[2][3]; // [sub-block][r,g,b]
	int32_t colors[2][4]; // [sub-block][colorIdx]

	// individual mode
	if( WF_ETC1_CHECK_DIFF_BIT( block ) == 0 )
	{
		baseColors[0][0] = wfETC1_ReadColor4( block, 60 );
		baseColors[0][1] = wfETC1_ReadColor4( block, 52 );
		baseColors[0][2] = wfETC1_ReadColor4( block, 44 );
		baseColors[1][0] = wfETC1_ReadColor4( block, 56 );
		baseColors[1][1] = wfETC1_ReadColor4( block, 48 );
		baseColors[1][2] = wfETC1_ReadColor4( block, 40 );
	}
	// differential mode
	else
	{
		wfETC1_ReadColor53( block, 56, &baseColors[0][0], &baseColors[1][0] );
		wfETC1_ReadColor53( block, 48, &baseColors[0][1], &baseColors[1][1] );
		wfETC1_ReadColor53( block, 40, &baseColors[0][2], &baseColors[1][2] );
	}

	// build color tables
	{
		const int32_t* intensityTable[2] = // [sub-block]
		{
			wfETC_IntensityTables[ ( block->baseColorsAndFlags >> WF_ETC1_COLOR_OFFSET(37) ) & 0x7 ],
			wfETC_IntensityTables[ ( block->baseColorsAndFlags >> WF_ETC1_COLOR_OFFSET(34) ) & 0x7 ]
		};
		WF_ETC1_BUILD_COLOR( colors, 0, 0, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 0, 1, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 0, 2, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 0, 3, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 1, 0, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 1, 1, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 1, 2, intensityTable, baseColors );
		WF_ETC1_BUILD_COLOR( colors, 1, 3, intensityTable, baseColors );
	}

	// vertical split
	if( WF_ETC1_CHECK_FLIP_BIT( block ) == 0 )
	{
		// row 0
		dst[0] = colors[0][ wfETC1_ReadPixel( block, 0  ) ];
		dst[1] = colors[0][ wfETC1_ReadPixel( block, 4  ) ];
		dst[2] = colors[1][ wfETC1_ReadPixel( block, 8  ) ];
		dst[3] = colors[1][ wfETC1_ReadPixel( block, 12 ) ];
		dst += dstStride;

		// row 1
		dst[0] = colors[0][ wfETC1_ReadPixel( block, 1  ) ];
		dst[1] = colors[0][ wfETC1_ReadPixel( block, 5  ) ];
		dst[2] = colors[1][ wfETC1_ReadPixel( block, 9  ) ];
		dst[3] = colors[1][ wfETC1_ReadPixel( block, 13 ) ];
		dst += dstStride;

		// row 2
		dst[0] = colors[0][ wfETC1_ReadPixel( block, 2  ) ];
		dst[1] = colors[0][ wfETC1_ReadPixel( block, 6  ) ];
		dst[2] = colors[1][ wfETC1_ReadPixel( block, 10 ) ];
		dst[3] = colors[1][ wfETC1_ReadPixel( block, 14 ) ];
		dst += dstStride;

		// row 3
		dst[0] = colors[0][ wfETC1_ReadPixel( block, 3  ) ];
		dst[1] = colors[0][ wfETC1_ReadPixel( block, 7  ) ];
		dst[2] = colors[1][ wfETC1_ReadPixel( block, 11 ) ];
		dst[3] = colors[1][ wfETC1_ReadPixel( block, 15 ) ];
		//dst += dstStride;
	}
	// horizontal split
	else
	{
		// row 0
		dst[0] = colors[0][ wfETC1_ReadPixel( block, 0  ) ];
		dst[1] = colors[0][ wfETC1_ReadPixel( block, 4  ) ];
		dst[2] = colors[0][ wfETC1_ReadPixel( block, 8  ) ];
		dst[3] = colors[0][ wfETC1_ReadPixel( block, 12 ) ];
		dst += dstStride;

		// row 1
		dst[0] = colors[0][ wfETC1_ReadPixel( block, 1  ) ];
		dst[1] = colors[0][ wfETC1_ReadPixel( block, 5  ) ];
		dst[2] = colors[0][ wfETC1_ReadPixel( block, 9  ) ];
		dst[3] = colors[0][ wfETC1_ReadPixel( block, 13 ) ];
		dst += dstStride;

		// row 2
		dst[0] = colors[1][ wfETC1_ReadPixel( block, 2  ) ];
		dst[1] = colors[1][ wfETC1_ReadPixel( block, 6  ) ];
		dst[2] = colors[1][ wfETC1_ReadPixel( block, 10 ) ];
		dst[3] = colors[1][ wfETC1_ReadPixel( block, 14 ) ];
		dst += dstStride;

		// row 3
		dst[0] = colors[1][ wfETC1_ReadPixel( block, 3  ) ];
		dst[1] = colors[1][ wfETC1_ReadPixel( block, 7  ) ];
		dst[2] = colors[1][ wfETC1_ReadPixel( block, 11 ) ];
		dst[3] = colors[1][ wfETC1_ReadPixel( block, 15 ) ];
		//dst += dstStride;
	}
}

void wfETC1_DecodeImage( const void* WF_RESTRICT pSrc, void* WF_RESTRICT pDst, const uint32_t width, const uint32_t height )
{
	const uint8_t* WF_RESTRICT src = (uint8_t*)pSrc;
	uint8_t* WF_RESTRICT dst = (uint8_t*)pDst;
	const uint32_t widthBlocks  = width/4;
	const uint32_t heightBlocks = height/4;
	uint32_t x, y;
	for( y = 0; y != heightBlocks; ++y )
	{
		for( x = 0; x != widthBlocks; ++x )
		{
			wfETC1_DecodeBlock( src, dst, width );
			src += 8;
			dst += 16;
		}
		dst += width*4*3;
	}
}
