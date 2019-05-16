#include "engine.h"
#include <string.h>

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#endif

int glob_keys = 0;

int SCREEN_WIDTH = 512; //576;
int SCREEN_HEIGHT = 300; //324;

struct game_ctx *glob_game_ctx = NULL;
struct SDL_Renderer *glob_renderer = NULL;
struct SDL_Window *glob_window = NULL;
int glob_running = 1;

void engine_quit(void) {

  glob_running = 0;
#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#endif
}

void run_loop(void* data)
{
  static unsigned int last_update = 0;
  SDL_Event event;
  /* Events */
  /* XXX add mouse input */
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_QUIT:
        /* XXX */
        if (glob_game_ctx->game_on_quit) {
          glob_game_ctx->game_on_quit(data);
        } else {
          engine_quit();
        }
        break;
      case SDL_KEYDOWN:
        if (event.key.repeat) {
          break;
        }
        if (glob_game_ctx->game_on_key_down) {
          glob_game_ctx->game_on_key_down(event.key.keysym.sym, data);
        }
        switch(event.key.keysym.sym) {
          case SDLK_UP:
            glob_keys |= CTRL_KEY_UP;
            break;
          case SDLK_DOWN:
            glob_keys |= CTRL_KEY_DOWN;
            break;
          case SDLK_LEFT:
            glob_keys |= CTRL_KEY_LEFT;
            break;
          case SDLK_RIGHT:
            glob_keys |= CTRL_KEY_RIGHT;
            break;
        }
        break;
      case SDL_KEYUP:
        if (glob_game_ctx->game_on_key_up) {
          glob_game_ctx->game_on_key_up(event.key.keysym.sym, data);
        }
        switch(event.key.keysym.sym) {
          case SDLK_UP:
            glob_keys &= ~CTRL_KEY_UP;
            break;
          case SDLK_DOWN:
            glob_keys &= ~CTRL_KEY_DOWN;
            break;
          case SDLK_LEFT:
            glob_keys &= ~CTRL_KEY_LEFT;
            break;
          case SDLK_RIGHT:
            glob_keys &= ~CTRL_KEY_RIGHT;
            break;
        }
        break;
      case SDL_TEXTINPUT:
        if (glob_game_ctx->game_text_input) {
          glob_game_ctx->game_text_input((char*)event.text.text, data);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (glob_game_ctx->game_on_mouse_up) {
          glob_game_ctx->game_on_mouse_up(event.button.button,
                                             event.button.x,
                                             event.button.y,
                                             data);
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (glob_game_ctx->game_on_mouse_down) {
          glob_game_ctx->game_on_mouse_down(event.button.button,
                                             event.button.x,
                                             event.button.y,
                                             data);
        }
        break;
      case SDL_MOUSEMOTION:
        if (glob_game_ctx->game_on_mouse_motion) {
          glob_game_ctx->game_on_mouse_motion(event.motion.x,
                                              event.motion.y,
                                              data);
        }
        break;
      default:
        break;
    }
  }

  unsigned int new_update = SDL_GetTicks();
  glob_game_ctx->game_update(data, (float)(new_update - last_update) / 1000.0f);
  last_update = new_update;
  glob_game_ctx->game_draw(data);

  SDL_RenderPresent(glob_renderer);
}

int engine_main(int argc, char *argv[])
{
  int use_acceleration = 1;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "noaccel")) {
      use_acceleration = 0;
    }
  }
  //glob_game_ctx = game_load();
  /* XXX call game_init first */
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  init_audio();
  /* ... and then create window based on init 'output' */
  glob_window = SDL_CreateWindow(glob_game_ctx->game_name ? glob_game_ctx->game_name : "game",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH * 2, SCREEN_HEIGHT *2, 0);

  if (use_acceleration) {
    glob_renderer = SDL_CreateRenderer(glob_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!glob_renderer) {
      printf("DEBUG: falling back to non-accelerated mode\n");
      glob_renderer = SDL_CreateRenderer(glob_window, -1, SDL_RENDERER_SOFTWARE);
    }
  } else {
    glob_renderer = SDL_CreateRenderer(glob_window, -1, SDL_RENDERER_SOFTWARE);
  }
  SDL_RenderSetLogicalSize(glob_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  void *data = NULL;
  glob_game_ctx->game_init(&data);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(run_loop, data, -1, 1);
#else
  while (glob_running) {
    run_loop(data);
  }
#endif
  SDL_DestroyRenderer(glob_renderer);
  SDL_DestroyWindow(glob_window);
  SDL_Quit();
  return 0;
}
