#include "engine.h"
#include "xm.h"

int mouse_x = 0;
int mouse_y = 0;

static void init(void **data)
{
  draw_color(0,0,0,255);
  text_color(150,150,150,255);
  audio_play_xm(audio_load_xm(xm,sizeof(xm)));
  audio_volume_xm(SDL_MIX_MAXVOLUME/1);
}

static void update(void *data, float delta)
{
}

static void motion(int x, int y, void *data)
{
  mouse_x = x;
  mouse_y = y;
}

static void draw(void *data)
{
  screen_clear();
  text_use_font(FONT_TINY);
  text_printf(0,0, "%03i,%03i", mouse_x, mouse_y);
  text_use_font(FONT_DEFAULT);
  text_printf(0,10, "%03i,%03i", mouse_x, mouse_y);
}

static void key_up(int key, void *data)
{
  engine_quit();
}

struct game_ctx *game_load(void)
{
  static struct game_ctx ctx = {
    .game_init = init,
    .game_update = update,
    .game_draw = draw,
    .game_on_key_up = key_up,
    .game_on_mouse_motion = motion,
  };
  return &ctx;
}
