#include "pvr_helper.h"

static int stbi__pvr_test(stbi__context *s)
{
	//	check header size
	if (stbi__get32le(s) != sizeof(PVR_Texture_Header)) {
		stbi__rewind(s);
		return 0;
	}

	// stbi__skip until the magic number
	stbi__skip(s, 10*4);

	// check the magic number
	if ( stbi__get32le(s) != PVRTEX_IDENTIFIER ) {
		stbi__rewind(s);
		return 0;
	}

	// Also rewind because the loader needs to read the header
	stbi__rewind(s);

	return 1;
}

#ifndef STBI_NO_STDIO

int      stbi__pvr_test_filename        		(char const *filename)
{
   int r;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   r = stbi__pvr_test_file(f);
   fclose(f);
   return r;
}

int      stbi__pvr_test_file        (FILE *f)
{
   stbi__context s;
   int r,n = ftell(f);
   stbi__start_file(&s,f);
   r = stbi__pvr_test(&s);
   fseek(f,n,SEEK_SET);
   return r;
}
#endif

int      stbi__pvr_test_memory      (stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__pvr_test(&s);
}

int      stbi__pvr_test_callbacks      (stbi_io_callbacks const *clbk, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__pvr_test(&s);
}

static int stbi__pvr_info(stbi__context *s, int *x, int *y, int *comp, int * iscompressed )
{
	PVR_Texture_Header header={0};

	stbi__getn( s, (stbi_uc*)(&header), sizeof(PVR_Texture_Header) );

	// Check the header size
	if ( header.dwHeaderSize != sizeof(PVR_Texture_Header) ) {
		stbi__rewind( s );
		return 0;
	}

	// Check the magic identifier
	if ( header.dwPVR != PVRTEX_IDENTIFIER ) {
		stbi__rewind(s);
		return 0;
	}

	*x = s->img_x = header.dwWidth;
	*y = s->img_y = header.dwHeight;
	*comp = s->img_n = ( header.dwBitCount + 7 ) / 8;

	if ( iscompressed )
		*iscompressed = 0;

	switch ( header.dwpfFlags & PVRTEX_PIXELTYPE )
	{
		case OGL_RGBA_4444:
			s->img_n = 2;
			break;
		case OGL_RGBA_5551:
			s->img_n = 2;
			break;
		case OGL_RGBA_8888:
			s->img_n = 4;
			break;
		case OGL_RGB_565:
			s->img_n = 2;
			break;
		case OGL_RGB_888:
			s->img_n = 3;
			break;
		case OGL_I_8:
			s->img_n = 1;
			break;
		case OGL_AI_88:
			s->img_n = 2;
			break;
		case OGL_PVRTC2:
			s->img_n = 4;
			if ( iscompressed )
				*iscompressed = 1;
			break;
		case OGL_PVRTC4:
			s->img_n = 4;
			if ( iscompressed )
				*iscompressed = 1;
			break;
		case OGL_RGB_555:
		default:
			stbi__rewind(s);
			return 0;
	}

	*comp = s->img_n;

	return 1;
}

int stbi__pvr_info_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *comp, int * iscompressed )
{
	stbi__context s;
	stbi__start_mem(&s,buffer, len);
	return stbi__pvr_info( &s, x, y, comp, iscompressed );
}

int stbi__pvr_info_from_callbacks (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int * iscompressed)
{
	stbi__context s;
	stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
	return stbi__pvr_info( &s, x, y, comp, iscompressed );
}

#ifndef STBI_NO_STDIO
int stbi__pvr_info_from_path(char const *filename,     int *x, int *y, int *comp, int * iscompressed)
{
   int res;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   res = stbi__pvr_info_from_file( f, x, y, comp, iscompressed );
   fclose(f);
   return res;
}

int stbi__pvr_info_from_file(FILE *f,                  int *x, int *y, int *comp, int * iscompressed)
{
   stbi__context s;
   int res;
   long n = ftell(f);
   stbi__start_file(&s, f);
   res = stbi__pvr_info(&s, x, y, comp, iscompressed);
   fseek(f, n, SEEK_SET);
   return res;
}
#endif

