#include "contrib/json.h"

json_value *json_find(json_value *jv, char *name);
json_value *json_find_recursive(json_value *jv, char *name);
char *json_find_string(json_value *jv, char *name);
int json_find_int(json_value *jv, char *name);
int json_get_type(json_value *jv);
int json_array_get_length(json_value *jv);
json_value** json_array_get_values(json_value *jv);
