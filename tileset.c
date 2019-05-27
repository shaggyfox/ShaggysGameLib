#include "engine.h"
#include "json_tk.h"
#include "tileset.h"

SDL_Texture* load_texture(char *data, size_t len, int *w, int *h, struct collision_map **c_map)
{
  SDL_RWops *data_rw = SDL_RWFromConstMem(data, len);
  SDL_Surface *surface = IMG_Load_RW(data_rw, 1);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(glob_renderer, surface);
  if (w && h) {
    *w = surface->w;
    *h = surface->h;
  }
  if (c_map) {
    *c_map = collision_map_from_surface(surface);
  }
  SDL_FreeSurface(surface);
  return texture;
}

struct animation *tileset_get_animation(struct tileset* tileset, char *name)
{
  for (int i = 0; i < tileset->tileset_animation_count; ++i) {
    if (!strcmp(name, tileset->tileset_animations[i].animation_name)) {
      return &tileset->tileset_animations[i];
    }
  }
  return NULL;
}

struct frame *tileset_get_frame(struct tileset *tileset, char *name)
{
  struct animation *anim = tileset_get_animation(tileset, name);
  if (anim) {
    return anim->animation_frames;
  }
  return NULL;
}

struct frame *tileset_get_frame_by_id(struct tileset *tileset, int id)
{
  if (id >= 0 && id < tileset->tileset_frame_count) {
    return &tileset->tileset_frames[id];
  }
  return NULL;
}

void animation_ctx_init(struct animation_ctx *ctx, struct animation *animation)
{
  ctx->animation_ctx_animation = animation;
  ctx->animation_ctx_timeleft = animation->animation_frames[animation->animation_begin].frame_duration;
  ctx->animation_ctx_pos = 0;
}

struct animation_ctx *animation_ctx_new(struct animation *animation)
{
  struct animation_ctx *ret = malloc(sizeof(*ret));
  animation_ctx_init(ret, animation);
  return ret;
}

void animation_ctx_set_animation(struct animation_ctx *ctx, struct animation *animation)
{
  animation_ctx_init(ctx, animation);
}

void animation_ctx_set_next_animation(
    struct animation_ctx *ctx,
    struct animation *animation,
    void (*next_animation_cb)(struct animation_ctx *, void *),
    void *cb_data)
{
  ctx->animation_ctx_next_animation = animation;
  ctx->animation_ctx_next_animation_cb =next_animation_cb;
  ctx->animation_ctx_next_animation_cb_data = cb_data;
}

void animation_ctx_update(struct animation_ctx *ctx, float delta)
{
  int begin = ctx->animation_ctx_animation->animation_begin;
  int end = ctx->animation_ctx_animation->animation_end;
  int len = end - begin;
  if (len > 0) {
    len += 1;
    ctx->animation_ctx_timeleft -= delta;
    if (ctx->animation_ctx_timeleft <= 0) {
      switch (ctx->animation_ctx_animation->animation_type) {
        case ANIMATION_TYPE_FORWARD:
          ctx->animation_ctx_pos = (ctx->animation_ctx_pos + 1) % len;
          break;
        case ANIMATION_TYPE_BACKWARD:
          break;
        case ANIMATION_TYPE_PINGPONG:
          if (ctx->animation_ctx_pingpong_direction == 0) {
            ctx->animation_ctx_pingpong_direction = 1;
          }
          if (ctx->animation_ctx_pingpong_direction == 1) {
            if(ctx->animation_ctx_pos >= len - 1) {
              ctx->animation_ctx_pingpong_direction = -1;
            }
          } else {
            if (ctx->animation_ctx_pos <= 0) {
              ctx->animation_ctx_pingpong_direction = 1;
            }
          }
          ctx->animation_ctx_pos += ctx->animation_ctx_pingpong_direction;
          break;
      }
      ctx->animation_ctx_timeleft = ctx->animation_ctx_animation->animation_frames[ctx->animation_ctx_pos].frame_duration;
    }
  }
}

struct frame* animation_ctx_get_frame(struct animation_ctx *ctx)
{
  return &ctx->animation_ctx_animation->animation_frames[ctx->animation_ctx_pos];
}

void tileset_color_mod(struct tileset *tileset, int r, int g, int b, int a)
{
  SDL_SetTextureColorMod(tileset->tileset_texture, r, g, b);
  SDL_SetTextureAlphaMod(tileset->tileset_texture, a);
}

/* XXX rotation is not supported here */
int frame_check_collision(int a_x, int a_y, struct frame* a_frame,
                          int b_x, int b_y, struct frame* b_frame)
{
  struct collision_map *c_map_a = a_frame->frame_tileset->tileset_collision_map;
  struct collision_map *c_map_b = b_frame->frame_tileset->tileset_collision_map;
  SDL_Rect *a_src = &a_frame->frame_src;
  SDL_Rect *b_src = &b_frame->frame_src;
  SDL_Rect a_rect = {a_x, a_y, a_src->w, a_src->h};
  SDL_Rect b_rect = {b_x, b_y, b_src->w, b_src->h};
  if (SDL_Rect_check_collision(&a_rect, &b_rect)) {
    if (collision_map_check_collision(
          c_map_a, c_map_b, &a_rect, &b_rect, a_src, b_src)) {
      return 1;
    }
  }
  return 0;
}

