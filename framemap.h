#ifndef FRAMEMAP_H
#define FRAMEMAP_H
#include "engine.h"
// API_CALL framemap_new INT INT INT INT RETURNS_MAP
struct map *framemap_new(int w, int h, int tile_size, int chunk_size);
// API_CALL framemap_set MAP INT INT FRAME
void framemap_set(struct map* map, int x, int y, struct frame *frame);
// API_CALL framemap_get MAP INT INT RETURNS_FRAME
struct frame *framemap_get(struct map *map, int x, int y);
// API_CALL framemap_draw MAP INT INT INT INT INT INT
void framemap_draw(struct map* map, int pos_x, int pos_y, int map_x, int map_y, int w, int h);
#endif
