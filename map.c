#include <stdlib.h>
#include "json_tk.h"
#include "map.h"

void *map_get(struct map* map, int x, int y)
{
  int chunk_id = (x / map->chunk_size) + (y / map->chunk_size) * map->width;
  struct map_chunk *chunk = &map->chunks[chunk_id];
  return chunk->data[(x % map->chunk_size) + (y % map->chunk_size) * map->chunk_size];
}

#define map_xy(map, x, y) \
  map->chunks[(x / map->chunk_size) + (y / map->chunk_size) * map->width].data[x % map->chunk_size + (y % map->chunk_size) * map->chunk_size]

void map_set(struct map* map, int x, int y, void* map_data)
{
  int chunk_id = (x / map->chunk_size) + (y / map->chunk_size) * map->width;
  struct map_chunk *chunk = &map->chunks[chunk_id];
  chunk->data[(x % map->chunk_size) + (y % map->chunk_size) * map->chunk_size] = map_data;
}

/* cb(int x, int y, void* map_data, void* user_data) */
void map_foreach(struct map* map, int in_x, int in_y, int in_w, int in_h, void(*cb)(int, int, void*, void*), void* cb_data)
{
  for (int y = in_y; y < in_y + in_h; ++y) {
    for (int x = in_x; x < in_x + in_w; ++x) {
      cb(x, y, map_xy(map, x, y), cb_data);
    }
  }
}
#include <stdio.h>
/* XXX PLEASE FIXME */
struct map *map_load(char *data, size_t length)
{
  struct map *ret = calloc(1, sizeof(*ret));
  json_value *v = json_parse(data, length);
  printf("%d\n", json_find_int(v, "height"));
  printf("%d\n", json_find_int(v, "width"));
  json_value *map_data = json_find_recursive(v, "data");
  printf("%p\n", map_data);
  json_value ** array = json_array_get_values(map_data);
  int len = json_array_get_length(map_data);
  for (int i = 0 ; i < len; ++i) {
    printf("%i", (int)array[i]->u.integer);
  }
  return ret;
}

struct map *map_new(int width, int height)
{
  struct map *ret = malloc(sizeof(*ret));
  ret->width = width / 64 + 1;
  ret->height = height / 64 + 1;
  ret->chunk_size = 64;
  ret->chunks = calloc(1, sizeof(*ret->chunks) * ret->width * ret->height);
  return ret;
}
