#ifndef PKM_HELPER_H
#define PKM_HELPER_H

typedef struct {
	char aName[6];
	unsigned short iBlank;
	unsigned char iPaddedWidthMSB;
	unsigned char iPaddedWidthLSB;
	unsigned char iPaddedHeightMSB;
	unsigned char iPaddedHeightLSB;
	unsigned char iWidthMSB;
	unsigned char iWidthLSB;
	unsigned char iHeightMSB;
	unsigned char iHeightLSB;
} PKMHeader;

#define PKM_HEADER_SIZE 16

#endif
