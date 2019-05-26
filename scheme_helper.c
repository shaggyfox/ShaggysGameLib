#include "engine.h"
#include "scheme_helper.h"

pointer scheme_easy_call(scheme *sc, char *cmd, ...)
{
  pointer arg;
  pointer cur_cons;
  pointer eval_cons;
  pointer sym = sc->vptr->mk_symbol(sc, cmd);
  if (!scheme_symbol_is_global_defined(sc, sym)) {
    /* silently ignore undefined functions */
    return sc->NIL;
  }
  cur_cons = eval_cons = sc->vptr->cons(sc, sym, sc->NIL);
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

char *scheme_get_string(scheme *sc, pointer *value, char **err)
{
  char * ret = NULL;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_string(v)) {
      ret = sc->vptr->string_value(v);
    } else {
      *err = "wrong argument: string expected";
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  } else if(err){
    *err = "missing argument: string expected";
  }
  return ret;
}


long int scheme_get_integer(scheme *sc, pointer *value, char **err)
{
  int ret = 0;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_integer(v)) {
      ret = sc->vptr->ivalue(v);
    } else if(sc->vptr->is_real(v)) {
      ret = sc->vptr->rvalue(v);
    } else if (*err){
      *err = "wrong argument: number expected";
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  } else if(*err){
    *err = "missing argument: number expected";
  }
  return ret;
}

float scheme_get_float(scheme *sc, pointer *value, char **err)
{
  float ret = 0;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_real(v)) {
      ret = sc->vptr->rvalue(v);
    } else if(sc->vptr->is_integer) {
      ret = sc->vptr->ivalue(v);
    } else if (*err) {
      *err = "wrong argument: number expected";
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  } else if (*err) {
    *err = "missing argument: number expected";
  }
  return ret;
}
static SDL_Rect rect_cache[10];
static int rect_cache_pos = 0;
SDL_Rect *scheme_get_SDL_Rect(scheme *sc, pointer *value, char **err)
{
  SDL_Rect *ret = NULL;
  if (*value != sc->NIL) {
    pointer v = sc->vptr->pair_car(*value);
    if (sc->vptr->is_vector(v)) {
      if ( 4 == sc->vptr->vector_length(v)) {
        /* vector content is not checked or 'number' */
        rect_cache[rect_cache_pos].x = sc->vptr->ivalue(sc->vptr->vector_elem(v, 0));
        rect_cache[rect_cache_pos].y = sc->vptr->ivalue(sc->vptr->vector_elem(v, 1));
        rect_cache[rect_cache_pos].w = sc->vptr->ivalue(sc->vptr->vector_elem(v, 2));
        rect_cache[rect_cache_pos].h = sc->vptr->ivalue(sc->vptr->vector_elem(v, 3));
        ret = &rect_cache[rect_cache_pos];
        rect_cache_pos = (rect_cache_pos + 1) % 10;
      } else if (*err) {
        *err = "wrong argumend: vector length == 4 expected";
      }
    } else {
      *err = "wrong argument: vector expected\n";
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  } else if (*err) {
    *err = "missing argument: rect or vector expected";
  }
  return ret;
}
enum {
  USER_TYPE_TILESET
};

pointer scheme_tileset_to_pointer(scheme *sc, struct tileset *ts)
{
  user_type my_user_type = {
    .data = ts,
    .type = USER_TYPE_TILESET,
    .free = free
  };
  my_user_type.data = ts;
  my_user_type.type = USER_TYPE_TILESET;
  my_user_type.free = free; /* XXX tileset needs a proper free function */
  return mk_user_type(sc, my_user_type);
}

struct tileset *scheme_get_tileset(scheme *sc, pointer *value, char **err)
{
  struct tileset *ret = NULL;
  if (*value == sc->NIL) {
    *err = "missing value: expected user_type\n";
  } else {
    if (!sc->vptr->is_user) {
      *err = "value with wrong type: expected user_type\n";
    } else {
      ret = sc->vptr->uvalue(*value);
    }
    /* move to next element */
    *value = sc->vptr->pair_cdr(*value);
  }
  return ret;
}
