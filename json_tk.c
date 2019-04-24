#include "json_tk.h"
#include <string.h>

static json_value *intern_json_find(json_value *jv, char *name, int recursive)
{
  if (!jv) return NULL;
  json_value *ret = NULL;
  if (jv->type == json_object) {
    for (unsigned int i = 0; i < jv->u.object.length; ++i) {
      if (!strcmp(jv->u.object.values[i].name, name)) {
        ret = jv->u.object.values[i].value;
        break;
      } else if(recursive) {
        if (jv->u.object.values[i].value->type == json_object) {
          if ((ret = intern_json_find(jv->u.object.values[i].value, name, 1))) {
            break;
          }
        } else if (jv->u.object.values[i].value->type == json_array) {
          for (unsigned int j = 0; !ret && j < jv->u.object.values[i].value->u.array.length; ++j) {
            ret = intern_json_find(jv->u.object.values[i].value->u.array.values[j], name, 1);
          }
          if (ret) {
            break;
          }
        }
      }
    }
  } else if (jv->type == json_array && recursive) {
    for (unsigned int i = 0; i < jv->u.array.length; ++i) {
      if ((ret = intern_json_find(jv->u.array.values[i], name, 1))) {
        break;
      }
    }
  }
  return ret;
}

json_value *json_find(json_value *jv, char *name)
{
  return intern_json_find(jv, name, 0);
}
json_value *json_find_recursive(json_value *jv, char *name)
{
  return intern_json_find(jv, name, 1);
}

char *json_find_string(json_value *jv, char *name)
{
  if (!jv) return NULL;
  char *ret = NULL;
  json_value *obj = json_find(jv, name);
  if (obj && obj->type == json_string) {
    ret = obj->u.string.ptr;
  }
  return ret;
}

int json_find_int(json_value *jv, char *name)
{
  if (!jv) return 0;
  int ret = 0;
  json_value *obj = json_find(jv, name);
  if (obj && obj->type == json_integer) {
    ret = obj->u.integer;
  }
  return ret;
}

int json_get_type(json_value *jv)
{
  return jv->type;
}

int json_array_get_length(json_value *jv)
{
  return jv->u.array.length;
}

json_value** json_array_get_values(json_value *jv)
{
  return jv->u.array.values;
}