/******************************************************************************
 Taken from:
 @File         PVRTDecompress.cpp
 @Title        PVRTDecompress
 @Copyright    Copyright (C)  Imagination Technologies Limited.
 @Platform     ANSI compatible
 @Description  PVRTC Texture Decompression.
******************************************************************************/

typedef unsigned char      PVRTuint8;
typedef unsigned short     PVRTuint16;
typedef unsigned int       PVRTuint32;

/*****************************************************************************
 * defines and consts
 *****************************************************************************/
#define PT_INDEX (2)	// The Punch-through index

#define BLK_Y_SIZE 	(4) // always 4 for all 2D block types

#define BLK_X_MAX	(8)	// Max X dimension for blocks

#define BLK_X_2BPP	(8) // dimensions for the two formats
#define BLK_X_4BPP	(4)

#define WRAP_COORD(Val, Size) ((Val) & ((Size)-1))

#define POWER_OF_2(X)   util_number_is_power_2(X)

/*
	Define an expression to either wrap or clamp large or small vals to the
	legal coordinate range
*/
#define PVRT_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define PVRT_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define PVRT_CLAMP(x, l, h)      (PVRT_MIN((h), PVRT_MAX((x), (l))))

#define LIMIT_COORD(Val, Size, AssumeImageTiles) \
	  ((AssumeImageTiles)? WRAP_COORD((Val), (Size)): PVRT_CLAMP((Val), 0, (Size)-1))

/*****************************************************************************
 * Useful typedefs
 *****************************************************************************/
typedef PVRTuint32 U32;
typedef PVRTuint8 U8;

/***********************************************************
				DECOMPRESSION ROUTINES
************************************************************/

/*!***********************************************************************
 @Struct	AMTC_BLOCK_STRUCT
 @Brief
*************************************************************************/
typedef struct
{
	// Uses 64 bits pre block
	U32 PackedData[2];
}AMTC_BLOCK_STRUCT;

 /*!***********************************************************************
  @Function		util_number_is_power_2
  @Input		input A number
  @Returns		TRUE if the number is an integer power of two, else FALSE.
  @Description	Check that a number is an integer power of two, i.e.
				1, 2, 4, 8, ... etc.
				Returns FALSE for zero.
*************************************************************************/
int util_number_is_power_2( unsigned  input )
{
  unsigned minus1;

  if( !input ) return 0;

  minus1 = input - 1;
  return ( (input | minus1) == (input ^ minus1) ) ? 1 : 0;
}

/*!***********************************************************************
 @Function		Unpack5554Colour
 @Input			pBlock
 @Input			ABColours
 @Description	Given a block, extract the colour information and convert
				to 5554 formats
*************************************************************************/
static void Unpack5554Colour(const AMTC_BLOCK_STRUCT *pBlock,
							 int   ABColours[2][4])
{
	U32 RawBits[2];

	int i;

	// Extract A and B
	RawBits[0] = pBlock->PackedData[1] & (0xFFFE); // 15 bits (shifted up by one)
	RawBits[1] = pBlock->PackedData[1] >> 16;	   // 16 bits

	// step through both colours
	for(i = 0; i < 2; i++)
	{
		// If completely opaque
		if(RawBits[i] & (1<<15))
		{
			// Extract R and G (both 5 bit)
			ABColours[i][0] = (RawBits[i] >> 10) & 0x1F;
			ABColours[i][1] = (RawBits[i] >>  5) & 0x1F;

			/*
				The precision of Blue depends on  A or B. If A then we need to
				replicate the top bit to get 5 bits in total
			*/
			ABColours[i][2] = RawBits[i] & 0x1F;
			if(i==0)
			{
				ABColours[0][2] |= ABColours[0][2] >> 4;
			}

			// set 4bit alpha fully on...
			ABColours[i][3] = 0xF;
		}
		else // Else if colour has variable translucency
		{
			/*
				Extract R and G (both 4 bit).
				(Leave a space on the end for the replication of bits
			*/
			ABColours[i][0] = (RawBits[i] >>  (8-1)) & 0x1E;
			ABColours[i][1] = (RawBits[i] >>  (4-1)) & 0x1E;

			// replicate bits to truly expand to 5 bits
			ABColours[i][0] |= ABColours[i][0] >> 4;
			ABColours[i][1] |= ABColours[i][1] >> 4;

			// grab the 3(+padding) or 4 bits of blue and add an extra padding bit
			ABColours[i][2] = (RawBits[i] & 0xF) << 1;

			/*
				expand from 3 to 5 bits if this is from colour A, or 4 to 5 bits if from
				colour B
			*/
			if(i==0)
			{
				ABColours[0][2] |= ABColours[0][2] >> 3;
			}
			else
			{
				ABColours[0][2] |= ABColours[0][2] >> 4;
			}

			// Set the alpha bits to be 3 + a zero on the end
			ABColours[i][3] = (RawBits[i] >> 11) & 0xE;
		}
	}
}

