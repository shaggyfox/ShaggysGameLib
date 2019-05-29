#include <string.h>
#include <sys/types.h>

#include "base64.h"

static char b64_encode_chars[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
    "ghijklmnopqrstuvwxyz0123456789+/";

static char b64_decode_chars[256] =
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 20 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 41 */
      -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, 0, -1,  /* 62 */
      -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,           /* 83 */
      19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, /* 104 */
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, /* 125 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 146 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 167 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 188 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 209 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 230 */
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 251 */
      -1, -1, -1, -1 };

#define b64_encode_chunk(in, out)                                        \
  (out)[0] = b64_encode_chars[(in[0] >> 2) & 63];                        \
  (out)[1] = b64_encode_chars[((in[0] & 3) << 4) | ((in[1] >> 4) & 15)]; \
  (out)[2] = b64_encode_chars[((in[1] & 15) << 2) | ((in[2] >> 6) & 3)]; \
  (out)[3] = b64_encode_chars[(in[2] & 63)];

void b64_encode(struct b64_buff *buff, const char *in, size_t in_size, char *out, size_t *out_size, int flush)
{
  size_t in_pos = 0;
  size_t out_pos = 0;
  int fill = 0;
  *out_size = 0;
  do {
    while (buff->len < 3 && in_pos < in_size) {
      buff->b[buff->len++] = in[in_pos++];
    }
    if (buff->len == 3) {
      if (buff->max_line_len && buff->line_len >= buff->max_line_len) {
        memcpy(out + (*out_size), buff->line_term, buff->line_term_len);
        (*out_size) += buff->line_term_len;
        buff->line_len = 0;
      }
      b64_encode_chunk(buff->b, out + *out_size);
      *out_size += 4;
      buff->len = 0;
      if (buff->max_line_len) {
        buff->line_len += 4;
      }
    } else if (flush && buff->len != 0) {
      if (buff->max_line_len && buff->line_len >= buff->max_line_len) {
        memcpy(out + (*out_size), buff->line_term, buff->line_term_len);
        (*out_size) += buff->line_term_len;
      }
      buff->line_len = 0;
      while (buff->len < 3) {
        buff->b[buff->len++] = 0;
        ++fill;
      }
      b64_encode_chunk(buff->b, out + *out_size);
      *out_size += 4;
      buff->len = 0;
      out_pos = *out_size;
      while (fill--) {
        out[--out_pos] = '=';
      }
    }
  } while (in_pos < in_size);
  if (buff->max_line_len && flush) {
    memcpy(out + (*out_size), buff->line_term, buff->line_term_len);
    (*out_size) += buff->line_term_len;
  }
}

#define b64_decode_chunk(in, out)                                                                                  \
  (out)[0] = (b64_decode_chars[(unsigned char)in[0]] << 2) | ((b64_decode_chars[(unsigned char)in[1]] >> 4) & 3);  \
  (out)[1] = (b64_decode_chars[(unsigned char)in[1]] << 4) | ((b64_decode_chars[(unsigned char)in[2]] >> 2) & 15); \
  (out)[2] = (b64_decode_chars[(unsigned char)in[2]] << 6) | (b64_decode_chars[(unsigned char)in[3]]);

void b64_decode(struct b64_buff *buff, const char *in, size_t in_size, char *out, size_t *out_size)
{
  size_t in_pos = 0;
  size_t trunc = 0;
  *out_size = 0;

  while (in_pos < in_size) {
    while (buff->len < 4 && in_pos < in_size) {
      if (in[in_pos] == '=' && trunc < 2) {
        ++trunc;
      }
      if (b64_decode_chars[(unsigned char)in[in_pos]] != -1) {
        buff->b[buff->len++] = in[in_pos];
      }
      in_pos++;
    }
    if (buff->len == 4) {
      b64_decode_chunk(buff->b, out + *out_size);
      *out_size += 3;
      buff->len = 0;
      *out_size -= trunc;
    }
  }
}
