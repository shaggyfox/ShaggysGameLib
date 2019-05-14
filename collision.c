#include <math.h>
#include "collision.h"

struct collision_map *collision_map_from_surface(SDL_Surface *surface)
{
  struct collision_map *ret = malloc(sizeof(*ret));
  unsigned char *pixel_data = surface->pixels;
  ret->w = surface->w;
  ret->h = surface->h;
  ret->data = malloc(ret->w * ret->h);
  for (int i = 0; i < ret->w * ret->h; ++i) {
    ret->data[i] = pixel_data[i*4+3];
  }
  return ret;
}

int collision_map_get(struct collision_map *map, int x, int y)
{
  return map->data[map->w * y + x];
}

#include <stdio.h>
void collision_map_print(struct collision_map *map, SDL_Rect *rect)
{
  for (int y = rect->y; y < rect->y + rect->h; ++y) {
    for (int x = rect->x; x < rect->x + rect->w; ++ x) {
      if (collision_map_get(map, x, y)) {
        printf("x");
      } else {
        printf(" ");
      }
    }
  printf(";\n");
  }
}

/* a and b are the tiles to check XXX the tiles musst overlap */
/* a_src and b_src are the source-tiles in the tileset/collision_map */
int collision_map_check_collision(struct collision_map *map_a, struct collision_map *map_b,
    SDL_Rect *a, SDL_Rect *b, SDL_Rect *a_src, SDL_Rect *b_src)
{
  SDL_Rect c_rect;
  SDL_Rect_and(a, b, &c_rect);
  for (int x = 0; x < c_rect.w; ++x) {
    for (int y = 0; y < c_rect.h; ++ y) {
      int a_test = collision_map_get(map_a, (c_rect.x + x) - a->x + a_src->x,
                                      (c_rect.y + y) - a->y + a_src->y);

      int b_test = collision_map_get(map_b, (c_rect.x + x) - b->x + b_src->x,
                                      (c_rect.y + y) - b->y + b_src->y);
      if (a_test && b_test) {
        return 1;
      }
    }
  }
  return 0;
}

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
