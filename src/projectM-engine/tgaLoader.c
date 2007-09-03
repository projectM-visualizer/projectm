

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <memory.h>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif



/*
 *	most compilers tend to align structure data size so that its size is 4byte
 *	aligned (ie, divisable by 4). This happens due to compiler optimisations that we have
 *	to manually prevent. This can be done with the #pragma pack() directive. Without it,
 *	the struct would probably end up looking like :
 *
 *		typedef struct
 *		{
 *			// bytes 0 to 3
 *			unsigned char  m_ImageIdent;
 *			unsigned char  m_ColourMapType;
 *			unsigned char  m_ImageType;
 *
 *			unsigned char  PADDING1;
 *
 *			// bytes 4 to 7
 *			unsigned short m_ColourMapOrigin;
 *			unsigned short m_ColourMapSize;
 *
 *			// bytes 8 to 11
 *			unsigned char  m_ColourMapESize;
 *
 *			unsigned char  PADDING2;
 *
 *			unsigned short m_Xorigin;
 *
 *			// bytes 12 to 15
 *			unsigned short m_Yorigin;
 *			unsigned short m_Width;
 *
 *			// bytes 16 to 19
 *			unsigned short m_Height;
 *			unsigned char  m_PixelIndexSize;
 *			unsigned char  m_ImageDescByte;
 *		}
 *		tga_header;
 *
 *	This struct is 20 bytes in size, which is going to cause us some big problems when
 *	trying to read an 18 byte chunk from the file. We will end up with rubbish data.
 *	This is why we tell compilers to align data structures to one byte.
 */
#pragma pack(push,1)

/*
*	This structure is used to hold the header of the tga image file format
*/
typedef struct
{
	unsigned char  m_ImageIdent;		/* size of image identification field */
	unsigned char  m_ColourMapType;		/* 0 - No colour map, 1 - has colour map */
	unsigned char  m_ImageType;			/* 0 - No Image, 1 - 8bit, 2 - uncompress RGB ..... */
	unsigned short m_ColourMapOrigin;	/* Index of first colour map entry */
	unsigned short m_ColourMapSize;		/* Number of colour map entries */
	unsigned char  m_ColourMapESize;	/* Number of bits for each entry (24) */
	unsigned short m_Xorigin;			/* x origin of image */
	unsigned short m_Yorigin;			/* y origin of image */
	unsigned short m_Width;				/* width of image in pixels */
	unsigned short m_Height;			/* height of image in pixels */
	unsigned char  m_PixelIndexSize;	/* number of bits stored in each pixel index */
	unsigned char  m_ImageDescByte;		/* should always be zero.... */
}
tga_header;


/*
 *	This structure is used purely for reading the RLE files.
 */
typedef struct
{
	unsigned char Count : 7;
	/* if these two bits are 1, then we have a start of a byte run */
	unsigned char Flag  : 1;
}
TgaByte;

/*
*	reset structure alignment back to the default
*/
#pragma pack(pop)


/*
 *	This union is a total of 2 bytes in size. s and b both share the
 *	same memory so the 2byte variable can be easily split into its constituant bytes.
 *	By assigning 1 to the 2byte variable, we can expect the array b to hold
 *	b[] = { 1, 0 } on a PC (LSB) and b[] = { 0, 1 } on an o2 (MSB). It also makes
 *	it nice and easy to swap the variables bytes.
 */
union charToShort
{
	/* 1 x 2 byte variable */
	unsigned short s;
	/* 2 x 1 byte variables */
	unsigned char  b[2];
};



/*
 *	This function is used to determine if the system has LSB or MSB byte ordering.
 *	By using a union, both the 2byte variable(short) and the array of 2 individual
 *	bytes (chars) will be stored in the same memory location. ie, the charToShort
 *	union data type is only two bytes in size and allows you to split apart a 2byte
 *	variable to check the ordering.
 *
 *	The two byte variables in the tga header will have to have their byte orderings
 *	switched on irix and mac, hence the use of this (sneaky) function to see if we need
 *	to swap the data bytes.
 */
int CheckByteOrder()
{
	/* Create a 2byte check variable */
	union charToShort cs;

	/* assign 1 to the 2byte variable */
	cs.s = 1;

	/* check the first byte */
	if(cs.b[0] == 0)
		return 1;

	return 0;
}



