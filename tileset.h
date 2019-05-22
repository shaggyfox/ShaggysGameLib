#ifndef tileset_h
#define tileset_h
#include <SDL.h>
#include "collision.h"

struct frame {
  SDL_Rect frame_src;
  float frame_duration; /* used for animation */
  /* back references */
  struct tileset *frame_tileset;
  //struct animation *frame_animation;
};

#define ANIMATION_TYPE_FORWARD 0
#define ANIMATION_TYPE_BACKWARD 1
#define ANIMATION_TYPE_PINGPONG 2
struct animation {
  char *animation_name;
  int animation_begin;
  int animation_end;
  int animation_type;
  void *cb_data;
  float (*duration_modifier_cb)(void *cb_data);
  /* back references */
  struct frame *animation_frames; /* pointer to tileset_frames */
  struct tileset *animation_tileset; /* pointer to tileset */
  struct SDL_Texture *animation_texture; /* pointer to tileset_texture */
};

struct animation_ctx {
  struct animation* animation_ctx_animation;
  struct animation* animation_ctx_next_animation;
  void *animation_ctx_next_animation_cb_data;
  void (*animation_ctx_next_animation_cb)(struct animation_ctx *, void*);
  int animation_ctx_pos;
  float animation_ctx_timeleft;
  int animation_ctx_pingpong_direction;
};

struct tileset {
  char *tileset_name; /* the name of the tileset */
  SDL_Texture *tileset_texture; /* the (one and only) texture of the tileset */
  struct animation *tileset_animations; /* all animations in the tileset */
  struct frame *tileset_frames; /* all frames (tiles) in the tileset */
  int tileset_frame_count;
  int tileset_animation_count;
  struct collision_map *tileset_collision_map;
};

/* load tileset from data */
/* FIXME: need to load texture
 * */
struct tileset *tileset_load(char *json_data, size_t json_data_len, char* image_data, size_t image_data_len);

/* find animation by name in tileset */
struct animation *tileset_get_animation(struct tileset* tileset, char *name);

int frame_check_collision(int a_x, int a_y, struct frame *frame_a,
                          int b_x, int b_y, struct frame *frame_b);

/* initialize animation context with animation */
void animation_ctx_init(struct animation_ctx *ctx, struct animation *animation);
/* use new animation from now */
void animation_ctx_set_animation(struct animation_ctx *ctx, struct animation *animation);
/* use new animation when current animation is over, call callback when new
 * animation starts */
void animation_ctx_set_next_animation(
    struct animation_ctx *ctx,
    struct animation *animation,
    void (*next_animation_cb)(struct animation_ctx *, void *),
    void *cb_data);
/* call this to update animation context, delta is in seconds */
void animation_ctx_update(struct animation_ctx *ctx, float delta);

struct frame *animation_ctx_get_frame(struct animation_ctx *ctx);
struct frame *tileset_get_frame(struct tileset *tileset, char *name);
struct frame *tileset_get_frame_by_id(struct tileset *tileset, int id);
void tileset_color_mod(struct tileset *tileset, int r, int g, int b, int a);

SDL_Texture* load_texture(char *data, size_t len, int *w, int *h, struct collision_map **);
#endif
