#include "engine.h"
#include "heatmap.h"

static heatmap *my_heatmap = NULL;


static void red_to_green(float f)
{
  f = 1.0 - f;
  if (f < 0) {
    f = 0.0;
  } else if(f > 1) {
    f = 1.0;
  }
  if (f < .5) {
    draw_color(255, 255 * 2 * f, 0, 255);
  } else {
    draw_color(255 - 255 * 2 * (f - .5), 255, 0, 255);
  }
}

static void draw_map()
{
  for (int i = 0; i <my_heatmap->length; ++i) {
    int x = i % my_heatmap->width;
    int y = i / my_heatmap->width;
    if (my_heatmap->avoid[i]) {
      draw_color(128, 128, 128, 255);
    } else {
      red_to_green(my_heatmap->data[i] / 15.0);
    }
    draw_fill_rect4(x * 10, y * 10, 10, 10);
  }
}



static void maptest_init(void **data)
{
  my_heatmap = heatmap_new(10, 10, 15);
  heatmap_set(my_heatmap, 0,0,0);
  for (int i = 3; i <= 7; ++i) {
    heatmap_set_avoid(my_heatmap, i, 3, 1);
    heatmap_set_avoid(my_heatmap, i, 7, 1);
    heatmap_set_avoid(my_heatmap, 3, i, 1);
    heatmap_set_avoid(my_heatmap, 7, i, 1);
  }
  heatmap_set_avoid(my_heatmap, 7, 7, 0);
  heatmap_update(my_heatmap, 0, NULL);
}

static void maptest_update(void *data, float delta)
{
}

static void on_key_up(int key, void* data)
{
  engine_quit();
}

void maptest_draw(void *data)
{
  draw_color(0, 0, 0, 255);
  clear_screen();
  draw_color(0, 0, 255, 255);
  draw_map();
}

static struct game_ctx ret = {
  .game_init = maptest_init,
  .game_update = maptest_update,
  .game_draw = maptest_draw,
  .game_on_key_up = on_key_up,
};

ENGINE_MAIN(&ret)
