#include "pkm_helper.h"
#include "wfETC.h"

static int stbi__pkm_test(stbi__context *s)
{
	//	check the magic number
	if (stbi__get8(s) != 'P') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != 'K') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != 'M') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != ' ') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != '1') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != '0') {
		stbi__rewind(s);
		return 0;
	}

	stbi__rewind(s);
	return 1;
}

#ifndef STBI_NO_STDIO

int      stbi__pkm_test_filename        		(char const *filename)
{
   int r;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   r = stbi__pkm_test_file(f);
   fclose(f);
   return r;
}

int      stbi__pkm_test_file        (FILE *f)
{
   stbi__context s;
   int r,n = ftell(f);
   stbi__start_file(&s,f);
   r = stbi__pkm_test(&s);
   fseek(f,n,SEEK_SET);
   return r;
}
#endif

int      stbi__pkm_test_memory      (stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__pkm_test(&s);
}

int      stbi__pkm_test_callbacks      (stbi_io_callbacks const *clbk, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__pkm_test(&s);
}

static int stbi__pkm_info(stbi__context *s, int *x, int *y, int *comp )
{
	PKMHeader header;
	unsigned int width, height;

	stbi__getn( s, (stbi_uc*)(&header), sizeof(PKMHeader) );

	if ( 0 != strncmp( header.aName, "PKM 10", sizeof(header.aName) ) ) {
		stbi__rewind(s);
		return 0;
	}

	width = (header.iWidthMSB << 8) | header.iWidthLSB;
	height = (header.iHeightMSB << 8) | header.iHeightLSB;

	*x = s->img_x = width;
	*y = s->img_y = height;
	*comp = s->img_n = 3;

	stbi__rewind(s);

	return 1;
}

int stbi__pkm_info_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *comp )
{
	stbi__context s;
	stbi__start_mem(&s,buffer, len);
	return stbi__pkm_info( &s, x, y, comp );
}

int stbi__pkm_info_from_callbacks (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp)
{
	stbi__context s;
	stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
	return stbi__pkm_info( &s, x, y, comp );
}

#ifndef STBI_NO_STDIO
int stbi__pkm_info_from_path(char const *filename,     int *x, int *y, int *comp)
{
   int res;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   res = stbi__pkm_info_from_file( f, x, y, comp );
   fclose(f);
   return res;
}

int stbi__pkm_info_from_file(FILE *f,                  int *x, int *y, int *comp)
{
   stbi__context s;
   int res;
   long n = ftell(f);
   stbi__start_file(&s, f);
   res = stbi__pkm_info(&s, x, y, comp);
   fseek(f, n, SEEK_SET);
   return res;
}
#endif

static void * stbi__pkm_load(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
	stbi_uc *pkm_data = NULL;
	stbi_uc *pkm_res_data = NULL;
	PKMHeader header;
	unsigned int width;
	unsigned int height;
	unsigned int compressed_size;

	stbi__getn( s, (stbi_uc*)(&header), sizeof(PKMHeader) );

	if ( 0 != strncmp( header.aName, "PKM 10", sizeof(header.aName) ) ) {
		return NULL;
	}

	width = (header.iWidthMSB << 8) | header.iWidthLSB;
	height = (header.iHeightMSB << 8) | header.iHeightLSB;

	*x = s->img_x = width;
	*y = s->img_y = height;
	*comp = s->img_n = 4;

	compressed_size = (((width + 3) & ~3) * ((height + 3) & ~3)) >> 1;

	pkm_data = (stbi_uc *)malloc(compressed_size);
	stbi__getn( s, pkm_data, compressed_size );

	pkm_res_data = (stbi_uc *)malloc(width * height * s->img_n);

	wfETC1_DecodeImage(pkm_data, pkm_res_data, width, height);

	free( pkm_data );

	if ( NULL != pkm_res_data ) {
		if( (req_comp < 4) && (req_comp >= 1) ) {
			//	user has some requirements, meet them
			if( req_comp != s->img_n ) {
				pkm_res_data = stbi__convert_format( pkm_res_data, s->img_n, req_comp, s->img_x, s->img_y );
				*comp = req_comp;
			}
		}

		return (stbi_uc *)pkm_res_data;
	} else {
		free( pkm_res_data );
	}

	return NULL;
}

#ifndef STBI_NO_STDIO
void *stbi__pkm_load_from_file   (FILE *f,                  int *x, int *y, int *comp, int req_comp)
{
	stbi__context s;
	stbi__start_file(&s,f);
	return stbi__pkm_load(&s,x,y,comp,req_comp);
}

void *stbi__pkm_load_from_path             (char const*filename,           int *x, int *y, int *comp, int req_comp)
{
   void *data;
   FILE *f = fopen(filename, "rb");
   if (!f) return NULL;
   data = stbi__pkm_load_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return data;
}
#endif

void *stbi__pkm_load_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__pkm_load(&s,x,y,comp,req_comp);
}

void *stbi__pkm_load_from_callbacks (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
	stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__pkm_load(&s,x,y,comp,req_comp);
}