/*!***********************************************************************
 @Function		UnpackModulations
 @Input			pBlock
 @Input			Do2bitMode
 @Input			ModulationVals
 @Input			ModulationModes
 @Input			StartX
 @Input			StartY
 @Description	Given the block and the texture type and it's relative
				position in the 2x2 group of blocks, extract the bit
				patterns for the fully defined pixels.
*************************************************************************/
static void	UnpackModulations(const AMTC_BLOCK_STRUCT *pBlock,
							  const int Do2bitMode,
							  int ModulationVals[8][16],
							  int ModulationModes[8][16],
							  int StartX,
							  int StartY)
{
	int BlockModMode;
	U32 ModulationBits;

	int x, y;

	BlockModMode= pBlock->PackedData[1] & 1;
	ModulationBits	= pBlock->PackedData[0];

	// if it's in an interpolated mode
	if(Do2bitMode && BlockModMode)
	{
		/*
			run through all the pixels in the block. Note we can now treat all the
			"stored" values as if they have 2bits (even when they didn't!)
		*/
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_2BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;

				// if this is a stored value...
				if(((x^y)&1) == 0)
				{
					ModulationVals[y+StartY][x+StartX] = ModulationBits & 3;
					ModulationBits >>= 2;
				}
			}
		}
	}
	else if(Do2bitMode) // else if direct encoded 2bit mode - i.e. 1 mode bit per pixel
	{
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_2BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;

				// double the bits so 0=> 00, and 1=>11
				if(ModulationBits & 1)
				{
					ModulationVals[y+StartY][x+StartX] = 0x3;
				}
				else
				{
					ModulationVals[y+StartY][x+StartX] = 0x0;
				}
				ModulationBits >>= 1;
			}
		}
	}
	else // else its the 4bpp mode so each value has 2 bits
	{
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_4BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;

				ModulationVals[y+StartY][x+StartX] = ModulationBits & 3;
				ModulationBits >>= 2;
			}
		}
	}

	// make sure nothing is left over
	assert(ModulationBits==0);
}

