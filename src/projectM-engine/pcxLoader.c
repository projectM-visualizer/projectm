#include "pcxLoader.h"

#pragma pack(push,1)

/*
	The pcx Header structure is 128 bytes in size. The information in the structure is roughly
	detailed as the following (according to the pcx file format description on www.wotsit.org)


		Byte      Item          Size   Description/Comments
		 0         Manufacturer 1      Constant Flag, 10 = ZSoft .pcx
		 1         Version      1      Version information
									   0 = Version 2.5 of PC Paintbrush
									   2 = Version 2.8 w/palette information
									   3 = Version 2.8 w/o palette information
									   4 = PC Paintbrush for Windows(Plus for
										  Windows uses Ver 5)
									   5 = Version 3.0 and > of PC Paintbrush
										  and PC Paintbrush +, includes
										  Publisher's Paintbrush . Includes
										  24-bit .PCX files
		 2         Encoding      1     1 = .PCX run length encoding
		 3         BitsPerPixel  1     Number of bits to represent a pixel
										  (per Plane) - 1, 2, 4, or 8
		 4         Window        8     Image Dimensions: Xmin,Ymin,Xmax,Ymax
		12         HDpi          2     Horizontal Resolution of image in DPI*
		14         VDpi          2     Vertical Resolution of image in DPI*
		16         Colormap     48     Color palette setting, see text
		64         Reserved      1     Should be set to 0.
		65         NPlanes       1     Number of color planes
		66         BytesPerLine  2     Number of bytes to allocate for a scanline
										  plane.  MUST be an EVEN number.  Do NOT
										  calculate from Xmax-Xmin.
		68         PaletteInfo   2     How to interpret palette- 1 = Color/BW,
										  2 = Grayscale (ignored in PB IV/ IV +)
		70         HscreenSize   2     Horizontal screen size in pixels. New field
										  found only in PB IV/IV Plus
		72         VscreenSize   2     Vertical screen size in pixels. New field
										  found only in PB IV/IV Plus
		74         Filler       54     Blank to fill out 128 byte header.  Set all
										 bytes to 0


		This then starts to look somewhat like this :
*/
typedef struct
{
	unsigned char	Manufacturer;
	unsigned char	Version;
	unsigned char	Encoding;
	unsigned char	BitsPerPixel;

	struct
	{
		unsigned short Xmin;
		unsigned short Ymin;
		unsigned short Xmax;
		unsigned short Ymax;
	}
	Window;

	short 			HDpi;
	short 			VDpi;
	unsigned char	ColorMap[48];
	unsigned char	Reserved1;
	unsigned char	NPlanes;
	short 			BytesPerLine;
	short 			PaletteInfo;
	short 			HscreenSize;
	short 			VscreenSize;
	unsigned char	Filler[54];
}
PcxHeader;

/*!
 *	\brief	This structure is intended to hold a single byte of information from the pcx file. This is kinda important
 *			because the bytes hold information about whether a run or single byte is used. This is basically a bit
 *			annoying so instead it is easier to create something using bit-fields that can quickly identify the bit
 *			flags that are set rather than using horrible bit-wise operations. I just kinda prefer it cos it's more
 *			readable....
 */
typedef struct
{
	union
	{
		struct
		{
			unsigned char Count : 6;
			/* if these two bits are 1, then we have a start of a byte run */
			unsigned char Flag1 : 1;
			unsigned char Flag2 : 1;
		};
		unsigned char byte;
	};
}
PcxByte;

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}
PcxColour;

#pragma pack(pop)


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



/*!
 *	\brief	This function loads a ZSoft PCX File image file.
 *	\param	filename	-	the name of the file to load
 *	\param	pixels		-	pointer to the pointer that will hold the pixel data (yuck)
 *	\param	w			-	the width of the image file
 *	\param	h			-	the height of the image file
 *	\param	bpp			-	the bytes per pixel of the image file.
 *	\return	1 if ok, 0 otherwise
 */
