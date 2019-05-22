#ifndef VECTOR_H
#define VECTOR_H

struct vec2d {
  float x;
  float y;
};

void vec2d_mul_sca(struct vec2d *v, float s, struct vec2d *result);
void vec2d_add(struct vec2d *a, struct vec2d *b, struct vec2d *result);

#endif
