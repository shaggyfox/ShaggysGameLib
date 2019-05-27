#include "engine.h"

#include "kongfont_json.h"
#include "kongfont_png.h"

#include "tinyfont_json.h"
#include "tinyfont_png.h"

static int text_initialized = 0;
static struct tileset *text_tileset = NULL;
static struct tileset *tiny_tileset = NULL;
static struct frame *text_map[128];
static struct frame *tiny_map[128];
static int font_x_spacing = 8;
static int font_y_spacing = 10;

static struct frame **current_map;
static struct tileset *current_tileset;

static struct tileset* text_initialize_int(struct frame **map,
                            char* json,
                            size_t json_size,
                            char *data,
                            size_t data_size)
{
  struct tileset *ret = tileset_load(json, json_size, data, data_size);
  for (unsigned char i = 0; i < 128; ++i) {
    char str[2] = {0,0};
    str[0] = i;
    struct frame *f = tileset_get_frame(ret, str);
    if (!f) {
      /* XXX */
      f = tileset_get_frame(ret, "?");
      if (!f) {
        f = tileset_get_frame(ret, "-");
      }
    }
    map[i] = f;
  }
  return ret;
}
void text_initialize(void)
{
  text_tileset = text_initialize_int(text_map,
      default_font_json, sizeof(default_font_json),
      default_font_data, sizeof(default_font_data));
  tiny_tileset = text_initialize_int(tiny_map,
      tiny_font_json, sizeof(tiny_font_json),
      tiny_font_data, sizeof(tiny_font_data));
  current_tileset = text_tileset;
  current_map = text_map;
  text_initialized = 1;
}

void text_use_font(int font)
{
  if (!text_initialized) {
    text_initialize();
    text_initialized = 1;
  }
  switch(font) {
    case FONT_DEFAULT:
      current_tileset = text_tileset;
      current_map = text_map;
      font_x_spacing = 8;
      font_y_spacing = 10;
      break;
    case FONT_TINY:
      current_tileset = tiny_tileset;
      current_map = tiny_map;
      font_x_spacing = 6;
      font_y_spacing = 8;
      break;
    default:
      break;
  }
}

void text_color(int r, int g, int b, int a)
{
  if (!text_initialized) {
    text_initialize();
    text_initialized = 1;
  }
  SDL_SetTextureColorMod(current_tileset->tileset_texture, r, g, b);
  SDL_SetTextureAlphaMod(current_tileset->tileset_texture, a);
}

void text_print(int in_x, int y, char *data)
{
  int x = in_x;
  if (!text_initialized) {
    text_initialize();
    text_initialized = 1;
  }
  for(int i = 0; data[i]; ++i) {
    char c = data[i];
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      x = in_x;
      y += font_y_spacing;
      continue;
    }
    draw_frame(x, y, current_map[c&127]);
    x += font_x_spacing;
  }
}

void text_dimensions(char *data, SDL_Rect *rect) {
  int c = 0;
  int w = 0;
  int h = font_y_spacing;
  for (int i = 0; data[i]; ++i) {
    c += 1;
    if (data[i] == '\n') {
      if (c > w) {
        w = c;
      }
      c = 0;
      h += font_y_spacing;
    }
  }
  if (c > w) {
    w = c;
  }
  if (w > 0) {
    w = w * font_x_spacing;
  }
  rect->w = w;
  rect->h = h;
}

#include <stdarg.h>
void text_printf(int x, int y, char *fmt, ...) {
  static char big_buffer[1024];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(big_buffer, sizeof(big_buffer), fmt, ap);
  text_print(x, y, big_buffer);
  va_end(ap);
}