/*
 *	This function switches the byte orderings of two byte variables. It accepts a
 *	pointer to the memory location you wish to swap.
 */
void Swap2Bytes(IN void *ptr)
{
	/* assign a pointer to the 2byte variable */
	unsigned char *pcs = (unsigned char*)ptr;

	/* swap the bytes around */
	unsigned char temp = pcs[0];
	pcs[0] = pcs[1];
	pcs[1] = temp;
}

/*
 *	\brief	This function flips pixel colours from RGB (or RGBA) to BGR/BGRA
 *	\param	data	-	the pixel data to flip
 *	\param	num_px	-	the number of pixels to flip
 *	\param	bpp		-	the bytes per pixel, ie 1,3,4
 */
void BGR_TO_RGB(unsigned char* data,unsigned int num_px,unsigned int bpp)
{
	/* get pointer to data end */
	unsigned char *end = data + (bpp * num_px);

	/* loop till end of data, move on by number of bytes in pixel each time */
	for( ; data != end ; data += bpp )
	{
		unsigned char temp = *data;
		*data = data[2];
		data[2] = temp;
	}
}

/*
 *	\brief	This function flips an image upside down...
 *	\param	data	-	the pixel data to flip
 *	\param	rowsize	-	the data size of each scanline
 *	\param	height 	-	the number of scanlines in the image
 */
void FlipDaImage( unsigned char* data, unsigned int rowsize,unsigned int height)
{
	unsigned int mid = height/2,i,j;
	unsigned char *temprow = (unsigned char*) malloc ( rowsize );
	assert( temprow );

	for( i=0, j=(height-1); i<mid; ++i,--j )
	{
		/* copy low row to temp */
		memcpy( temprow           , &data[ i*rowsize ], rowsize );

		/* copy high row to low row */
		memcpy( &data[ i*rowsize ], &data[ j*rowsize ], rowsize );

		/* copy temp to high row */
		memcpy( &data[ j*rowsize ], temprow           , rowsize );
	}

	/* delete allocated data */
	free( temprow );
}

/*
 *	A function to load a tga file and return the OpenGL texture object
 */
