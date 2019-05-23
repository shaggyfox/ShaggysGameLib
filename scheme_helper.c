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
