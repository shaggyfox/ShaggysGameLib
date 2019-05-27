#include <stdarg.h>

#define USE_INTERFACE 1
#include "scheme.h"
#include "scheme-private.h"

#undef cons

pointer scheme_easy_call(scheme *sc, char *cmd, ...);
char *scheme_get_string(scheme *sc, pointer *value, char **err);
long int scheme_get_integer(scheme *sc, pointer *value, char **err);
float scheme_get_float(scheme *sc, pointer *value, char **err);
SDL_Rect *scheme_get_SDL_Rect(scheme *sc, pointer *value, char **err);

struct tileset *scheme_get_tileset(scheme *sc, pointer *value, char **err);
pointer scheme_tileset_to_pointer(scheme *sc, struct tileset *ts);

struct frame *scheme_get_frame(scheme *sc, pointer *value, char **err);
pointer scheme_frame_to_pointer(scheme *sc, struct frame *fr);

struct animation *scheme_get_animation(scheme *sc, pointer *value, char **err);
pointer scheme_animation_to_pointer(scheme *sc, struct animation *anim);

struct animation_ctx *scheme_get_animation_ctx(scheme *sc, pointer *value, char **err);
pointer scheme_animation_ctx_to_pointer(scheme *sc, struct animation_ctx *anim_ctx);

pointer scheme_int_to_pointer(scheme *sc, int i);
