#include "draw.h"

void draw_frame_ex(int x, int y, struct frame *frame, double rotation, int flags)
{
  SDL_Rect *src = &frame->frame_src;
  SDL_Rect dst = {.x = x, .y = y, .w = src->w, .h = src->h};
  SDL_RenderCopyEx(glob_renderer, frame->frame_tileset->tileset_texture, src, &dst, rotation, NULL, flags);
}

void draw_frame(int x, int y, struct frame *frame)
{
  draw_frame_ex(x, y, frame, 0, 0);
}


