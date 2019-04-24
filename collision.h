#ifndef collision_h
#define collision_h
#include <SDL.h>

struct collision_map {
  int w;
  int h;
  unsigned char *data;
};

struct collision_map *collision_map_from_surface(SDL_Surface *surface);

int collision_map_get(struct collision_map *map, int x, int y);
int collision_map_check_collision(struct collision_map *map_a, struct collision_map *map_p,
    SDL_Rect *a, SDL_Rect *b, SDL_Rect *a_src, SDL_Rect *b_src);
int SDL_Rect_check_collision(SDL_Rect *a, SDL_Rect *b);
void SDL_Rect_and(SDL_Rect *in_a, SDL_Rect *in_b, SDL_Rect *ret);
void SDL_Rect_rand(SDL_Rect *ret);

/* DBG */
void collision_map_print(struct collision_map *map, SDL_Rect *rect);
#endif

