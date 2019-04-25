#ifndef text_h
#define text_h
void text_color(int r, int g, int b, int a);
void text_print(int x, int y, char *data);
void text_printf(int x, int y, char *fmt, ...);
void text_dimensions(char *data, SDL_Rect *rect);
#endif