static void json_to_SDL_Rect(json_value *jv, SDL_Rect *rect)
{
  json_object_entry *values = jv->u.object.values;
  for (int i = 0; i <jv->u.object.length; ++i) {
    switch (*values[i].name) {
      case 'x':
        rect->x = values[i].value->u.integer;
        break;
      case 'y':
        rect->y = values[i].value->u.integer;
        break;
      case 'w':
        rect->w = values[i].value->u.integer;
        break;
      case 'h':
        rect->h = values[i].value->u.integer;
        break;
    }
  }
}

struct tileset *tileset_load_from_file(char *json_file_name)
{
  struct tileset *ret = NULL;
  char *json_data = NULL;
  char *image_data = NULL;
  do {
    char *image_filename = NULL;
    size_t json_data_len = 0;
    size_t image_data_len = 0;
    char error[128];
    static json_settings jst;
    /* load json file into memory */
    json_data = load_file(json_file_name, &json_data_len);
    json_value *jv = json_parse_ex(&jst, json_data, json_data_len, error);
    if (!jv) {
      printf("%s: %s\n",json_file_name, error);
      break;
    }
    json_value *meta = json_find(jv, "meta");
    /* get image file-name from json file */
    image_filename = json_find_string(meta, "image");
    if (!image_filename) {
      printf("%s: %s\n", "json_file_name", "meta/image missing");
      break;
    }
    /* we no longer need jv here */
    json_value_free(jv);
    /* load image file data */
    image_data = load_file(image_filename, &image_data_len);
    if (!image_data) {
      printf("%s: %s\n", "image_filename", "can not open");
      break;
    }
    ret = tileset_load(json_data, json_data_len, image_data, image_data_len);
  } while(0);
  /* cleanup */
  free(image_data);
  free(json_data);
  return ret;
}

struct tileset *tileset_load(char *json_data, size_t json_data_len, char *image_data, size_t image_data_len)
{
  static json_settings jst;
  struct tileset *ret = calloc(1, sizeof(*ret));
  char error[128];
  json_value *jv = json_parse_ex(&jst, json_data, json_data_len, error);
  if (!jv) {
    printf("%s: %s\n","<strdata>", error);
    exit(1);
  }
  json_value *frames = json_find(jv, "frames");
  json_value **frames_array = json_array_get_values(frames);
  int frames_length = json_array_get_length(frames);
  struct frame *my_frames = calloc(frames_length, sizeof(struct frame));
  for (int i = 0; i < frames_length; ++i) {
    json_value *frame_object = json_find(frames_array[i], "frame");
    json_to_SDL_Rect(frame_object, &my_frames[i].frame_src);
    my_frames[i].frame_duration = (float)json_find_int(frames_array[i], "duration") / 1000.0f;
    my_frames[i].frame_tileset = ret;
  }
  /* texture */
  ret->tileset_texture = load_texture(image_data, image_data_len, NULL, NULL, &ret->tileset_collision_map);
  /* animations */
  json_value *meta = json_find(jv, "meta");
  json_value *frame_tags = json_find(meta, "frameTags");
  int animations_length = json_array_get_length(frame_tags);
  json_value **frame_tags_array = json_array_get_values(frame_tags);
  struct animation *my_animations = calloc(animations_length, sizeof(struct animation));
  for (int i = 0; i < animations_length; ++i) {
    char *type_str = json_find_string(frame_tags_array[i], "direction");
    if (!strcmp(type_str, "forward")) {
      my_animations[i].animation_type = ANIMATION_TYPE_FORWARD;
    } else if(!strcmp(type_str, "backward")) {
      my_animations[i].animation_type = ANIMATION_TYPE_BACKWARD;
    } else if (!strcmp(type_str, "pingpong")) {
      my_animations[i].animation_type = ANIMATION_TYPE_PINGPONG;
    } else {
      my_animations[i].animation_type = ANIMATION_TYPE_FORWARD;
    }
    /* this is save because we never free jv */
    my_animations[i].animation_name = json_find_string(frame_tags_array[i], "name");
    my_animations[i].animation_begin = json_find_int(frame_tags_array[i], "from");
    my_animations[i].animation_end = json_find_int(frame_tags_array[i], "to");
    my_animations[i].animation_frames = &my_frames[my_animations[i].animation_begin];
    my_animations[i].animation_texture = ret->tileset_texture;
    my_animations[i].animation_tileset = ret;
  }
  ret->tileset_animations = my_animations;
  ret->tileset_frames = my_frames;
  ret->tileset_animation_count = animations_length;
  ret->tileset_frame_count = frames_length;

  return ret;
}
