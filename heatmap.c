typedef struct heatmap_s {
  int *data;
  int length;
  int width;
} heatmap;

#define VAL_A (map->data[pos - map->width])
#define VAL_B (map->data[pos])
#define VAL_C (map->data[pos - map->width])
#define SMALEST3(a,b,c) (a) < (b) ? (a) < (c) ? (a) : (c) : (b) < (c) ? (b) : (c);
#define SMALEST2(a,b) (a) < (b) ? (a) : (b)
#define GET_ABC SMALEST3(VAL_A, VAL_B, VAL_C)
#define GET_AB  VAL_A < VAL_B ? VAL_A : VAL_B
#define GET_BC  VAL_B < VAL_C ? VAL_B : VAL_C
#define GET_B &VAL_B

void update(heatmap *map)
{
  int pos = 0;
  int *c = GET_B;
  int left = 255;
  int mid = GET_BC; ++pos;
  int right = GET_BC; ++pos;

  int smalest = mid > right ? right : mid;
  if (*c - 1 > smalest) {
    mid = *c = smalest +1;
  }

  while (pos < map->width - 1) {
    c = GET_B;
    left = mid;
    mid = right;
    right = GET_BC; ++pos;
    smalest = SMALEST3(left, mid, right);
    if (*c - 1 > smalest) {
      mid = *c = smalest + 1;
    }
  }
  left = mid;
  mid = right;
  c = GET_B;
  smalest = SMALEST2(mid, left);
  if (*c -1 > smalest) {
    *c = smalest + 1;
  }
}