int LoadTgaImage(const char filename[],unsigned char** ppData,unsigned int* w,unsigned int* h,unsigned int* bpp)
{

	/*
	 *	Create an instance of the struct to hold the tga file header
	 */
	tga_header header;


	/*
	 *	Create a file pointer called fp and attempt to open the file
	 */
	FILE *fp = fopen(filename,"rb");

	/*
	 *	if the file pointer is equal to NULL, the file failed to open
	 */
	if(fp)
	{
		/*
		 *	A pointer to hold the memory location of allocated memory that
		 *	we will require in order to store the pixel data
		 */
		unsigned int i,j,k;

		/*
		 *	Read the file header
		 */
		fread(&header,1,sizeof(tga_header),fp);

		/*
		 *	Check the byte ordering of the computer to see if we have to swap any bytes .
		 *	It's that pesky MSB/LSB thing again....
		 */
		if(CheckByteOrder())
		{
			/*
			 *	Swap the 2byte variables from the tga file header so that they make
			 *	sense on a unix box...
			 */
			Swap2Bytes( (void*)&header.m_ColourMapOrigin );
			Swap2Bytes( (void*)&header.m_ColourMapSize   );
			Swap2Bytes( (void*)&header.m_Xorigin );
			Swap2Bytes( (void*)&header.m_Yorigin );
			Swap2Bytes( (void*)&header.m_Width  );
			Swap2Bytes( (void*)&header.m_Height );
		}

		*w = header.m_Width;
		*h = header.m_Height;

		/* skip the image identification field if present */
		for( i=0; i!=header.m_ImageIdent; ++i )
		{
			fgetc(fp);
		}

		/* store bytes per pixel */
		*bpp = header.m_PixelIndexSize/8;

		/*
		 *	Determine the size (in bytes) of the pixel data size contained
		 *	in the file. width * height * 3 bytes per colour (RGB).
		 *
		 *	Use malloc to allocate the required memory size
		 */
		*ppData = (unsigned char*)malloc( (*bpp) * header.m_Width * header.m_Height );

		/*
		 *	Use assert to ensure that the allocation succeeded
		 */
		assert( *ppData != NULL);


		/* an 8bit colour mapped image */
		if( header.m_ImageType  == 1 )
		{
			/* This pointer holds reference to the actual 8bit data */
			unsigned char* tempptr = *ppData,*pd=0,*pData=0;

			/* number of bytes per row */
			unsigned int rowsize = (header.m_ColourMapESize/8) * header.m_Width;

			/* allocate and then read in the colour map.... */
			unsigned char* colourMap = (unsigned char*) malloc ( header.m_ColourMapSize * (header.m_ColourMapESize/8) );

			/* check allocation */
			assert( colourMap );

			/* store the new bytes per pixel (as given by the colour map entry size) */
			*bpp = ( header.m_ColourMapESize / 8 );

			/* read colour map data */
			fread( colourMap, header.m_ColourMapSize *(*bpp), 1, fp );

			/* flip colour map from BGR to RGB */
			BGR_TO_RGB( colourMap, header.m_ColourMapSize, *bpp );

			/* read the data in one go, we'll flip it upside down when expanding the colour map data.... */
			fread( tempptr, 1, header.m_Width * header.m_Height, fp );

			/* allocate memory for the new expanded image data... */
			*ppData = (unsigned char*) malloc( header.m_Height * rowsize );
			assert( *ppData );

			/* this pointer is used as an iterator  */
			pData = (*ppData);


			/* now going to run backwards through 8bit image and write it upside down when expanding the colour map */
			for( i=0; i != header.m_Height; ++i )
			{
				/* get a pointer to the data row */
				pd = tempptr + ( (header.m_Height-i-1) * header.m_Width );

				/* loop till the end of the row */
				for( j=0; j != rowsize; ++pd )
				{
					/* get pointer to actual colour in colour map */
					unsigned char *col =  &colourMap[ 3*(*pd) ];

					/* get end of colour */
					unsigned char *colend = col + *bpp;

					/* copy the colour over (3 or 4bytes probably...) */
					for( ; col != colend; ++col, ++j, ++pData )
					{
						/* copy colour from colour map into correct place in expanded image */
						*pData = *col;
					}
				}
			}

			/* free up any data... */
			free( tempptr );
			free( colourMap );


		}
		else

		/* uncompressed grey/RGB/RGBA data */
		if( header.m_ImageType  == 2 )
		{
			/* number of bytes per row */
			unsigned int rowsize = (*bpp) * header.m_Width;

			/* pointer to last row in image data */
			unsigned char* pd = *ppData + (*bpp) * (header.m_Height-1) * header.m_Width;

			/*
			 *	read the pixel data from the file in rows cos tga files are upside down...
			 */
			for( i = 0; i < header.m_Height; ++i, pd -= rowsize )
			{
				fread( pd, 1, rowsize, fp );
			}

			/* don't bother flipping bytes if greyscale image */
			if( *bpp != 1 )
			{
				/*
				 *	the tga file stores pixel data as bgr values. OpenGL however requires
				 *	the data in RGB. Therefore we have to traverse the array and swap the
				 *	pixel data around abit.
				 */
				BGR_TO_RGB( (*ppData), header.m_Height * header.m_Width, *bpp );
			}

			/* turn the image upside down .... */
			FlipDaImage( *ppData, rowsize, header.m_Height );
		}
		else

		/* RLE colour mapped images */
		if( header.m_ImageType  == 9 )
		{
			/* This pointer holds reference to the actual 8bit data */
			unsigned char* tempptr = *ppData;

			/* number of bytes per row */
			unsigned int rowsize = (header.m_ColourMapESize/8) * header.m_Width;

			/* allocate and then read in the colour map.... */
			unsigned char* colourMap = (unsigned char*) malloc ( header.m_ColourMapSize * (header.m_ColourMapESize/8) );

			/* temporary iterator pointers */
			unsigned char *pde,*pData;

			/* check allocation */
			assert( colourMap );

			/* store the new bytes per pixel (as given by the colour map entry size) */
			*bpp = ( header.m_ColourMapESize / 8 );

			/* read colour map data */
			fread( colourMap, header.m_ColourMapSize * (*bpp), 1, fp );

			/* flip colour map from BGR to RGB */
			BGR_TO_RGB( colourMap, header.m_ColourMapSize, *bpp );

			/* allocate memory for the uncompressed image... */
			*ppData = (unsigned char*) malloc ( header.m_Height * header.m_Width * (*bpp)  );

			/* get ptr to first element */
			pData = *ppData;

			/* get end pointer marker */
			pde = pData + header.m_Height * header.m_Width * (*bpp);

			/* now the reading process begins!! */
			while( pde != pData )
			{
				unsigned char byte;

				fread( &byte, 1, 1, fp );

				/*
				 *	if the flag is zero, then the count indicates a run of raw data
				 */
				if( (byte & 0x80) == 0 )
				{
					byte++;

					for(j=0;j<byte; ++j)
					{
						unsigned char c = fgetc(fp);
						for( k = 0; k != *bpp; ++k, ++pData )
						{
							*pData = colourMap[ (*bpp)*c + k ];
						}
					}
				}

				/*
				 *	If the flag is one, then the count represents the amount of times that the next byte
				 *	is repeated. This is the RLE bit....
				 */
				else
				{
					unsigned char c = fgetc(fp);
					byte -= 0x7F;

					for(j=0;j<byte; ++j)
					{
						for( k = 0; k != *bpp; ++k, ++pData )
						{
							*pData = colourMap[ (*bpp)*c + k ];
						}
					}

				}
			}

			/* turn the image upside down .... */
			FlipDaImage( *ppData, rowsize, header.m_Height );
		}
		else

		/* RLE compressed grey/RGB/RGBA images */
		if( header.m_ImageType  == 10 )
		{
			/* This pointer holds reference to the actual 8bit data */
			unsigned char* tempptr = *ppData;

			/* number of bytes per row */
			unsigned int rowsize = (*bpp) * header.m_Width;

			/* temporary iterator pointers */
			unsigned char* pde,*pData;

			/* get ptr to first element */
			pData = *ppData;

			/* get end pointer marker */
			pde = pData + header.m_Height * header.m_Width * (*bpp);

			/* now the reading process begins!! */
			while( pData < pde )
			{
				unsigned char colour[10] = {0,0,0,0};
				unsigned char byte;

				fread( &byte, 1, 1, fp );

				/*
				 *	if the flag is zero, then the count indicates a run of raw data
				 */
				if( (byte & 0x80) == 0 )
				{
					byte++;

					for(j=0;j<byte; ++j)
					{
						fread( colour, 1, *bpp, fp );

						for( k = 0; k != *bpp; ++k, ++pData )
						{
							*pData = colour[ k ];
						}
					}
				}

				/*
				 *	If the flag is one, then the count represents the amount of times that the next byte
				 *	is repeated. This is the RLE bit....
				 */
				else
				{
					byte -= 0x7F;

					fread( colour, 1, *bpp, fp );

					for(j=0;j<byte; ++j)
					{
						for( k = 0; k != *bpp; ++k, ++pData )
						{
							*pData = colour[ k ];
						}
					}

				}
			}

			/* flip colour map from BGR to RGB */
			BGR_TO_RGB( *ppData, header.m_Width*header.m_Height, *bpp );

			/* turn the image upside down .... */
			FlipDaImage( *ppData, rowsize, header.m_Height );
		}

		/* unsupported image type... */
		else
		{
			switch( header.m_ImageType )
			{
			case 0:
				printf("[ERROR] The file %s contains no image data\n",filename);
				break;
			case 3:
				printf("[ERROR] The file %s contains black & white data which is unsupported\n",filename);
				break;
			case 11:
				printf("[ERROR] The file %s contains compressed black & white data which is unsupported\n",filename);
				break;
			case 32:
				printf("[ERROR] The file %s contains compressed color map data which is un-supported\n",filename);
				break;
			case 33:
				printf("[ERROR] The file %s contains compressed color map data which is un-supported (4pass quad tree process)\n",filename);
				break;
			default:
				printf("[ERROR] The file %s does not appear to be valid\n",filename);
				break;

			}
		}

		/*
		 *	close the file
		 */
        fclose(fp);

        return 1;
	}
	return 0;
}
