typedef struct heatmap_s {
  int *data;
  int length;
  int width;
} heatmap;

#include <stdio.h>
void printmap(heatmap *map)
{
  for(int i=0;i<map->length;++i){
    if (i != 0 && i % map->width == 0) {
      printf("\n");
    }
    printf("%02i ", map->data[i]);
  }
  printf("\n");
}
#include <stdlib.h>
heatmap *new_map(int w, int h, int val) {
  heatmap *ret = malloc(sizeof(*ret));
  ret->data = malloc(w * h * sizeof(int));
  for(int i = 0; i < w * h; ++i) {
    ret->data[i] = val;
  }
  ret->length = w * h;
  ret->width = w;
  return ret;
}

void set_map(heatmap *map, int x, int y, int val)
{
  map->data[y * map->width + x] = val;
}


void update(heatmap *map)
{
  int pos = 0;
  int *c;
  int left;
  int mid;
  int right;
  int smalest;
  int updates = 0;

#define VAL_A (map->data[pos - map->width])
#define VAL_B (map->data[pos])
#define VAL_C (map->data[pos + map->width])
#define SMALEST3(a,b,c) (a) < (b) ? (a) < (c) ? (a) : (c) : (b) < (c) ? (b) : (c);
#define SMALEST2(a,b) (a) < (b) ? (a) : (b)
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
  left = 255; \
  mid = 255;  \
  right = GET_BC;

#define START_UR \
  left = GET_BC; \
  mid = 255; \
  right = 255;

#define START_L \
  left = 255; \
  mid = 255; \
  right = GET_ABC;

#define START_R \
  left = GET_ABC; \
  mid = 255; \
  right = 255;

#define MOVE_RIGHT \
  c = GET_B; \
  left = mid; \
  mid = right; \
  ++ pos; \

#define MOVE_LEFT\
  c = GET_B; \
  right = mid; \
  mid = left; \
  -- pos; \

#define UPDATE\
  smalest = SMALEST3(left, mid, right); \
  DOIT;

#define START_DL \
  left = 255; \
  mid = 255; \
  right = GET_AB;

#define START_DR \
  left = GET_AB; \
  mid = 255; \
  right = 255;

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
    right = 255;
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
      right = 255;
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
    right = 255;
    UPDATE;

    if (!updates) {
      break;
    }
    updates = 0;
    /* now backwards */
    
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
    left = 255;
    UPDATE;
    
    while (pos > map->width ) {
      printf("pos:%d\n", pos);
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
      left = 255;
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
    left = 255;
    UPDATE;
  } while(updates);

  printf("->%d\n", pos);
}

int main() {
  heatmap *map = new_map(10,10,99);
  set_map(map, 9, 9, 0);
  update(map);
  printmap(map);
}