/*!***********************************************************************
 @Function		InterpolateColours
 @Input			ColourP
 @Input			ColourQ
 @Input			ColourR
 @Input			ColourS
 @Input			Do2bitMode
 @Input			x
 @Input			y
 @Modified		Result
 @Description	This performs a HW bit accurate interpolation of either the
				A or B colours for a particular pixel.

				NOTE: It is assumed that the source colours are in ARGB 5554
				format - This means that some "preparation" of the values will
				be necessary.
*************************************************************************/
static void InterpolateColours(const int ColourP[4],
						  const int ColourQ[4],
						  const int ColourR[4],
						  const int ColourS[4],
						  const int Do2bitMode,
						  const int x,
						  const int y,
						  int Result[4])
{
	int u, v, uscale;
	int k;

	int tmp1, tmp2;

	int P[4], Q[4], R[4], S[4];

	// Copy the colours
	for(k = 0; k < 4; k++)
	{
		P[k] = ColourP[k];
		Q[k] = ColourQ[k];
		R[k] = ColourR[k];
		S[k] = ColourS[k];
	}

	// put the x and y values into the right range
	v = (y & 0x3) | ((~y & 0x2) << 1);

	if(Do2bitMode)
		u = (x & 0x7) | ((~x & 0x4) << 1);
	else
		u = (x & 0x3) | ((~x & 0x2) << 1);

	// get the u and v scale amounts
	v  = v - BLK_Y_SIZE/2;

	if(Do2bitMode)
	{
		u = u - BLK_X_2BPP/2;
		uscale = 8;
	}
	else
	{
		u = u - BLK_X_4BPP/2;
		uscale = 4;
	}

	for(k = 0; k < 4; k++)
	{
		tmp1 = P[k] * uscale + u * (Q[k] - P[k]);
		tmp2 = R[k] * uscale + u * (S[k] - R[k]);

		tmp1 = tmp1 * 4 + v * (tmp2 - tmp1);

		Result[k] = tmp1;
	}

	// Lop off the appropriate number of bits to get us to 8 bit precision
	if(Do2bitMode)
	{
		// do RGB
		for(k = 0; k < 3; k++)
		{
			Result[k] >>= 2;
		}

		Result[3] >>= 1;
	}
	else
	{
		// do RGB  (A is ok)
		for(k = 0; k < 3; k++)
		{
			Result[k] >>= 1;
		}
	}

	// sanity check
	for(k = 0; k < 4; k++)
	{
		assert(Result[k] < 256);
	}


	/*
		Convert from 5554 to 8888

		do RGB 5.3 => 8
	*/
	for(k = 0; k < 3; k++)
	{
		Result[k] += Result[k] >> 5;
	}

	Result[3] += Result[3] >> 4;

	// 2nd sanity check
	for(k = 0; k < 4; k++)
	{
		assert(Result[k] < 256);
	}

}

/*!***********************************************************************
 @Function		GetModulationValue
 @Input			x
 @Input			y
 @Input			Do2bitMode
 @Input			ModulationVals
 @Input			ModulationModes
 @Input			Mod
 @Input			DoPT
 @Description	Get the modulation value as a numerator of a fraction of 8ths
*************************************************************************/
static void GetModulationValue(int x,
							   int y,
							   const int Do2bitMode,
							   const int ModulationVals[8][16],
							   const int ModulationModes[8][16],
							   int *Mod,
							   int *DoPT)
{
	static const int RepVals0[4] = {0, 3, 5, 8};
	static const int RepVals1[4] = {0, 4, 4, 8};

	int ModVal;

	// Map X and Y into the local 2x2 block
	y = (y & 0x3) | ((~y & 0x2) << 1);

	if(Do2bitMode)
		x = (x & 0x7) | ((~x & 0x4) << 1);
	else
		x = (x & 0x3) | ((~x & 0x2) << 1);

	// assume no PT for now
	*DoPT = 0;

	// extract the modulation value. If a simple encoding
	if(ModulationModes[y][x]==0)
	{
		ModVal = RepVals0[ModulationVals[y][x]];
	}
	else if(Do2bitMode)
	{
		// if this is a stored value
		if(((x^y)&1)==0)
			ModVal = RepVals0[ModulationVals[y][x]];
		else if(ModulationModes[y][x] == 1) // else average from the neighbours if H&V interpolation..
		{
			ModVal = (RepVals0[ModulationVals[y-1][x]] +
					  RepVals0[ModulationVals[y+1][x]] +
					  RepVals0[ModulationVals[y][x-1]] +
					  RepVals0[ModulationVals[y][x+1]] + 2) / 4;
		}
		else if(ModulationModes[y][x] == 2) // else if H-Only
		{
			ModVal = (RepVals0[ModulationVals[y][x-1]] +
					  RepVals0[ModulationVals[y][x+1]] + 1) / 2;
		}
		else // else it's V-Only
		{
			ModVal = (RepVals0[ModulationVals[y-1][x]] +
					  RepVals0[ModulationVals[y+1][x]] + 1) / 2;
		}
	}
	else // else it's 4BPP and PT encoding
	{
		ModVal = RepVals1[ModulationVals[y][x]];

		*DoPT = ModulationVals[y][x] == PT_INDEX;
	}

	*Mod =ModVal;
}

