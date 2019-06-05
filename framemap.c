#include "engine.h"
//#include "tileset.h"
//#include "map.h"

#define MAP_TYPE_FRAMEMAP 1
struct map *framemap_new(int w, int h)
{
  struct map *ret = map_new(w, h, MAP_TYPE_FRAMEMAP);
  return ret;
}

struct frame *framemap_get(struct map *map, int x, int y) {
  return map_get(map, x, y);
}

void framemap_set(struct map* map, int x, int y, struct frame *frame)
{
  map_set(map, x, y, frame);
}

struct framemap_chunk_data {
 SDL_Texture *texture;
 int rebuild;
};
#if 0
static void update_chunk(struct map_chunk *chunk)
{
  SDL_CreateTexture(glob_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, MAP_CHUNK_SIZE, MAP_CHUNK_SIZE);
}
#endif

void framemap_draw(struct map* map, int pos_x, int pos_y, int map_x, int map_y, int w, int h)
{
  
}
