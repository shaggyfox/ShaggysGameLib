#ifndef audio_h
#define audio_h
#include <sys/types.h>
/* audio */
void init_audio(void);
// API_CALL audio_load_sfx_from_file STR RETURNS_INT
int audio_load_sfx_from_file(char *file_name);

int audio_load_sfx(char *data, size_t len);
// API_CALL audio_play_sfx INT
void audio_play_sfx(int id);
// API_CALL audio_stop_sfx
void audio_stop_sfx(void);

int audio_load_xm(char *data, size_t len);
// API_CALL audio_load_xm_from_file STR RETURNS_INT
int audio_load_xm_from_file(char *file_name);
// API_CALL audio_play_xm INT
void audio_play_xm(int id);
// API_CALL audio_stop_xm
void audio_stop_xm(void);
// API_CALL audio_volume_xm INT
void audio_volume_xm(int volume);
// API_CALL audio_volume_sfx INT
void audio_volume_sfx(int volume);
#endif
