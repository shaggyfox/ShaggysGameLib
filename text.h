#ifndef text_h
#define text_h
#define FONT_DEFAULT 0
#define FONT_TINY 1
void text_use_font(int font);
void text_color(int r, int g, int b, int a);
// API_CALL draw_text INT INT STR
#define draw_text(x, y, data) text_print(x, y, data)
void text_print(int x, int y, char *data);
void text_printf(int x, int y, char *fmt, ...);
void text_dimensions(char *data, SDL_Rect *rect);
#endif

