#include "engine.h"
#include "xm.h"

int mouse_x = 0;
int mouse_y = 0;

static void init(void **data)
{
  draw_color(0,0,0,255);
  text_color(150,150,150,255);
  audio_play_xm(audio_load_xm(xm,sizeof(xm)));
  audio_volume_xm(SDL_MIX_MAXVOLUME/1.0);
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
  clear_screen();
  text_use_font(FONT_TINY);
  text_printf(0,0, "%03i,%03i", mouse_x, mouse_y);
  text_use_font(FONT_DEFAULT);
  text_printf(0,10, "%03i,%03i", mouse_x, mouse_y);
}

static void key_up(int key, void *data)
{
  engine_quit();
}

static struct game_ctx ctx = {
  .screen_width = 320,
  .screen_height = 240,
  .screen_scale = 3,
  .game_init = init,
  .game_update = update,
  .game_draw = draw,
  .game_on_key_up = key_up,
  .game_on_mouse_motion = motion,
  .game_on_mouse_up = NULL,
  .game_on_mouse_down = NULL,
};

ENGINE_MAIN(&ctx);
