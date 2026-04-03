
static int stbi_test_main(stbi__context *s)
{
   #ifndef STBI_NO_JPEG
   if (stbi__jpeg_test(s)) return STBI_jpeg;
   #endif
   #ifndef STBI_NO_PNG
   if (stbi__png_test(s))  return STBI_png;
   #endif
   #ifndef STBI_NO_BMP
   if (stbi__bmp_test(s))  return STBI_bmp;
   #endif
   #ifndef STBI_NO_GIF
   if (stbi__gif_test(s))  return STBI_gif;
   #endif
   #ifndef STBI_NO_PSD
   if (stbi__psd_test(s))  return STBI_psd;
   #endif
   #ifndef STBI_NO_PIC
   if (stbi__pic_test(s))  return STBI_pic;
   #endif
   #ifndef STBI_NO_PNM
   if (stbi__pnm_test(s))  return STBI_pnm;
   #endif
   #ifndef STBI_NO_DDS
   if (stbi__dds_test(s))  return STBI_dds;
   #endif
   #ifndef STBI_NO_PVR
   if (stbi__pvr_test(s))  return STBI_pvr;
   #endif
   #ifndef STBI_NO_PKM
   if (stbi__pkm_test(s))  return STBI_pkm;
   #endif
   #ifndef STBI_NO_QOI
   if (stbi__qoi_test(s))  return STBI_qoi;
   #endif
   #ifndef STBI_NO_HDR
   if (stbi__hdr_test(s))  return STBI_hdr;
   #endif
   #ifndef STBI_NO_TGA
   if (stbi__tga_test(s))  return STBI_tga;
   #endif
   return STBI_unknown;
}

#ifndef STBI_NO_STDIO
int stbi_test_from_file(FILE *f)
{
   stbi__context s;
   stbi__start_file(&s,f);
   return stbi_test_main(&s);
}

int stbi_test(char const *filename)
{
   FILE *f = fopen(filename, "rb");
   int result;
   if (!f) return STBI_unknown;
   result = stbi_test_from_file(f);
   fclose(f);
   return result;
}
#endif //!STBI_NO_STDIO

int stbi_test_from_memory(stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi_test_main(&s);
}

int stbi_test_from_callbacks(stbi_io_callbacks const *clbk, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi_test_main(&s);
}
