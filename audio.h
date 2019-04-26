#ifndef audio_h
#define audio_h
#include <sys/types.h>
/* audio */
void init_audio(void);
int audio_load_sfx(char *data, size_t len);
void audio_play_sfx(int id);
void audio_stop_sfx(void);
int audio_load_xm(char *data, size_t len);
void audio_play_xm(int id);
void audio_stop_xm(void);
void audio_volume_xm(int volume);
void audio_volume_sfx(int volume);
#endif
