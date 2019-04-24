#include "vector.h"

void vec2d_mul_sca(struct vec2d *v, float s, struct vec2d *result)
{
  result->x = v->x * s;
  result->y = v->y * s;
}

void vec2d_add(struct vec2d *a, struct vec2d *b, struct vec2d *result)
{
  result->x = a->x + b->x;
  result->y = a->y + b->y;
}


