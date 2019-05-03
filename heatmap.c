#include "heatmap.h"
#include <stdio.h>
#include <stdlib.h>

static void heatmap_super_debug_printmap(heatmap *map, int *data)
{
  for(int i=0;i<map->length;++i){
    if (i != 0 && i % map->width == 0) {
      printf("\n");
    }
    printf("%02i ", data[i]);
  }
  printf("\n");
}

void heatmap_debug_printmap(heatmap *map)
{
  heatmap_super_debug_printmap(map, map->data);
}

void heatmap_debug_printescapemap(heatmap *map)
{
  heatmap_super_debug_printmap(map, map->escape);
}

/* Create new map with w/h dimensions and high_val as highest value */
heatmap *heatmap_new(int w, int h, int high_val) {
  heatmap *ret = calloc(1, sizeof(*ret));
  ret->data = malloc(w * h * sizeof(int));
  for(int i = 0; i < w * h; ++i) {
    ret->data[i] = high_val;
  }
  ret->avoid = calloc(1, w * h * sizeof(unsigned char));
  ret->length = w * h;
  ret->width = w;
  ret->height = h;
  ret->high_val = high_val;
  return ret;
}

void heatmap_reset(heatmap *map)
{
  for (int i = 0; i < map->length; ++i) {
    map->data[i] = map->high_val;
  }
}

void heatmap_reset_avoid(heatmap *map)
{
  for (int i = 0; i < map->length; ++i) {
    map->avoid[i] = 0;
  }
}

/* multiplicate heatmap with val */
void heatmap_mul(heatmap *map, float val)
{
  for (int i = 0; i < map->length; ++i) {
    if (map->avoid[i]) continue;
    map->data[i] *= val;
  }
}

void heatmap_set_avoid(heatmap *map, int x, int y, int val)
{
  map->avoid[y * map->width +x] = val;
}

void heatmap_set(heatmap *map, int x, int y, int val)
{
  map->data[y * map->width + x] = val;
}

static int heatmap_super_get(heatmap *map, int *data, int x, int y)
{
  if (x < 0 || y < 0 || x >= map->width || y >= map->height) {
    return map->high_val;
  }
  return data[y * map->width + x];
}

int heatmap_get(heatmap *map, int x, int y)
{
  return heatmap_super_get(map, map->data, x, y);
}

int heatmap_get_escape(heatmap *map, int x, int y)
{
  return heatmap_super_get(map, map->escape, x, y);
}

/* return directions(s) leading to the destination */
static int heatmap_get_direction_super(heatmap *map, int *data, int x, int y)
{
  int smalest = map->high_val;
  int tmp;
  int ret = HEATMAP_DIR_NONE;
  int current_dir = 1;
  /* upper_left, up, upper_right */
  for (int i = -1; i <= 1; ++i) {
    if ((tmp = heatmap_super_get(map, data, x + i, y - 1)) <= smalest) {
      if (tmp < smalest) {
        ret = 0;
        smalest = tmp;
      }
      ret |= current_dir;
    }
    current_dir *= 2;
  }
  if ((tmp = heatmap_super_get(map, data, x + 1, y)) <= smalest) {
    if (tmp < smalest) {
      ret = 0;
      smalest = tmp;
    }
    ret |= current_dir;
  }
  current_dir *= 2;
  /* down right, down, down left */
  for (int i = 1; i >= -1; --i) {
    if ((tmp = heatmap_super_get(map, data, x + i, y + 1)) <= smalest) {
      if (tmp < smalest) {
        smalest = tmp;
        ret = 0;
      }
      ret |= current_dir;
    }
    current_dir *= 2;
  }
  if ((tmp = heatmap_super_get(map, data, x - 1, y)) <= smalest) {
    if (tmp < smalest) {
      ret = 0;
    }
    ret |= current_dir;
  }

  return ret;
}

int heatmap_get_direction(heatmap *map, int x, int y)
{
  return heatmap_get_direction_super(map, map->data, x, y);
}

int heatmap_get_direction_escape(heatmap *map, int x, int y)
{
  return heatmap_get_direction_super(map, map->escape, x, y);
}

/* returns 0 when finisched */
static int heatmap_update_super(heatmap *map, int *data, int max_iterations, int *iterations_ret)
{
  int avoid = 0;
  int pos = 0;
  int *c;
  int left;
  int mid;
  int right;
  int smalest;
  int updates = 0;
  int iterations = 0;
  if (!data) {
    data = map->data;
  }

#define AVOID (map->avoid[pos])
#define VAL_A (data[pos - map->width])
#define VAL_B (data[pos])
#define VAL_C (data[pos + map->width])
#define SMALEST3(a,b,c) (a) < (b) ? (a) < (c) ? (a) : (c) : (b) < (c) ? (b) : (c);
#define GET_ABC SMALEST3(VAL_A, VAL_B, VAL_C)
#define GET_AB  VAL_A < VAL_B ? VAL_A : VAL_B
#define GET_BC  VAL_B < VAL_C ? VAL_B : VAL_C
#define GET_B &VAL_B
#define DOIT if (*c - 1 > smalest) {   \
               *c = smalest + 1; \
               updates += 1; \
               if (mid > *c) {mid = *c;} \
             }
