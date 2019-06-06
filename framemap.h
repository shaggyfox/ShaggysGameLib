#ifndef FRAMEMAP_H
#define FRAMEMAP_H
#include "engine.h"
struct map *framemap_new(int w, int h);
void framemap_set(struct map* map, int x, int y, struct frame *frame);
struct frame *framemap_get(struct map *map, int x, int y);
void framemap_draw(struct map* map, int pos_x, int pos_y, int map_x, int map_y, int w, int h);
#endif
