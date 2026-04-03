
#ifndef NULL
#define NULL 0
#endif

#define QOI_SRGB   0
#define QOI_LINEAR 1

typedef struct {
   unsigned int width;
   unsigned int height;
   unsigned char channels;
   unsigned char colorspace;
} qoi_desc;

#ifndef QOI_ZEROARR
   #define QOI_ZEROARR(a) memset((a),0,sizeof(a))
#endif

#define QOI_OP_INDEX  0x00 /* 00xxxxxx */
#define QOI_OP_DIFF   0x40 /* 01xxxxxx */
#define QOI_OP_LUMA   0x80 /* 10xxxxxx */
#define QOI_OP_RUN    0xc0 /* 11xxxxxx */
#define QOI_OP_RGB    0xfe /* 11111110 */
#define QOI_OP_RGBA   0xff /* 11111111 */

#define QOI_COLOR_HASH(C) (C.rgba.r*3 + C.rgba.g*5 + C.rgba.b*7 + C.rgba.a*11)
#define QOI_MAGIC \
   (((unsigned int)'q') << 24 | ((unsigned int)'o') << 16 | \
    ((unsigned int)'i') <<  8 | ((unsigned int)'f'))
#define QOI_HEADER_SIZE 14

/* 2GB is the max file size that this implementation can safely handle. We guard
against anything larger than that, assuming the worst case with 5 bytes per
pixel, rounded down to a nice clean value. 400 million pixels ought to be
enough for anybody. */
#define QOI_PIXELS_MAX ((unsigned int)400000000)

typedef union {
   struct { unsigned char r, g, b, a; } rgba;
   unsigned int v;
} qoi_rgba_t;

static const unsigned char qoi_padding[8] = {0,0,0,0,0,0,0,1};

static void qoi_write_32(unsigned char *bytes, int *p, unsigned int v) {
   bytes[(*p)++] = (0xff000000 & v) >> 24;
   bytes[(*p)++] = (0x00ff0000 & v) >> 16;
   bytes[(*p)++] = (0x0000ff00 & v) >> 8;
   bytes[(*p)++] = (0x000000ff & v);
}

static void *qoi_encode(const void *data, const qoi_desc *desc, int *out_len) {
   int i, max_size, p, run;
   int px_len, px_end, px_pos, channels;
   unsigned char *bytes;
   const unsigned char *pixels;
   qoi_rgba_t index[64];
   qoi_rgba_t px, px_prev;

   if (
      data == NULL || out_len == NULL || desc == NULL ||
      desc->width == 0 || desc->height == 0 ||
      desc->channels < 3 || desc->channels > 4 ||
      desc->colorspace > 1 ||
      desc->height >= QOI_PIXELS_MAX / desc->width
   ) {
      return NULL;
   }

   max_size =
      desc->width * desc->height * (desc->channels + 1) +
      QOI_HEADER_SIZE + sizeof(qoi_padding);

   p = 0;
   bytes = (unsigned char *) STBIW_MALLOC(max_size);
   if (!bytes) {
      return NULL;
   }

   qoi_write_32(bytes, &p, QOI_MAGIC);
   qoi_write_32(bytes, &p, desc->width);
   qoi_write_32(bytes, &p, desc->height);
   bytes[p++] = desc->channels;
   bytes[p++] = desc->colorspace;


   pixels = (const unsigned char *)data;

   QOI_ZEROARR(index);

   run = 0;
   px_prev.rgba.r = 0;
   px_prev.rgba.g = 0;
   px_prev.rgba.b = 0;
   px_prev.rgba.a = 255;
   px = px_prev;

   px_len = desc->width * desc->height * desc->channels;
   px_end = px_len - desc->channels;
   channels = desc->channels;

   for (px_pos = 0; px_pos < px_len; px_pos += channels) {
      px.rgba.r = pixels[px_pos + 0];
      px.rgba.g = pixels[px_pos + 1];
      px.rgba.b = pixels[px_pos + 2];

      if (channels == 4) {
         px.rgba.a = pixels[px_pos + 3];
      }

      if (px.v == px_prev.v) {
         run++;
         if (run == 62 || px_pos == px_end) {
            bytes[p++] = QOI_OP_RUN | (run - 1);
            run = 0;
         }
      }
      else {
         int index_pos;

         if (run > 0) {
            bytes[p++] = QOI_OP_RUN | (run - 1);
            run = 0;
         }

         index_pos = QOI_COLOR_HASH(px) % 64;

         if (index[index_pos].v == px.v) {
            bytes[p++] = QOI_OP_INDEX | index_pos;
         }
         else {
            index[index_pos] = px;

            if (px.rgba.a == px_prev.rgba.a) {
               signed char vr = px.rgba.r - px_prev.rgba.r;
               signed char vg = px.rgba.g - px_prev.rgba.g;
               signed char vb = px.rgba.b - px_prev.rgba.b;

               signed char vg_r = vr - vg;
               signed char vg_b = vb - vg;

               if (
                  vr > -3 && vr < 2 &&
                  vg > -3 && vg < 2 &&
                  vb > -3 && vb < 2
               ) {
                  bytes[p++] = QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
               }
               else if (
                  vg_r >  -9 && vg_r <  8 &&
                  vg   > -33 && vg   < 32 &&
                  vg_b >  -9 && vg_b <  8
               ) {
                  bytes[p++] = QOI_OP_LUMA     | (vg   + 32);
                  bytes[p++] = (vg_r + 8) << 4 | (vg_b +  8);
               }
               else {
                  bytes[p++] = QOI_OP_RGB;
                  bytes[p++] = px.rgba.r;
                  bytes[p++] = px.rgba.g;
                  bytes[p++] = px.rgba.b;
               }
            }
            else {
               bytes[p++] = QOI_OP_RGBA;
               bytes[p++] = px.rgba.r;
               bytes[p++] = px.rgba.g;
               bytes[p++] = px.rgba.b;
               bytes[p++] = px.rgba.a;
            }
         }
      }
      px_prev = px;
   }

   for (i = 0; i < (int)sizeof(qoi_padding); i++) {
      bytes[p++] = qoi_padding[i];
   }

   *out_len = p;
   return bytes;
}

static int stbi_write_qoi_core(stbi__write_context *s, int x, int y, int comp, const void*data)
{
   if (y <= 0 || x <= 0 || data == NULL)
      return 0;
   qoi_desc desc;
   desc.width = x;
   desc.height = y;
   desc.channels = comp;
   desc.colorspace = QOI_LINEAR;
   int out_len = 0;
   void* res = qoi_encode(data, &desc, &out_len);
   if (res != NULL)
      s->func(s->context, res, out_len);
   return res != NULL ? 1 : 0;
}

STBIWDEF int stbi_write_qoi_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void *data)
{
   stbi__write_context s = { 0 };
   stbi__start_write_callbacks(&s, func, context);
   return stbi_write_qoi_core(&s, x, y, comp, (void *) data);
}

#ifndef STBI_WRITE_NO_STDIO
STBIWDEF int stbi_write_qoi(char const *filename, int x, int y, int comp, const void *data)
{
   stbi__write_context s = { 0 };
   if (stbi__start_write_file(&s,filename)) {
      int r = stbi_write_qoi_core(&s, x, y, comp, (void *) data);
      stbi__end_write_file(&s);
      return r;
   } else
      return 0;
}
#endif