int LoadPcxImage( const char filename[],
				  unsigned char** pixels,
				  unsigned int* w,
				  unsigned int* h,
				  unsigned int* bpp)
{
	/* open the file */
	FILE* fp = fopen( filename,"rb" );

	/* if opened OK */
	if(fp)
	{
		/* holds the 128 byte header */
		PcxHeader header;

		/* read the header in */
		fread(&header,128,1,fp);

		if( header.Manufacturer == 10 )
		{
			/* calculate the image width & height */
			unsigned int XSIZE 			= header.Window.Xmax   -  header.Window.Xmin  +  1;
			unsigned int YSIZE 			= header.Window.Ymax   -  header.Window.Ymin  +  1;

			/* The number of bytes per line */
			unsigned int BytesPerLine	= header.BytesPerLine  *  header.NPlanes;

			/* simple count vars */
			unsigned int i,j,k;

			/* a temporary buffer for a scanline */
			unsigned char *tempbuffer;

			/* another temp var */
			unsigned int iCount;



			/* check pcx version */
			if( header.Version != 0x05 )
			{
				printf("[ERROR] Only version 5 of the pcx file format is supported....\n");
				fclose(fp);
				return 0;
			}

			/* allocate enough space for the pixel data */
			tempbuffer = (unsigned char*) malloc ( BytesPerLine );
			assert( tempbuffer );


			/* allocate enough space for the pixel data */
			(*pixels) = (unsigned char*) malloc ( header.NPlanes * XSIZE * YSIZE );
			assert( *pixels );

			/* store info */
			*w    =  XSIZE;
			*h    =  YSIZE;
			*bpp  =  header.NPlanes;

			/* Loop through until whole image is loaded.... */
			for (i = 0; i < YSIZE; ++i )
			{
				j = 0;

				/*
				 *	because the scanlines are orderer RRRGGGBBB rather than RGBRGBRGB, I'm reading and decompressing
				 *	the file one scanline at a time.
				 */
				while( j < BytesPerLine )
				{
					PcxByte byte;

					/* assume that at the very least we have a count of 1 */
					iCount=1;

					/* read a byte */
					fread( &byte, 1, 1, fp );

					/*
					 *	If the top 2 bits of the byte are 1, then the lower 6 bits represent a data count.
					 *	The following byte will then be repeated 'count' number of times.
					 */
					if( byte.Flag1 && byte.Flag2 )
					{
						/* see how many byte repeats we have */
						iCount = (unsigned int)byte.Count;

						/* read the data byte */
						fread( &byte, 1, 1, fp );
					}

					/* expand the compressed data... */
					for ( k = 0; k < iCount; ++k )
					{
						tempbuffer[j++] = byte.byte;
					}
				}

				/* having read the scanline into the scanline buffer, now re-organise into RGB data in the pixel buffer */
				for( j=0; j < XSIZE; ++j )
				{
					unsigned char *scanline = ((*pixels) + (XSIZE * header.NPlanes * (YSIZE-i-1)));

					for( k =0; k < header.NPlanes; ++k )
					{
						scanline[  (j * header.NPlanes) +  k ] = tempbuffer[ (header.BytesPerLine * k) + j ];
					}
				}
			}

			/* make sure we get rid of the temporary buffer memory allocation.... */
			free( tempbuffer );

			/* if the bytes per pixel == 1, then there will be a colourmap at the end (hopefully) */
			if( header.NPlanes == 1 )
			{
				unsigned char testchar=0;

				/* move to 769 bytes from the end... */
				fseek( fp, -769, SEEK_END );

				/* read the next char */
				testchar=fgetc(fp);

				/* hopefully it will be 12, or 0x0A which indicates the presence of a colour map */
				if( testchar == 0x0C )
				{
					/* allocate enough memory for the colour map */
					PcxColour* Colourmap = (PcxColour*) malloc ( 768 );
					assert(Colourmap);

					/* read the colour map data */
					fread( Colourmap, sizeof(PcxColour), 256, fp );

					/* see notes below/... */
					testchar = 0x01;

					/*
					 *	this is a really crap check for greyscale images. As far as I can tell, a pcx file stores
					 *	greyscale images by storing the grey tones in the colour palette. I can't really see the point
					 *	of expanding this to 24bit because it will then work differently to the other image loaders.
					 *	as a result I check to see if all colours read in actually match their index in which case
					 *	I will just ignore the colour pallette info....
					 */
					for(i=0;i<256;++i)
					{
						if( Colourmap[i].r != i ||
						    Colourmap[i].g != i ||
						    Colourmap[i].b != i )
						{
							testchar = 0;
							break;
						}
					}

					/* if the test proved false, then I will expand the data out into a 24bit colour image */
					if( testchar == 0x00 )
					{
						unsigned char* px_o= (*pixels);
						unsigned char* end_o = px_o + (XSIZE*YSIZE);
						unsigned char* px;

						tempbuffer = (unsigned char*) malloc( XSIZE*YSIZE*3 );
						assert(tempbuffer);

						px = tempbuffer;

						/* Expand the old 8bit colourmapped data into 24bit... */
						while( px_o != end_o )
						{
							*px = Colourmap[ *px_o ].r;
							++px;

							*px = Colourmap[ *px_o ].g;
							++px;

							*px = Colourmap[ *px_o ].b;
							++px;

							++px_o;
						}

						/* delete the not needed 8bit data */
						free( *pixels );

						/* store pointer to new pixel data */
						*pixels = tempbuffer;

						/* update the bytes per pixel... */
						*bpp  =  3;
					}

					free( Colourmap );
				}
			}

		}
		else
		{
			printf("[ERROR] %s is not a pcx file!!\n" , filename );
			fclose(fp);
			return 0;
		}

		fclose(fp);

		return 1;
	}
	return 0;
}