/*!***********************************************************************
 @Function		TwiddleUV
 @Input			YSize	Y dimension of the texture in pixels
 @Input			XSize	X dimension of the texture in pixels
 @Input			YPos	Pixel Y position
 @Input			XPos	Pixel X position
 @Returns		The twiddled offset of the pixel
 @Description	Given the Block (or pixel) coordinates and the dimension of
				the texture in blocks (or pixels) this returns the twiddled
				offset of the block (or pixel) from the start of the map.

				NOTE the dimensions of the texture must be a power of 2
*************************************************************************/
static int DisableTwiddlingRoutine = 0;

static U32 TwiddleUV(U32 YSize, U32 XSize, U32 YPos, U32 XPos)
{
	U32 Twiddled;

	U32 MinDimension;
	U32 MaxValue;

	U32 SrcBitPos;
	U32 DstBitPos;

	int ShiftCount;

	assert(YPos < YSize);
	assert(XPos < XSize);

	assert(POWER_OF_2(YSize));
	assert(POWER_OF_2(XSize));

	if(YSize < XSize)
	{
		MinDimension = YSize;
		MaxValue	 = XPos;
	}
	else
	{
		MinDimension = XSize;
		MaxValue	 = YPos;
	}

	// Nasty hack to disable twiddling
	if(DisableTwiddlingRoutine)
		return (YPos* XSize + XPos);

	// Step through all the bits in the "minimum" dimension
	SrcBitPos = 1;
	DstBitPos = 1;
	Twiddled  = 0;
	ShiftCount = 0;

	while(SrcBitPos < MinDimension)
	{
		if(YPos & SrcBitPos)
		{
			Twiddled |= DstBitPos;
		}

		if(XPos & SrcBitPos)
		{
			Twiddled |= (DstBitPos << 1);
		}


		SrcBitPos <<= 1;
		DstBitPos <<= 2;
		ShiftCount += 1;

	}

	// prepend any unused bits
	MaxValue >>= ShiftCount;

	Twiddled |=  (MaxValue << (2*ShiftCount));

	return Twiddled;
}

/***********************************************************/
/*
// Decompress
//
// Takes the compressed input data and outputs the equivalent decompressed
// image.
*/
/***********************************************************/

