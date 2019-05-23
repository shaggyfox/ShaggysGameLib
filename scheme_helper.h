#include <stdarg.h>

#define USE_INTERFACE 1
#include "scheme.h"
#include "scheme-private.h"

#undef cons

pointer scheme_easy_call(scheme *sc, char *cmd, ...);
char *scheme_get_string(scheme *sc, pointer *value, char **err);
long int scheme_get_integer(scheme *sc, pointer *value, char **err);
float scheme_get_float(scheme *sc, pointer *value, char **err);

