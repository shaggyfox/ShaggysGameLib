#include "engine.h"


#include "ball_png.h"
#include "ball_json.h"
#include "xm.h"
#include <stdio.h>
struct frame *ball;

int music_id = 0;

void test_init(void **data)
{
  struct tileset *my_tileset = tileset_load(ball_json,sizeof(ball_json), ball_png, sizeof(ball_png));
  ball = tileset_get_frame(my_tileset, "ball");
  music_id = audio_load_xm(xm, sizeof(xm));
  audio_play_xm(music_id);
}

  static double r = 0;
void test_update(void *data, float delta)
{
  r += 360.0 * delta;
}

void red_to_green(float f)
{
  if (f < .5) {
    draw_color(255, 255 * 2 * f, 0, 255);
  } else {
    draw_color(255 - 255 * 2 * (f - .5), 255, 0, 255);
  }
}

void test_draw(void *data)
{
  char *text = "Hello World\nThis is a Test";
  draw_color(0,0,0,255);
  screen_clear();
  draw_color(255,0,0,255);
  draw_line(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
  draw_color(255,255,0,255);
  frame_draw_ex(ball, 10, 10, r, 0);
  /*SDL_Rect text_rect = {0,0,0,0};
  text_dimensions(text, &text_rect);
  draw_fill_rect(&text_rect);
  text_color(0,0,0,255);
  text_print(0, 0, text);*/
  for (int i = 0; i < 10; ++i) {
    red_to_green(i / 10.0);
    draw_fill_rect4(i*20, 100, 20, 10);
  }
}

void test_on_key_down(int key, void *data)
{
  //printf("->%c\n", key);
  if (key == 'q') {
    engine_quit();
  }
}

void test_on_mouse_button_down(int button, int x, int y, void* data)
{
  //printf("mouse button down at %d,%d\n", x, y);
}

void test_on_mouse_button_up(int button, int x, int y, void *data)
{
  //printf("mouse button up at %d,%d\n", x, y);
}

struct game_ctx *game_load(void)
{
  static struct game_ctx ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.game_name = "test";
  ctx.game_init = test_init;
  ctx.game_update = test_update;
  ctx.game_draw = test_draw;
  ctx.game_on_key_down = test_on_key_down;
  ctx.game_on_mouse_down = test_on_mouse_button_down;
  ctx.game_on_mouse_up = test_on_mouse_button_up;
  return &ctx;
}
