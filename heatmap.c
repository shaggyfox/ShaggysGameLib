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
               if (mid > *c) {mid = *c;} \
             }

void update(heatmap *map)
{
  int pos = 0;
  int *c;
  int left;
  int mid;
  int right;
  int smalest;

  /* ================== first line =================*/
  c = GET_B;
  left = 255;
  mid = GET_BC; ++pos;
  right = GET_BC;

  smalest = SMALEST2(mid, right);
  DOIT;
  while (pos < map->width - 1) {
    c = GET_B;
    left = mid;
    mid = right;
    ++pos;
    right = GET_BC;
    smalest = SMALEST3(left, mid, right);
    DOIT;
  }

  left = mid;
  mid = right;
  c = GET_B;
  ++pos;
  smalest = SMALEST2(mid, left);
  DOIT;

  /* ================ lines up to one before the end =============== */
  int max_lines = map->length / map->width - 2;
  for (int line = 0; line < max_lines; ++line) {
    int end_of_line = pos + map->width - 1;
    /* begin */
    c = GET_B; /* get center-point */
    left = 255; /* left is 'empty' */
    mid = GET_ABC; ++pos;
    right = GET_ABC;
    smalest = SMALEST2(mid, right);
    DOIT;
    while (pos < end_of_line) {
      c = GET_B;
      left = mid;
      mid = right;
      ++pos;
      right = GET_ABC;
      smalest = SMALEST3(left, mid, right);
      DOIT;
    }
    /* last one of line */
    left = mid;
    mid = right;
    c = GET_B;++pos;
    smalest = SMALEST2(mid, left);
    DOIT;
  }

  /* ======== last line ========= */
  c = GET_B;
  left = 255;
  mid = GET_AB; ++pos;
  right = GET_AB;
  smalest = SMALEST2(mid, right);
  DOIT;
  while (pos < map->length - 1) {
    c = GET_B;
    left = mid;
    mid = right;
    ++pos;
    right = GET_AB;
    smalest = SMALEST3(left, mid, right);
    DOIT;
  }
  left = mid;
  mid = right;
  c = GET_B;
  ++pos;
  smalest = SMALEST2(mid, left);
  DOIT;
}

int main() {
  heatmap *map = new_map(10,10,99);
  set_map(map, 0, 0, 0);
  update(map);
  printmap(map);
}
