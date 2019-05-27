#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h> /* XXX to be removed */
#include <string.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>

#include "helper.h"

#include "vector.h"
#include "tileset.h"
#include "text.h"
#include "audio.h"
#include "draw.h"

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

// API_CALL UP_KEY_PRESSED RETURNS_BOOL
#define UP_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_UP)
// API_CALL DOWN_KEY_PRESSED RETURNS_BOOL
#define DOWN_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_DOWN)
// API_CALL LEFT_KEY_PRESSED RETURNS_BOOL
#define LEFT_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_LEFT)
// API_CALL RIGHT_KEY_PRESSED RETURNS_BOOL
#define RIGHT_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_RIGHT)
// API_CALL A_KEY_PRESSED RETURNS_BOOL
#define A_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_A)
// API_CALL B_KEY_PRESSED RETURNS_BOOL
#define B_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_B)
// API_CALL X_KEY_PRESSED RETURNS_BOOL
#define X_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_X)
// API_CALL Y_KEY_PRESSED RETURNS_BOOL
#define Y_KEY_PRESSED() KEY_IS_PRESSED(CTRL_KEY_Y)

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern SDL_Renderer *glob_renderer;

struct game_ctx {
  int screen_width;
  int screen_height;
  int screen_scale;
  char *game_name;
  void (*game_load)(void **data);
  void (*game_init)(void **data);
  void (*game_update)(void *data, float delta);
  void (*game_draw)(void *data);
  void (*game_on_key_down)(int key, void *data);
  void (*game_on_key_up)(int key, void *data);
  void (*game_text_input)(char *text, void* data);
  void (*game_on_quit)(void *data);
  void (*game_on_mouse_down)(int x, int y, int button, void *data);
  void (*game_on_mouse_up)(int x, int y, int button, void *data);
  void (*game_on_mouse_motion)(int x, int y, void *data);
};

extern struct game_ctx *glob_game_ctx;

int engine_main(int argc, char **argv);

// API_CALL engine_quit
void engine_quit(void);
#ifdef _WIN32
#include <windows.h>
#define ENGINE_MAIN(game_ctx) \
 int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR     lpCmdLine, int       nShowCmd) { \
    glob_game_ctx = game_ctx; \
    return engine_main(0, NULL); \
  }
#else
#define ENGINE_MAIN(game_ctx) \
 int main(int argc, char **argv) { \
    glob_game_ctx = game_ctx; \
    return engine_main(argc, argv); \
  }

#endif

#endif
