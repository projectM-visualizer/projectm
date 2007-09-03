#include "bmpLoader.h"

#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>


/* force 2 byte alignment of structures */
#pragma pack(push,2)

/*!
 *	This structure comes first in the bitmap file. It's a pretty boring structure really....
 */
typedef struct
{
	/*!
	 *	The characters identifying the bitmap. The following entries are possible:
	 *
	 *	'BM' - Windows 3.1x, 95, NT, .
	 *	'BA' - OS/2 Bitmap Array
	 *	'CI' - OS/2 Color Icon
	 *	'CP' - OS/2 Color Pointer
	 *	'IC' - OS/2 Icon
	 *	'PT' - OS/2 Pointer
	 */
	unsigned char	Identifier[2];

	/*!
	 *	Complete file size in bytes
	 */
	unsigned long	FileSize;

	/*!
	 *	Reserved for later use
	 */
	unsigned long	Reserved;

	/*!
	 *	Offset from beginning of file to the beginning of the bitmap data
	 */
	unsigned long	BitmapDataOffset;
}
BitmapFileHeader;


/*!
 *	This structure immediately follows the main file header. This basically describes
 *	the information contained within
 */
typedef struct
{
	/*!
	 *	Length of the Bitmap Info Header used to describe the bitmap colors, compression,
	 *	The following sizes are possible:
	 *
	 *	28h - Windows 3.1x, 95, NT, .
	 *	0Ch - OS/2 1.x
	 *	F0h - OS/2 2.x
	 */
	unsigned long	BitmapHeaderSize;

	/*!
	 *	Horizontal width of bitmap in pixels.
	 */
	long			Width;

	/*!
	 *	Vertical height of bitmap in pixels.
	 */
	long			Height;

	/*!
	 *	Number of planes in this bitmap.
	 */
	unsigned short	Planes;

	/*!
	 *	Bits per pixel used to store palette entry information. This also identifies
	 *	in an indirect way the number of possible colors. Possible values are:
	 *
	 *	1 - Monochrome bitmap
	 *	4 - 16 color bitmap
	 *	8 - 256 color bitmap
	 *	16 - 16bit (high color) bitmap
	 *	24 - 24bit (true color) bitmap
	 *	32 - 32bit (true color) bitmap
	 */
	unsigned short 	BitsPerPixel;

	/*!
	 *	Compression specifications. The following values are possible:
	 *
	 *	0 - none (Also identified by BI_RGB)
	 *	1 - RLE 8-bit / pixel (Also identified by BI_RLE4)
	 *	2 - RLE 4-bit / pixel (Also identified by BI_RLE8)
	 *	3 - Bitfields  (Also identified by BI_BITFIELDS)
	 */
	unsigned long  	Compression;

	/*!
	 *	Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
	 */
	unsigned long  	BitmapDataSize;

	/*!
	 *	Horizontal resolution expressed in pixel per meter.
	 */
	unsigned long   HResolution;

	/*!
	 *	Vertical resolution expressed in pixels per meter.
	 */
	unsigned long   VResolution;

	/*!
	 *	Number of colors used by this bitmap. For a 8-bit / pixel
	 *	bitmap this will be 100h or 256.
	 */
	unsigned long   ColorsUsed;

	/*!
	 *	Number of important colors. This number will be equal to the
	 *	number of colors when every color is important.
	 */
	unsigned long   ImportantColors;
}
BitmapInfoHeader;


/*!
 *	This structure is used to hold a colour map entry.
 */
typedef struct
{
	/*!	blue component	*/
	unsigned char b;

	/*!	green component */
	unsigned char g;

	/*!	red component */
	unsigned char r;

	/*!	some say alpha some say reserved.... */
	unsigned char a;
}
ColorMapEntry;

#pragma pack(pop)



/*!
 *	\brief	This function loads a bitmap image
 *	\param	filename	-
 */