static void Decompress(AMTC_BLOCK_STRUCT *pCompressedData,
					   const int Do2bitMode,
					   const int XDim,
					   const int YDim,
					   const int AssumeImageTiles,
					   unsigned char* pResultImage)
{
	int x, y;
	int i, j;

	int BlkX, BlkY;
	int BlkXp1, BlkYp1;
	int XBlockSize;
	int BlkXDim, BlkYDim;

	int StartX, StartY;

	int ModulationVals[8][16];
	int ModulationModes[8][16];

	int Mod, DoPT;

	unsigned int uPosition;

	/*
	// local neighbourhood of blocks
	*/
	AMTC_BLOCK_STRUCT *pBlocks[2][2];

	AMTC_BLOCK_STRUCT *pPrevious[2][2] = {{NULL, NULL}, {NULL, NULL}};

	/*
	// Low precision colours extracted from the blocks
	*/
	struct
	{
		int Reps[2][4];
	}Colours5554[2][2];

	/*
	// Interpolated A and B colours for the pixel
	*/
	int ASig[4], BSig[4];

	int Result[4];

	if(Do2bitMode)
	{
		XBlockSize = BLK_X_2BPP;
	}
	else
	{
		XBlockSize = BLK_X_4BPP;
	}


	/*
	// For MBX don't allow the sizes to get too small
	*/
	BlkXDim = PVRT_MAX(2, XDim / XBlockSize);
	BlkYDim = PVRT_MAX(2, YDim / BLK_Y_SIZE);

	/*
	// Step through the pixels of the image decompressing each one in turn
	//
	// Note that this is a hideously inefficient way to do this!
	*/
	for(y = 0; y < YDim; y++)
	{
		for(x = 0; x < XDim; x++)
		{
			/*
			// map this pixel to the top left neighbourhood of blocks
			*/
			BlkX = (x - XBlockSize/2);
			BlkY = (y - BLK_Y_SIZE/2);

			BlkX = LIMIT_COORD(BlkX, XDim, AssumeImageTiles);
			BlkY = LIMIT_COORD(BlkY, YDim, AssumeImageTiles);


			BlkX /= XBlockSize;
			BlkY /= BLK_Y_SIZE;

			//BlkX = LIMIT_COORD(BlkX, BlkXDim, AssumeImageTiles);
			//BlkY = LIMIT_COORD(BlkY, BlkYDim, AssumeImageTiles);


			/*
			// compute the positions of the other 3 blocks
			*/
			BlkXp1 = LIMIT_COORD(BlkX+1, BlkXDim, AssumeImageTiles);
			BlkYp1 = LIMIT_COORD(BlkY+1, BlkYDim, AssumeImageTiles);

			/*
			// Map to block memory locations
			*/
			pBlocks[0][0] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkY, BlkX);
			pBlocks[0][1] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkY, BlkXp1);
			pBlocks[1][0] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkYp1, BlkX);
			pBlocks[1][1] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkYp1, BlkXp1);


			/*
			// extract the colours and the modulation information IF the previous values
			// have changed.
			*/
			if(memcmp(pPrevious, pBlocks, 4*sizeof(void*)) != 0)
			{
				StartY = 0;
				for(i = 0; i < 2; i++)
				{
					StartX = 0;
					for(j = 0; j < 2; j++)
					{
						Unpack5554Colour(pBlocks[i][j], Colours5554[i][j].Reps);

						UnpackModulations(pBlocks[i][j],
							Do2bitMode,
							ModulationVals,
							ModulationModes,
							StartX, StartY);

						StartX += XBlockSize;
					}/*end for j*/

					StartY += BLK_Y_SIZE;
				}/*end for i*/

				/*
				// make a copy of the new pointers
				*/
				memcpy(pPrevious, pBlocks, 4*sizeof(void*));
			}/*end if the blocks have changed*/


			/*
			// decompress the pixel.  First compute the interpolated A and B signals
			*/
			InterpolateColours(Colours5554[0][0].Reps[0],
				Colours5554[0][1].Reps[0],
				Colours5554[1][0].Reps[0],
				Colours5554[1][1].Reps[0],
				Do2bitMode, x, y,
				ASig);

			InterpolateColours(Colours5554[0][0].Reps[1],
				Colours5554[0][1].Reps[1],
				Colours5554[1][0].Reps[1],
				Colours5554[1][1].Reps[1],
				Do2bitMode, x, y,
				BSig);

			GetModulationValue(x,y, Do2bitMode, (const int (*)[16])ModulationVals, (const int (*)[16])ModulationModes,
				&Mod, &DoPT);

			/*
			// compute the modulated colour
			*/
			for(i = 0; i < 4; i++)
			{
				Result[i] = ASig[i] * 8 + Mod * (BSig[i] - ASig[i]);
				Result[i] >>= 3;
			}
			if(DoPT)
			{
				Result[3] = 0;
			}

			/*
			// Store the result in the output image
			*/
			uPosition = (x+y*XDim)<<2;
			pResultImage[uPosition+0] = (unsigned char)Result[0];
			pResultImage[uPosition+1] = (unsigned char)Result[1];
			pResultImage[uPosition+2] = (unsigned char)Result[2];
			pResultImage[uPosition+3] = (unsigned char)Result[3];

		}/*end for x*/
	}/*end for y*/

}

