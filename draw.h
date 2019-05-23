#ifndef DRAW_H
#define DRAW_H

#include "engine.h"
#include "tileset.h"

// API_CALL clear_screen
#define clear_screen() SDL_RenderClear(glob_renderer);
// API_CALL draw_line INT INT INT INT
#define draw_line(x1, y1, x2, y2) SDL_RenderDrawLine(glob_renderer, x1, y1, x2, y2)
#define draw_point(x, y) SDL_RenderDrawPoint(glob_renderer, x, y)
// API_CALL draw_color INT INT INT INT
#define draw_color(r, g, b, a) SDL_SetRenderDrawColor(glob_renderer, r, g, b, a)
#define draw_rect(rect) SDL_RenderDrawRect(glob_renderer, rect)
#define draw_rect4(x,y,w,h) SDL_RenderDrawRect(glob_renderer, &(SDL_Rect){x, y, w ,h})
#define draw_fill_rect(rect) SDL_RenderFillRect(glob_renderer, rect)
#define draw_fill_rect4(x,y,w,h) SDL_RenderFillRect(glob_renderer, &(SDL_Rect){x, y, w ,h})
void draw_frame_ex(struct frame *frame, int x, int y, double rotation, int flags);
void draw_frame(struct frame *frame, int x, int y);

#endif
