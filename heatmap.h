typedef struct heatmap_s {
  int *data;
  int *escape; /* data for escape-map */
  unsigned char *avoid; /* e.g. unpassable */
  int length;
  int width;
  int height;
  int high_val;
} heatmap;

void heatmap_debug_printmap(heatmap *map);
heatmap *heatmap_new(int w, int h, int val);
void heatmap_mul(heatmap *map, float val);
void heatmap_set_avoid(heatmap *map, int x, int y, int val);
void heatmap_set(heatmap *map, int x, int y, int val);
int heatmap_update(heatmap *map, int max_iterations, int *iterations_ret);

void heatmap_debug_printescapemap(heatmap *map);
void heatmap_reset(heatmap *map);
void heatmap_reset_avoid(heatmap *map);
void heatmap_reset_escape(heatmap *map, float val);
int heatmap_get(heatmap *map, int x, int y);
int heatmap_get_escape(heatmap *map, int x, int y);
int heatmap_get_direction(heatmap *map, int x, int y);
int heatmap_get_direction_escape(heatmap *map, int x, int y);
int heatmap_update_escape(heatmap *map, int max_iterations, int *iterations_ret);

#define HEATMAP_DIR_NONE 0
#define HEATMAP_DIR_UL 1
#define HEATMAP_DIR_U  2
#define HEATMAP_DIR_UR 4
#define HEATMAP_DIR_R 8
#define HEATMAP_DIR_DR 16
#define HEATMAP_DIR_D 32
#define HEATMAP_DIR_DL 64
#define HEATMAP_DIR_L 128

