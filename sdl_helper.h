#ifndef SDL_HELPER_H
#define SDL_HELPER_H
#include <SDL.h>
int SDL_Rect_check_collision(SDL_Rect *a, SDL_Rect *b);
void SDL_Rect_and(SDL_Rect *in_a, SDL_Rect *in_b, SDL_Rect *ret);
#endif
