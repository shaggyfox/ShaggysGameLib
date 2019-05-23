#include "engine.h"
#include "scheme_helper.h"


/* -------- API WRAPPER ---------- */

#if 0
static void my_free(void *data)
{
  printf("cleanup %s\n",data);
  free(data);
}

static pointer scheme_test_obj(scheme *sc, pointer args)
{
  user_type obj = {
    .free = my_free,
    .data = strdup("testtest"),
    .type = 1,
  };
  return mk_user_type(sc, obj);
}
#endif


/* -------- GAME CTX ------------- */
void scheme_init_api(scheme *sc);

static void tinytest_init(void **data_p)
{
  scheme *sc = scheme_init_new();
  scheme_init_api(sc);
  scheme_set_output_port_file(sc, stdout);
  *data_p = sc;
  FILE *f = fopen("game.scm", "r");
  scheme_load_named_file(sc, f, "game.scm");
  fclose(f);
}

static void tinytest_update(void* data, float delta)
{
  scheme *sc = data;
  scheme_easy_call(sc, "game-update", mk_real(sc, delta), NULL);
  scheme_flush_sink(sc);
}

static void tinytest_draw(void *data)
{
  scheme *sc = data;
  scheme_easy_call(sc, "game-draw", NULL);
  scheme_flush_sink(sc);
}

static void tiny_mouse_motion(int x, int y, void *data)
{
  scheme *sc = data;
  scheme_easy_call(sc, "game-mouse-motion", mk_integer(sc, x), mk_integer(sc, y), NULL);
  scheme_flush_sink(sc);
}

static struct game_ctx ctx = {
  .game_init = tinytest_init,
  .game_update = tinytest_update,
  .game_draw = tinytest_draw,
  .game_on_mouse_motion = tiny_mouse_motion,
};

ENGINE_MAIN(&ctx);

#if 0
int main()
{
  scheme *sc = scheme_init_new();
  //sc->vptr->scheme_define(sc, sc->global_env, sc->vptr->mk_symbol(sc, "c_function"), sc->vptr->mk_foreign_func(sc, &c_function));
  sc->vptr->scheme_define(sc, sc->global_env, sc->vptr->mk_symbol(sc, "my_test"), sc->vptr->mk_foreign_func(sc, &s_my_test));
  sc->gc_verbose = 1;
  scheme_set_output_port_file(sc, stdout);
  scheme_load_string(sc, "(define value 0) (define (count) (set! value (+ value 1)) (display value) (newline) value)");
  while(1) {
    scheme_easy_call(sc, "my_test", mk_integer(sc, 3), mk_string(sc, "bla"), NULL);
    scheme_flush_sink(sc);
  }
}
#endif
