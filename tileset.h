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

// API_CALL tileset_load_from_file STR RETURNS_TILESET
struct tileset *tileset_load_from_file(char *json_file_name);

/* find animation by name in tileset */

// API_CALL tileset_get_animation TILESET STR RETURNS_ANIMATION
struct animation *tileset_get_animation(struct tileset* tileset, char *name);

// API_CALL frame_check_collision INT INT FRAME INT INT FRAME
int frame_check_collision(int a_x, int a_y, struct frame *frame_a,
                          int b_x, int b_y, struct frame *frame_b);

/* initialize animation context with animation */
void animation_ctx_init(struct animation_ctx *ctx, struct animation *animation);

/* create a new animation_ctx initialized with animation */

// API_CALL animation_ctx_new ANIMATION RETURNS_ANIMATION_CTX
struct animation_ctx *animation_ctx_new(struct animation *animation);

/* use new animation from now */

// API_CALL animation_ctx_set_animation ANIMATION_CTX ANIMATION
void animation_ctx_set_animation(struct animation_ctx *ctx, struct animation *animation);

/* use new animation when current animation is over, call callback when new
 * animation starts */
void animation_ctx_set_next_animation(
    struct animation_ctx *ctx,
    struct animation *animation,
    void (*next_animation_cb)(struct animation_ctx *, void *),
    void *cb_data);

/* call this to update animation context, delta is in seconds */

// API_CALL animation_ctx_update ANIMATION_CTX FLOAT
void animation_ctx_update(struct animation_ctx *ctx, float delta);

// API_CALL animation_ctx_get_frame ANIMATION_CTX RETURNS_FRAME
struct frame *animation_ctx_get_frame(struct animation_ctx *ctx);

// API_CALL tileset_get_frame TILESET STR RETURNS_FRAME
struct frame *tileset_get_frame(struct tileset *tileset, char *name);

// API_CALL tileset_get_frame_by_id TILESET INT RETURNS_FRAME
struct frame *tileset_get_frame_by_id(struct tileset *tileset, int id);

// API_CALL tileset_color_mod TILESET INT INT INT INT
void tileset_color_mod(struct tileset *tileset, int r, int g, int b, int a);

SDL_Texture* load_texture(char *data, size_t len, int *w, int *h, struct collision_map **);
#endif
