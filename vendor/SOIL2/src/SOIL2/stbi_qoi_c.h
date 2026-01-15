
static int stbi__qoi_test(stbi__context *s)
{
   int i;
   for (i = 0; i < 4; i++) {
      if (stbi__get8(s) != "qoif"[i]) {
         stbi__rewind(s);
         return 0;
      }
   }
   return 1;
}

#ifndef STBI_NO_STDIO

int stbi__qoi_test_filename(char const *filename)
{
   int r;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   r = stbi__qoi_test_file(f);
   fclose(f);
   return r;
}

int stbi__qoi_test_file(FILE *f)
{
   stbi__context s;
   int r,n = ftell(f);
   stbi__start_file(&s,f);
   r = stbi__qoi_test(&s);
   fseek(f,n,SEEK_SET);
   return r;
}
#endif

int      stbi__qoi_test_memory      (stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__qoi_test(&s);
}

int      stbi__qoi_test_callbacks      (stbi_io_callbacks const *clbk, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__qoi_test(&s);
}

static int stbi__qoi_info(stbi__context *s, int *x, int *y, int *comp)
{
   stbi__uint32 length;

   stbi__rewind(s);

   int i;
   for (i = 0; i < 4; i++) {
      if (stbi__get8(s) != "qoif"[i]) {
         stbi__rewind(s);
         return 0;
      }
   }

   length = stbi__get32be(s);
   if (x) *x = (int)length;

   length = stbi__get32be(s);
   if (y) *y = (int)length;

   int channels   = stbi__get8(s);       // 3 = RGB, 4 = RGBA
   int colorspace = stbi__get8(s) != 0;  // 0 = sRGB with linear alpha, 1 = all channels linear

   if (comp) *comp = channels;

   return 1 + colorspace;
}

int stbi__qoi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp )
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__qoi_info( &s, x, y, comp );
}

int stbi__qoi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__qoi_info( &s, x, y, comp );
}

#ifndef STBI_NO_STDIO
int stbi__qoi_info_from_path(char const *filename, int *x, int *y, int *comp)
{
   int res;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   res = stbi__qoi_info_from_file( f, x, y, comp );
   fclose(f);
   return res;
}

int stbi__qoi_info_from_file(FILE *f, int *x, int *y, int *comp)
{
   stbi__context s;
   int res;
   long n = ftell(f);
   stbi__start_file(&s, f);
   res = stbi__qoi_info(&s, x, y, comp);
   fseek(f, n, SEEK_SET);
   return res;
}
#endif

static void *stbi__qoi_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   if (!stbi__qoi_info(s, (int*)&s->img_x, (int*)&s->img_y, &s->img_n))
      return NULL;

   if (req_comp == 3 || req_comp == 4)
      s->img_n = req_comp;

   if (x) *x = s->img_x;
   if (y) *y = s->img_y;
   if (comp) *comp = s->img_n;

   if (!stbi__mad3sizes_valid(s->img_x, s->img_y, s->img_n, 0))
      return stbi__errpuc("too large", "QOI too large");

   stbi_uc *out = (stbi_uc *) stbi__malloc_mad3(s->img_x, s->img_y, s->img_n, 0);
   if (!out)
      return stbi__errpuc("outofmem", "Out of memory");

   stbi_uc *dst = out;
   stbi_uc *end = &dst[s->img_x * s->img_y * s->img_n];

   stbi__uint32 c = 255;
   stbi__uint32 recent[0x40];
   memset(recent, 0, 0x40*sizeof(stbi__uint32));

   while (dst < end) {
      stbi_uc tag = stbi__get8(s);
      if (tag == 0xfe) {
         stbi__uint32 rgb =
            ((stbi__uint32) stbi__get8(s) << 24) |
            ((stbi__uint32) stbi__get8(s) << 16) |
            ((stbi__uint32) stbi__get8(s) << 8);
         c = (c & 0xff) | rgb;
      }
      else if (tag == 0xff) {
         c =
            ((stbi__uint32) stbi__get8(s) << 24) |
            ((stbi__uint32) stbi__get8(s) << 16) |
            ((stbi__uint32) stbi__get8(s) << 8) |
            (stbi__uint32) stbi__get8(s);
      }
      else if ((tag >> 6) == 0) {
         c = recent[tag];
      }
      else if ((tag >> 6) == 1) {
         stbi_uc r = (c >> 24) + ((tag >> 4) & 3) - 2;
         stbi_uc g = (c >> 16) + ((tag >> 2) & 3) - 2;
         stbi_uc b = (c >> 8)  + (tag & 3) - 2;
         c = (c & 0xff) | ((stbi__uint32) r << 24) | ((stbi__uint32) g << 16) | ((stbi__uint32) b << 8);
      }
      else if ((tag >> 6) == 2) {
         stbi_uc rb = stbi__get8(s);
         stbi_uc dg = (tag & 0x3f) - 32;
         stbi_uc r = (c >> 24) + dg + (rb >> 4) - 8;
         stbi_uc g = (c >> 16) + dg;
         stbi_uc b = (c >> 8)  + dg + (rb & 0xf) - 8;
         c = (c & 0xff) | ((stbi__uint32) r << 24) | ((stbi__uint32) g << 16) | ((stbi__uint32) b << 8);
      }
      else {
         int run = tag & 0x3f;
         if (&dst[run * s->img_n] > end)
            break;

         if (s->img_n == 3) {
            int i;
            for (i = 0; i < run; i++) {
               *dst++ = c >> 24;
               *dst++ = c >> 16;
               *dst++ = c >> 8;
            }
         }
         else {
            int i;
            for (i = 0; i < run; i++) {
               *dst++ = c >> 24;
               *dst++ = c >> 16;
               *dst++ = c >> 8;
               *dst++ = c;
            }
         }
      }

      *dst++ = c >> 24;
      *dst++ = c >> 16;
      *dst++ = c >> 8;
      if (s->img_n != 3) *dst++ = c;

      recent[0x3f & (
         ((c >> 24) & 0xff) * 3u +
         ((c >> 16) & 0xff) * 5u +
         ((c >>  8) & 0xff) * 7u +
         (c & 0xff) * 11u
      )] = c;
   }

   if (req_comp)
      out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);

   return out;
}

#ifndef STBI_NO_STDIO
void *stbi__qoi_load_from_file(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__result_info ri;
   stbi__start_file(&s,f);
   return stbi__qoi_load(&s,x,y,comp,req_comp,&ri);
}

void *stbi__qoi_load_from_path(char const*filename, int *x, int *y, int *comp, int req_comp)
{
   void *data;
   FILE *f = fopen(filename, "rb");
   if (!f) return NULL;
   data = stbi__qoi_load_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return data;
}
#endif

void *stbi__qoi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__result_info ri;
   stbi__start_mem(&s,buffer, len);
   return stbi__qoi_load(&s,x,y,comp,req_comp,&ri);
}

void *stbi__qoi_load_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__result_info ri;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__qoi_load(&s,x,y,comp,req_comp,&ri);
}