static stbi_uc * stbi__pvr_load(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
	stbi_uc *pvr_data = NULL;
	stbi_uc *pvr_res_data = NULL;
	PVR_Texture_Header header={0};
	int iscompressed = 0;
	int bitmode = 0;
	unsigned int levelSize = 0;

	stbi__getn( s, (stbi_uc*)(&header), sizeof(PVR_Texture_Header) );

	// Check the header size
	if ( header.dwHeaderSize != sizeof(PVR_Texture_Header) ) {
		return NULL;
	}

	// Check the magic identifier
	if ( header.dwPVR != PVRTEX_IDENTIFIER ) {
		return NULL;
	}

	*x = s->img_x = header.dwWidth;
	*y = s->img_y = header.dwHeight;

	/* Get if the texture is compressed and the texture mode ( 2bpp or 4bpp ) */
	switch ( header.dwpfFlags & PVRTEX_PIXELTYPE )
	{
		case OGL_RGBA_4444:
			s->img_n = 2;
			break;
		case OGL_RGBA_5551:
			s->img_n = 2;
			break;
		case OGL_RGBA_8888:
			s->img_n = 4;
			break;
		case OGL_RGB_565:
			s->img_n = 2;
			break;
		case OGL_RGB_888:
			s->img_n = 3;
			break;
		case OGL_I_8:
			s->img_n = 1;
			break;
		case OGL_AI_88:
			s->img_n = 2;
			break;
		case OGL_PVRTC2:
			bitmode = 1;
			s->img_n = 4;
			iscompressed = 1;
			break;
		case OGL_PVRTC4:
			s->img_n = 4;
			iscompressed = 1;
			break;
		case OGL_RGB_555:
		default:
			return NULL;
	}

	*comp = s->img_n;

	// Load only the first mip map level
	levelSize = (s->img_x * s->img_y * header.dwBitCount + 7) / 8;

	// get the raw data
	pvr_data = (stbi_uc *)malloc( levelSize );
	stbi__getn( s, pvr_data, levelSize );

	// if compressed decompress as RGBA
	if ( iscompressed ) {
		pvr_res_data = (stbi_uc *)malloc( s->img_x * s->img_y * 4 );
		Decompress( (AMTC_BLOCK_STRUCT*)pvr_data, bitmode, s->img_x, s->img_y, 1, (unsigned char*)pvr_res_data );
		free( pvr_data );
	} else {
		// otherwise use the raw data
		pvr_res_data = pvr_data;
	}

	if( (req_comp <= 4) && (req_comp >= 1) ) {
		//	user has some requirements, meet them
		if( req_comp != s->img_n ) {
			pvr_res_data = stbi__convert_format( pvr_res_data, s->img_n, req_comp, s->img_x, s->img_y );
			*comp = req_comp;
		}
	}

	return pvr_res_data;
}

#ifndef STBI_NO_STDIO
stbi_uc *stbi__pvr_load_from_file   (FILE *f,                  int *x, int *y, int *comp, int req_comp)
{
	stbi__context s;
	stbi__start_file(&s,f);
	return stbi__pvr_load(&s,x,y,comp,req_comp);
}

stbi_uc *stbi__pvr_load_from_path             (char const*filename,           int *x, int *y, int *comp, int req_comp)
{
   stbi_uc *data;
   FILE *f = fopen(filename, "rb");
   if (!f) return NULL;
   data = stbi__pvr_load_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return data;
}
#endif

stbi_uc *stbi__pvr_load_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__pvr_load(&s,x,y,comp,req_comp);
}

stbi_uc *stbi__pvr_load_from_callbacks (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
	stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__pvr_load(&s,x,y,comp,req_comp);
}
