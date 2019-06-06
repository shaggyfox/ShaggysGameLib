#include "engine.h"
//#include "tileset.h"
//#include "map.h"

#define TILE_SIZE 32
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
      dst_data = &in_dst_data[(dst_y + y) * dst_pitch + (dst_x + x) * 4];
      src_data = &in_src_data[(src_y + y) * src_pitch + (src_x + x) * 4];
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
  if (!chunk->chunk_data) {
    chunk->chunk_data = calloc(1, sizeof(struct framemap_chunk_data));
  }
  struct framemap_chunk_data *chunk_meta = chunk->chunk_data;
  if (!chunk_meta->texture) {
    chunk_meta->texture = SDL_CreateTexture(glob_renderer,
                                            SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            MAP_CHUNK_SIZE * TILE_SIZE,
                                            MAP_CHUNK_SIZE * TILE_SIZE);
    SDL_SetTextureBlendMode(chunk_meta->texture, SDL_BLENDMODE_BLEND);
    /* force initial build */
    chunk_meta->rebuild = 1;
  }
  /* wenn Textur bereits besteht SDL_UpdateTexture() für jedes frame das sich
   * geändert hat
   * wenn eine  bestimmte Anzahl an updates überschritten wurde kann auch
   * einfach ein volles update gemacht werden */


  /** UPDATE WHOLE CHUNK **/
  if (chunk_meta->rebuild == 1) {
    chunk_meta->rebuild = 0;
    printf("rebuild\n");
    void *ptr;
    int pitch;
    SDL_LockTexture(chunk_meta->texture, NULL, &ptr, &pitch);
    for (int i = 0; i < MAP_CHUNK_SIZE * MAP_CHUNK_SIZE; ++i) {
      SDL_Rect dst = {.x = (i % MAP_CHUNK_SIZE) * TILE_SIZE,
                      .y = (i / MAP_CHUNK_SIZE) * TILE_SIZE,
                      .w = TILE_SIZE,
                      .h =TILE_SIZE};
      struct frame *frame = chunk->data[i];
      if (frame) {
        void *src_pixels = frame->frame_tileset->tileset_surface->pixels;
        int src_pitch = frame->frame_tileset->tileset_surface->pitch;
        raw_blit(ptr, &dst, pitch, src_pixels, &frame->frame_src, src_pitch);
      }
    }
    SDL_UnlockTexture(chunk_meta->texture);
  }
}

/* XXX replace with SDL_Rect? */
struct framemap_draw_chunk_callback_data {
  int x;
  int y;
  int w;
  int h;
  int map_x;
  int map_y;
};

/* XXX only draw that parts of each chunk that is 'in range' */
/* can use our stuff from collision-detection */
/* SDL_Rect_or ... */
/* x            : chunk position (in chunks)
 * y            : chunk position (in chunks)
 * chunk        : current chunk data
 * data         : callback_data
 *                * x : destination position (in pixel)
 *                * y : destination position (in pixel)
 *                * w : size of area to draw (in tiles)
 *                * h : size of area to draw (in tiles)
 */
void draw_chunk_callback(int x, int y, struct map_chunk* chunk, void *data)
{
  struct framemap_draw_chunk_callback_data *draw_data = data;
  update_chunk(chunk);
  struct framemap_chunk_data *chunk_meta = chunk->chunk_data;
  int draw_w = MAP_CHUNK_SIZE * TILE_SIZE;
  int draw_h = MAP_CHUNK_SIZE * TILE_SIZE;
  if (draw_w > draw_data->w * TILE_SIZE) {
    draw_w = draw_data->w * TILE_SIZE;
  }
  if (draw_h > draw_data->h * TILE_SIZE) {
    draw_h = draw_data->h * TILE_SIZE;
  }

  SDL_Rect chunk_rect = {x * MAP_CHUNK_SIZE, y * MAP_CHUNK_SIZE, MAP_CHUNK_SIZE, MAP_CHUNK_SIZE};
  SDL_Rect show_rect = {draw_data->map_x, draw_data->map_y, draw_data->w, draw_data->h};
  SDL_Rect result_rect;

  SDL_Rect_and(&chunk_rect, &show_rect, &result_rect);
  result_rect.x -= x * MAP_CHUNK_SIZE;
  result_rect.y -= y * MAP_CHUNK_SIZE;
  result_rect.x *= TILE_SIZE;
  result_rect.y *= TILE_SIZE;
  result_rect.w *= TILE_SIZE;
  result_rect.h *= TILE_SIZE;
  SDL_Rect dst_rect = result_rect;
  dst_rect.x += x * MAP_CHUNK_SIZE * TILE_SIZE + draw_data->x - TILE_SIZE * draw_data->map_x;
  dst_rect.y += y * MAP_CHUNK_SIZE * TILE_SIZE + draw_data->y - TILE_SIZE * draw_data->map_y;
  SDL_RenderCopy(glob_renderer, chunk_meta->texture, &result_rect, &dst_rect);
}

void framemap_draw(struct map* map, int pos_x, int pos_y, int map_x, int map_y, int w, int h)
{
  struct framemap_draw_chunk_callback_data cb_data = {pos_x, pos_y, w, h, map_x, map_y};
  map_foreach_chunk(map, map_x, map_y, w, h, &draw_chunk_callback, &cb_data);
}
