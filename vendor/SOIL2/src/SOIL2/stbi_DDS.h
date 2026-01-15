/*
	adding DDS loading support to stbi
*/

#ifndef HEADER_STB_IMAGE_DDS_AUGMENTATION
#define HEADER_STB_IMAGE_DDS_AUGMENTATION

/*	is it a DDS file? */
extern int      stbi__dds_test_memory      (stbi_uc const *buffer, int len);
extern int      stbi__dds_test_callbacks   (stbi_io_callbacks const *clbk, void *user);

extern void    *stbi__dds_load_from_path   (const char *filename,           int *x, int *y, int *comp, int req_comp);
extern void    *stbi__dds_load_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp);
extern void    *stbi__dds_load_from_callbacks (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp);

#ifndef STBI_NO_STDIO
extern int      stbi__dds_test_filename    (char const *filename);
extern int      stbi__dds_test_file        (FILE *f);
extern void    *stbi__dds_load_from_file   (FILE *f,                  int *x, int *y, int *comp, int req_comp);
#endif

extern int      stbi__dds_info_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *comp, int *iscompressed);
extern int      stbi__dds_info_from_callbacks (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int *iscompressed);


#ifndef STBI_NO_STDIO
extern int      stbi__dds_info_from_path   (char const *filename,     int *x, int *y, int *comp, int *iscompressed);
extern int      stbi__dds_info_from_file   (FILE *f,                  int *x, int *y, int *comp, int *iscompressed);
#endif

/*
//
////   end header file   /////////////////////////////////////////////////////*/
#endif /* HEADER_STB_IMAGE_DDS_AUGMENTATION */