#define START_UL \
  left = map->high_val; \
  mid = map->high_val;  \
  right = GET_BC;

#define START_UR \
  left = GET_BC; \
  mid = map->high_val; \
  right = map->high_val;

#define START_L \
  left = map->high_val; \
  mid = map->high_val; \
  right = GET_ABC;

#define START_R \
  left = GET_ABC; \
  mid = map->high_val; \
  right = map->high_val;

#define START_DL \
  left = map->high_val; \
  mid = map->high_val; \
  right = GET_AB;

#define START_DR \
  left = GET_AB; \
  mid = map->high_val; \
  right = map->high_val;

#define MOVE_RIGHT \
  c = GET_B; \
  left = mid; \
  mid = right; \
  avoid = AVOID; \
  ++ pos; \

#define MOVE_LEFT\
  c = GET_B; \
  right = mid; \
  mid = left; \
  avoid = AVOID; \
  -- pos; \

#define UPDATE\
  if (!avoid) { \
    smalest = SMALEST3(left, mid, right); \
    DOIT; \
  }


  do {
    updates = 0;
    pos = 0;

    /* ================== first line =================*/
    START_UL;
    MOVE_RIGHT;
    right = GET_BC;
    UPDATE;
    while (pos < map->width - 1) {
      MOVE_RIGHT;
      right = GET_BC;
      UPDATE;
    }
    MOVE_RIGHT;
    right = map->high_val;
    UPDATE;

    /* ================ lines up to one before the end =============== */
    int max_lines = map->length / map->width - 2;
    for (int line = 0; line < max_lines; ++line) {
      int end_of_line = pos + map->width - 1;
      /* begin */
      START_L;
      MOVE_RIGHT;
      right = GET_ABC;
      UPDATE;
      while (pos < end_of_line) {
        MOVE_RIGHT;
        right = GET_ABC;
        UPDATE;
      }
      /* last one of line */
      MOVE_RIGHT;
      right = map->high_val;
      UPDATE;
    }

    /* ======== last line ========= */
    START_DL
    MOVE_RIGHT;
    right = GET_AB;
    UPDATE;
    while (pos < map->length - 1) {
      MOVE_RIGHT;
      right = GET_AB;
      UPDATE;
    }
    MOVE_RIGHT;
    right = map->high_val;
    UPDATE;

    if (!updates) {
      break;
    }
    updates = 0;

    /* ============= now backwards =============== */
    /* lowest line */
    pos = map->length -1;
    START_DR;
    MOVE_LEFT;
    left = GET_AB;
    UPDATE;
    while(pos > map->length - map->width) {
      MOVE_LEFT;
      left = GET_AB;
      UPDATE;
    }
    MOVE_LEFT;
    left = map->high_val;
    UPDATE;
    while (pos > map->width ) {
      int end_of_line = pos - map->width +1;
      START_R;
      MOVE_LEFT;
      left = GET_ABC;
      UPDATE;
      while (pos > end_of_line) {
        MOVE_LEFT;
        left = GET_ABC;
        UPDATE;
      }
      MOVE_LEFT;
      UPDATE;
    }

    /* last line */
    START_UR;
    MOVE_LEFT;
    left = GET_BC;
    UPDATE;
    while (pos > 0) {
      MOVE_LEFT;
      left = GET_BC;
      UPDATE;
    }
    MOVE_LEFT;
    left = map->high_val;
    UPDATE;
    ++ iterations;
  } while(updates && iterations != max_iterations);
  if (iterations_ret) {
    *iterations_ret = iterations;
  }
  return !updates;
}

void heatmap_reset_escape(heatmap *map, float val)
{
  if (!map->escape) {
    map->escape = malloc(map->length * sizeof(int));
  }
  for (int i = 0; i < map->length; ++i) {
    if (map->avoid[i]) {
      map->escape[i] = map->high_val;
      continue;
    }
    map->escape[i] = map->data[i] * val;
  }
}

int heatmap_update_escape(heatmap *map, int max_iterations, int *iterations_ret)
{
  return heatmap_update_super(map, map->escape, max_iterations, iterations_ret);
}

int heatmap_update(heatmap *map, int max_iterations, int *iterations_ret)
{
  return heatmap_update_super(map, map->data, max_iterations, iterations_ret);
}
