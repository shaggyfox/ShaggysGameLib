#ifndef BASE64_H
#define BASE64_H 1

struct b64_buff {
  char b[4];
  size_t len;
  size_t line_len;
  size_t max_line_len;
  char *line_term;
  size_t line_term_len;
};

void b64_encode(struct b64_buff *buff, const char *in, size_t in_size, char *out, size_t *out_size, int flush);
void b64_decode(struct b64_buff *buff, const char *in, size_t in_size, char *out, size_t *out_size);

#define b64_encode_size(s) (((s) / 3) * 4 + (((s) % 3) ? 4 : 0))

#endif
