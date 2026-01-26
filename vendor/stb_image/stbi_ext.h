#ifndef HEADER_STB_IMAGE_EXT
#define HEADER_STB_IMAGE_EXT

enum {
	STBI_unknown= 0,
	STBI_jpeg	= 1,
	STBI_png	= 2,
	STBI_bmp	= 3,
	STBI_gif	= 4,
	STBI_tga	= 5,
	STBI_psd	= 6,
	STBI_pic	= 7,
	STBI_pnm	= 8,
	STBI_dds	= 9,
	STBI_pvr	= 10,
	STBI_pkm	= 11,
	STBI_hdr	= 12,
	STBI_qoi	= 13
};

extern int      stbi_test_from_memory      (stbi_uc const *buffer, int len);
extern int      stbi_test_from_callbacks   (stbi_io_callbacks const *clbk, void *user);

#ifndef STBI_NO_STDIO
extern int      stbi_test					(char const *filename);
extern int      stbi_test_from_file        (FILE *f);
#endif

#endif /* HEADER_STB_IMAGE_EXT */
