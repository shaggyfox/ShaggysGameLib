#include "engine.h"

#include "kongfont_json.h"
#include "kongfont_png.h"

static int text_initialized = 0;
static struct tileset *text_tileset = NULL;
static struct frame *text_map[128];
static void text_initialize(void)
{
  text_tileset = tileset_load((char*)default_font_json, sizeof(default_font_json),
                              (char*)default_font_data, sizeof(default_font_data));
  for (unsigned char i = 0; i < 128; ++i) {
    char str[2] = {0,0};
    str[0] = i;
    struct frame *f = tileset_get_frame(text_tileset, str);
    if (!f) {
      f = tileset_get_frame(text_tileset, "?");
    }
    text_map[i] = f;
  }
}

void text_color(int r, int g, int b, int a)
{
  if (!text_initialized) {
    text_initialize();
    text_initialized = 1;
  }
  SDL_SetTextureColorMod(text_tileset->tileset_texture, r, g, b);
  SDL_SetTextureAlphaMod(text_tileset->tileset_texture, a);
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
      y += 10;
      continue;
    }
    frame_draw(text_map[c&127], x, y, 0);
    x += 8; /* XXX */
  }
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
