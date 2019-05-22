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

#define MAX_ARGS 12
static pointer intern_argv_array[12];


pointer* scheme_list_to_argv(scheme *sc, pointer args, int *argc_par)
{
  int argc = 0;
  while(args != sc->NIL) {
    pointer e = sc->vptr->pair_car(args);
    args = sc->vptr->pair_cdr(args);
    intern_argv_array[argc++] = e;
  }
  if (argc_par) {
    *argc_par = argc;
  }
  return intern_argv_array;
}

pointer c_function(scheme *sc, pointer args)
{
  int argc = 0;
  pointer *argv = NULL;
  argv = scheme_list_to_argv(sc, args, &argc);
  for( int i = 0; i < argc; ++i) {
    switch (type(argv[i])) {
      case T_STRING:
        printf("%s\n", sc->vptr->string_value(argv[i]));
        break;
      case T_NUMBER:
        printf("%.2f\n", sc->vptr->rvalue(argv[i]));
        break;
      default:
        printf("got %p\n", argv[i]);
        break;
    }
  }
  return sc->NIL;
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
      printf("is integer\n");
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

int main()
{
  scheme *sc = scheme_init_new();
  sc->vptr->scheme_define(sc, sc->global_env, sc->vptr->mk_symbol(sc, "c_function"), sc->vptr->mk_foreign_func(sc, &c_function));
  sc->vptr->scheme_define(sc, sc->global_env, sc->vptr->mk_symbol(sc, "my_test"), sc->vptr->mk_foreign_func(sc, &s_my_test));
  sc->gc_verbose = 1;
  scheme_set_output_port_file(sc, stdout);
  scheme_load_string(sc, "(define value 0) (define (count) (set! value (+ value 1)) (display value) (newline) value)");
  while(1) {
    scheme_easy_call(sc, "c_function", 
        mk_string(sc, "blabla"),
        mk_integer(sc, 1),
        mk_real(sc, 3.3), NULL);
    scheme_easy_call(sc, "my_test", mk_integer(sc, 3), mk_string(sc, "bla"), NULL);
    //scheme_apply0(sc, "c_function");
    //scheme_eval(sc, cmd);
    //sc->vptr->set_car(sc->sink, sc->NIL);
    scheme_flush_sink(sc);
  }
}
