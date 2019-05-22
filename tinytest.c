#include "engine.h"
#include <stdarg.h>

#define USE_INTERFACE 1
#include "scheme.h"
#include "scheme-private.h"

#undef cons

pointer scheme_easy_call(scheme *sc, char *cmd, ...)
{
  pointer arg;
  pointer cur_cons;
  pointer eval_cons;
  cur_cons = eval_cons = sc->vptr->cons(sc, sc->vptr->mk_symbol(sc, cmd), sc->NIL);
  va_list va;
  va_start(va, cmd);
  while(NULL != (arg = va_arg(va, pointer))) {
    pointer new_cons = sc->vptr->cons(sc, arg, sc->NIL);
    sc->vptr->set_cdr(cur_cons, new_cons);
    cur_cons = new_cons;
  }
  va_end(va);
  return scheme_eval(sc, eval_cons);
}

char *scheme_get_string(scheme *sc, pointer *value)
{
  char * ret = NULL;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_string(v)) {
      ret = sc->vptr->string_value(v);
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  }
  return ret;
}

long int scheme_get_integer(scheme *sc, pointer *value)
{
  int ret = 0;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_integer(v)) {
      ret = sc->vptr->ivalue(v);
    } else if(sc->vptr->is_real(v)) {
      ret = sc->vptr->rvalue(v);
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  }
  return ret;
}

float scheme_get_float(scheme *sc, pointer *value)
{
  float ret = 0;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_real(v)) {
      ret = sc->vptr->rvalue(v);
    } else if(sc->vptr->is_integer) {
      ret = sc->vptr->ivalue(v);
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  }
  return ret;
}

void my_test(int num, char *str) {
  printf("got num :%d and str: %s\n", num, str);
}

pointer s_my_test(scheme *sc, pointer args)
{
  printf("jojo\n");
  if (sc->vptr->list_length(sc, args) != 2) {
    printf("parameter!\n");
    return sc->NIL;
  }
  int par1 = scheme_get_integer(sc, &args);
  char *par2 = scheme_get_string(sc, &args);
  my_test(par1, par2);
  return sc->NIL;
}

/* -------- API WRAPPER ---------- */

static pointer scheme_clear_screen(scheme *sc, pointer args)
{
  clear_screen();
  return sc->NIL;
}

static pointer scheme_draw_text(scheme *sc, pointer args)
{
  char *text = scheme_get_string(sc, &args);
  int x = scheme_get_integer(sc, &args);
  int y = scheme_get_integer(sc, &args);
  text_print(x, y, text);
  return sc->NIL;
}

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



/* -------- GAME CTX ------------- */
#define FUNCTION(name, function) \
        sc->vptr->scheme_define(sc, sc->global_env, \
          sc->vptr->mk_symbol(sc, name), \
          sc->vptr->mk_foreign_func(sc, &function))

static void tinytest_init(void **data_p)
{
  scheme *sc = scheme_init_new();

  FUNCTION("clear-screen", scheme_clear_screen);
  FUNCTION("draw-text", scheme_draw_text);
  FUNCTION("obj-test", scheme_test_obj);
  //FUNCTION("*error-hook*", s_my_test);
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
}


static struct game_ctx ctx = {
  .game_init = tinytest_init,
  .game_update = tinytest_update,
  .game_draw = tinytest_draw
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
