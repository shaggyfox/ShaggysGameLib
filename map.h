#ifndef map_h
#define map_h
#include <sys/types.h>


#define MAP_CHUNK_SIZE 32
struct map_chunk {
  void *chunk_data;
  void *data[MAP_CHUNK_SIZE * MAP_CHUNK_SIZE];
};

struct map {
  int type;
  int width; /* in tiles */
  int height; /* in tiles */
  int w_chunks; /* width in chunks */
  int h_chunks; /* height in chunks */
  int chunk_size;
  struct map_chunk *chunks;
};

int map_type(struct map* map);

void *map_get(struct map* map, int x, int y);

void map_set(struct map* map, int x, int y, void* map_data);

struct map *map_load(char *data, size_t length);

struct map *map_new(int width, int height, int type);

void map_free(struct map*);

void map_foreach_chunk(struct map* map, int in_x, int in_y, int in_w, int in_h,
    void (*cb)(int, int, struct map_chunk*, void*), void *cb_data);

void map_chunk_foreach(struct map_chunk *chunk, int x, int y, int w, int in_h,
    void (*cb)(int, int, void*, void*), void *cb_data);

/* cb(int x, int y, void* map_data, void* user_data) */
void map_foreach(struct map* map, int in_x, int in_y, int in_w, int in_h,
    void(*cb)(int, int, void*, void*), void* cb_data);
#endif