int LoadBmpImage(const char            filename[],
				 unsigned char** pixels,
				 unsigned int*   w,
				 unsigned int*   h,
				 unsigned int*   bpp)
{
	BitmapFileHeader	FileHeader;
	BitmapInfoHeader 	ImageHeader;

	FILE *fp = fopen( filename, "rb" );

	/* ensure file is a happy file... */
	if( fp == NULL )
	{
		return 0;
	}

	/* read the bitmap file */
	fread( &FileHeader, sizeof(BitmapFileHeader), 1, fp );

	/*
	 *	Make sure we have a valid bitmap file
	 */
	if( FileHeader.Identifier[0] != 'B' ||
		FileHeader.Identifier[1] != 'M' )
	{
		return 0;
	}

	/*
	 *	Read the image header.
	 */
	fread( &ImageHeader, sizeof(BitmapInfoHeader), 1, fp );

	if( ImageHeader.Compression != 0 )
	{
		printf("[ERROR] This bitmap loader does not support compressed files (yet)\n");
		return 0;
	}

	/* store some useful info */
	*w   = ImageHeader.Width;
	*h   = ImageHeader.Height;


	/*----------------------------------------------------------------------------	8 Bit Colour mapped images	*/

	/*
	 *	It appears as though all 8bit bmp images will have a colour map. This is
	 *	a tad bit annoying as a grey scale image would have also been nice!
	 */
	if( ImageHeader.BitsPerPixel == 8 )
	{
		/* pointer for colour map data */
		ColorMapEntry* map;

		/* hopefully if we have a colour map it will be a sensible 8bit one... */
		if( ImageHeader.BitsPerPixel != 8 )
		{
			fprintf(stderr,"[ERROR] The bitmap file you are trying to load has a really odd colour map definition!!\n");
		}

		/* allocate space for colour map */
		map = (ColorMapEntry*) malloc( sizeof(ColorMapEntry) * 256 );
		assert( map );

		/* read colour map */
		fread( map, sizeof(ColorMapEntry), 256, fp );
		
		{
			unsigned int i;
			for(i=0;i<256;++i)
			{
				/* if not a grey colour ignore greyscale reading */
				if( i != map[i].r &&
					i != map[i].g &&
					i != map[i].b ) {
					goto hacky_hacky_moo;
				}
			}		
			
			/* ignore colour map */
			*bpp = 1;
			free( map );

			/* allocate memory for the pixel data */
			*pixels = (unsigned char*) malloc( (*w) * (*h) );
			assert(*pixels);
			fread( *pixels, (*w) * (*h), 1, fp );
			return 1;
		}
		
		hacky_hacky_moo: ;

		/*
		 *	Bitmap files are rather irritatingly aligned to 4 bytes on a line....
		 */
		{
			unsigned int i;
			unsigned int RowSize = ImageHeader.Width;
			unsigned char *scanline;

			if( RowSize%4 != 0 )
			{
				RowSize += (4 - RowSize%4);
			}

			/* allocate enough memory for a scanline */
			scanline = (unsigned char*) malloc( RowSize );
			assert(scanline);

			/* we will expand the colour map to 24bit */
			*bpp = 3;

			/* allocate memory for the pixel data */
			*pixels = (unsigned char*) malloc( (*w) * (*h) * (*bpp) );
			assert(*pixels);

			/* loop through each scanline */
			for(i=0;i<(*h); ++i)
			{
				/* get a pointer to the start of the scanline in expanded data */
				unsigned char* pline = ( (*pixels) + ( 3 * (*w) * i ) );
				unsigned int j;

				/* read the scanline including padding */
				fread( scanline, RowSize, 1, fp );

				/* go through each pixel in line */
				for( j=0; j<(*w); ++j )
				{
					/* copy red from colour map to pixel data array */
					(*pline) = map[ scanline[j] ].r;
					++pline;

					/* copy green from colour map to pixel data array */
					(*pline) = map[ scanline[j] ].g;
					++pline;

					/* copy blue from colour map to pixel data array */
					(*pline) = map[ scanline[j] ].b;
					++pline;
				}
			}

			/* delete the dynamic data */
			free( scanline );
			free( map );
		}
		fclose(fp);
		return 1;
	}


	/*----------------------------------------------------------------------------	24/32 Bit uncompressed images	*/

	{
		unsigned int bdepth;
		unsigned int i;
		unsigned int RowSize;
		unsigned int padding = 0;

		/* determine the bitdepth of the actual bitmap image */
		switch( ImageHeader.BitsPerPixel )
		{
		case 24:
			bdepth=3;
			break;

		case 32:
			bdepth=4;
			break;

		default:
			printf("[ERROR] bitmap loader does not support this bit depth\n");
			fclose(fp);
			return 0;
		}
		*bpp = bdepth;

		/* we will need this value a fair amount, so pre-calculate it... */
		RowSize = bdepth * ImageHeader.Width;

		/* determine the padding at the end of the scanline */
		if( (RowSize % 4) != 0 )
		{
			padding = 4 - (RowSize % 4);
		}



		/* allocate memory for the pixel data */
		*pixels = (unsigned char*) malloc( (*h) * RowSize );
		assert(*pixels);

		for( i=0; i != *h; ++i )
		{
			unsigned char* end;
			/* get a pointer to the start of the scanline in expanded data */
			unsigned char* pline = ( (*pixels) + ( RowSize * i ) );
			unsigned int j;

			/* read the scanline including padding */
			fread( pline, RowSize, 1, fp );

			end   = pline + RowSize;

			/* flip scanline from BGR to RGB */
			for( ; pline != end ; pline += bdepth )
			{
				unsigned char temp  = *pline;
				pline[0] = pline[2];
				pline[2] = temp;
			}

			/* now read past padding */
			for( j=0; j!= padding; ++j )
			{
				fgetc(fp);
			}
		}
	}

	fclose(fp);

	return 1;
}
