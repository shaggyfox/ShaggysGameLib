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

/* XXX in scheme we only support FRAME. Arbitrary scheme-objects will get
 * flushed by gc */

// API_CALL map_get MAP INT INT RETURNS_FRAME
void *map_get(struct map* map, int x, int y);

// API_CALL map_set MAP INT INT FRAME
void map_set(struct map* map, int x, int y, void* map_data);

struct map *map_load(char *data, size_t length);
// API_CALL map_new INT INT RETURNS_MAP
struct map *map_new(int width, int height);

void map_free(struct map*);

/* cb(int x, int y, void* map_data, void* user_data) */
void map_foreach(struct map* map, int in_x, int in_y, int in_w, int in_h, void(*cb)(int, int, void*, void*), void* cb_data);
#endif
