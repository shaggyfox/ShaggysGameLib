#include "draw.h"

void draw_frame_ex(struct frame *frame, int x, int y, double rotation, int flags)
{
  SDL_Rect *src = &frame->frame_src;
  SDL_Rect dst = {.x = x, .y = y, .w = src->w, .h = src->h};
  SDL_RenderCopyEx(glob_renderer, frame->frame_tileset->tileset_texture, src, &dst, rotation, NULL, flags);
}

void draw_frame(struct frame *frame, int x, int y)
{
  draw_frame_ex(frame, x, y, 0, 0);
}


