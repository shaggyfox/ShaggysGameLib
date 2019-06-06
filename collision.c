#include <math.h>
#include "collision.h"
#include "sdl_helper.h"

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

