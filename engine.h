#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include "vector.h"
#include "tileset.h"
#include "text.h"
#include "audio.h"

#define CTRL_KEY_UP 1
#define CTRL_KEY_DOWN 2
#define CTRL_KEY_LEFT 4
#define CTRL_KEY_RIGHT 8
#define CTRL_KEY_A 16
#define CTRL_KEY_B 32
#define CTRL_KEY_X 64
#define CTRL_KEY_Y 128
extern int glob_keys;
extern struct game_ctx* game_load(void);

#define KEY_IS_PRESSED(key) ((key) & glob_keys)

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern SDL_Renderer *glob_renderer;

struct game_ctx {
  char *game_name;
  void (*game_init)(void **data);
  void (*game_update)(void *data, float delta);
  void (*game_draw)(void *data);
  void (*game_on_key_down)(int key, void *data);
  void (*game_on_key_up)(int key, void *data);
};

extern struct game_ctx *glob_game_ctx;

#define screen_clear(r, g, b) \
  SDL_SetRenderDrawColor(glob_renderer, r, g, b, 255); \
  SDL_RenderClear(glob_renderer);

void engine_quit(void);

