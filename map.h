#ifndef map_h
#define map_h
#include <sys/types.h>
struct map_chunk {
  void *data[64 * 64];
};

struct map {
  int width; /* in chunks */
  int height; /* in chunks */
  int chunk_size;
  struct map_chunk *chunks;
};

void *map_get(struct map* map, int x, int y);
void map_set(struct map* map, int x, int y, void* map_data);
struct map *map_load(char *data, size_t length);
struct map *map_new(int width, int height);

/* cb(int x, int y, void* map_data, void* user_data) */
void map_foreach(struct map* map, int in_x, int in_y, int in_w, int in_h, void(*cb)(int, int, void*, void*), void* cb_data);
#endif
