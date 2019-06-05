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

void raw_blit(char *in_dst_data, SDL_Rect *dst, int dst_pitch, char *in_src_data, SDL_Rect *src, int src_pitch)
{
  int src_x = src->x;
  int src_y = src->y;
  int dst_x = dst->x;
  int dst_y = dst->y;
  char *src_data;
  char *dst_data;
  for (int y = 0; y < src->h; ++y) {
    for (int x = 0; x < src->w; ++x) {
      dst_data = &in_dst_data[(dst_y + y) * dst_pitch + dst_x + x];
      src_data = &in_src_data[(src_y + y) * src_pitch + src_x + x];
      dst_data[0] = src_data[0];
      dst_data[1] = src_data[1];
      dst_data[2] = src_data[2];
      dst_data[3] = src_data[3];
    }
  }
}

void update_chunk(struct map_chunk *chunk)
{
  /* wenn keine textur existiert -> fastmode */
  /* im fast mode gehen wir davon aus das sich alle frames geändert haben */
  /* dafür erstellen wir sowas wie raw_blit(sdl_rect dst, dst_pitch, sdl_rect src, src_pitch);
   * das kann übrigens auch für das update per frame benutzt werden */
  struct framemap_chunk_data *chunk_meta = chunk->chunk_data;
  if (!chunk_meta->texture) {
    chunk_meta->texture = SDL_CreateTexture(glob_renderer,
                                            SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            MAP_CHUNK_SIZE * 8,
                                            MAP_CHUNK_SIZE * 8);
    /* force initial build */
    chunk_meta->rebuild = 1;
  }
  /* wenn Textur bereits besteht SDL_UpdateTexture() für jedes frame das sich
   * geändert hat
   * wenn eine  bestimmte Anzahl an updates überschritten wurde kann auch
   * einfach ein volles update gemacht werden */
  void *ptr;
  int pitch;
  SDL_LockTexture(chunk_meta->texture, NULL, &ptr, &pitch);
  for (int i = 0; i < MAP_CHUNK_SIZE * MAP_CHUNK_SIZE; ++i) {
    SDL_Rect dst = {.x = (i % MAP_CHUNK_SIZE) * 8,
                    .y = (i / MAP_CHUNK_SIZE) * 8,
                    .w = 8,
                    .h =8};
    struct frame *frame = chunk->data[i];
    if (frame) {
      void *src_pixels = frame->frame_tileset->tileset_surface->pixels;
      int src_pitch = frame->frame_tileset->tileset_surface->pitch;
      raw_blit(ptr, &dst, pitch, src_pixels, &frame->frame_src, src_pitch);
    }
  }
  SDL_UnlockTexture(chunk_meta->texture);
}

void framemap_draw(struct map* map, int pos_x, int pos_y, int map_x, int map_y, int w, int h)
{
  
}
