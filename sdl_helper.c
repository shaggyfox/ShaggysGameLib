#include "sdl_helper.h"

int SDL_Rect_check_collision(SDL_Rect *a, SDL_Rect *b)
{
  float A = fabs(a->x - b->x + (a->w - b->w) / 2.0);
  float A2 = (a->w + b->w) / 2.0;
  float B = fabs(a->y - b->y + (a->h - b->h) / 2.0);
  float B2 = (a->h + b->h) / 2.0;
  return A < A2 && B < B2;
}

void SDL_Rect_and(SDL_Rect *in_a, SDL_Rect *in_b, SDL_Rect *ret)
{
  SDL_Rect *a;
  SDL_Rect *b;

  if (in_a->x < in_b->x) {
    a = in_a; b = in_b;
  } else {
    a = in_b; b = in_a;
  }

  ret->w = a->x + a->w - b->x;
  ret->x = b->x;

  if (ret->w > b->w) {
    ret->w = b->w;
  }

  if (in_a->y < in_b->y) {
    a = in_a; b = in_b;
  } else {
    a = in_b; b = in_a;
  }

  ret->h = a->y + a->h - b->y;
  ret->y = b->y;
  if (ret->h > b->h) {
    ret->h = b->h;
  }
}
