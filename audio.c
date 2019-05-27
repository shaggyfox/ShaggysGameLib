#include <string.h>
#include "engine.h"
#include "contrib/ibxm.h"

#define DEFAULT_AUDIO_FREQ 48000

#define MAX_AUDIO_CHANNELS 4
static struct sfx_channel_s {
  int on;
  int id;
  Uint32 pos;
} glob_sfx_channels [MAX_AUDIO_CHANNELS];
static int glob_sfx_channel_pos = 0;

static struct sfx_list {
  Uint32 len;
  Uint8 *buffer;
} *glob_sfx_list = NULL;
static size_t glob_sfx_list_len = 0;

static struct module **glob_xm_list = NULL;
static size_t glob_xm_list_length = 0;
static struct replay *glob_xm_replay = NULL;
static int glob_xm_replay_id=-1;

static SDL_AudioSpec glob_audio_spec;

static int audio_buffer_get(Uint8 *dst, int dst_len)
{
  static int *music_buffer = NULL;
  static int music_buffer_len = 0;

  if (!music_buffer) {
    music_buffer = calloc(1, calculate_mix_buf_len(DEFAULT_AUDIO_FREQ) * sizeof(int));
  }
  int *music_buffer_pos = music_buffer;
  while (dst_len) {
    if (music_buffer_len) {
      /* use music from buffer */
      while (music_buffer_len && dst_len) {
        int current_sample = *(music_buffer_pos++); --music_buffer_len;
        *(dst++) = current_sample & 0xFF; --dst_len;
        *(dst++) = (current_sample >> 8) & 0xFF; --dst_len;
      }
    } else {
      /* music buffer empty. Generate new audio */
      music_buffer_len = replay_get_audio(glob_xm_replay, music_buffer) * 2;
      music_buffer_pos = music_buffer;
    }
  }
  if (music_buffer_len) {
    /* if we left some samples in music_buffer move them to
     * the buffer start position */
    memmove(music_buffer, music_buffer_pos, music_buffer_len * sizeof(int));
  }

  return 0;
}

static int glob_sfx_volume = SDL_MIX_MAXVOLUME / 2;
static int glob_xm_volume = SDL_MIX_MAXVOLUME / 2;

void audio_volume_xm(int volume)
{
  glob_xm_volume = volume;
}

void audio_volume_sfx(int volume)
{
  glob_sfx_volume = volume;
}

void MyAudioCallback(void*  userdata,
                       Uint8* stream,
                       int    len)
{
  static Uint8 *mix_buffer = NULL;
  static int mix_buffer_len = 0;
  if (len > mix_buffer_len) {
    mix_buffer_len = len;
    mix_buffer = realloc(mix_buffer, mix_buffer_len);
  }
  memset(stream, 0, len);
  if (glob_xm_replay) {
    audio_buffer_get(mix_buffer, len);
    SDL_MixAudioFormat(stream, mix_buffer, glob_audio_spec.format, len, glob_xm_volume);
  }
  for (int i = 0; i < MAX_AUDIO_CHANNELS; ++i) {
    struct sfx_channel_s *channel = &glob_sfx_channels[i];
    struct sfx_list *sfx = &glob_sfx_list[channel->id];

    if (!channel->on) {
      continue;
    }
    int left = sfx->len - channel->pos;
    if (left < len) {
      SDL_MixAudioFormat(stream, &sfx->buffer[channel->pos], glob_audio_spec.format, left, glob_sfx_volume);
      /* audio end */
      channel->on = 0;
    } else {
      SDL_MixAudioFormat(stream, &sfx->buffer[channel->pos], glob_audio_spec.format, len, glob_sfx_volume);
      channel->pos += len;
    }
  }
}
void init_audio(void)
{
  SDL_AudioSpec want;
  SDL_AudioDeviceID dev;

  SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
  want.freq = DEFAULT_AUDIO_FREQ;
  want.format = AUDIO_S16;
  want.channels = 2;
  want.samples = 2048;
  want.callback = MyAudioCallback; /* you wrote this function elsewhere -- see SDL_AudioSpec for details */

  dev = SDL_OpenAudioDevice(NULL, 0, &want, &glob_audio_spec, 0);
  if (dev == 0) {
      SDL_Log("Failed to open audio: %s", SDL_GetError());
  } else {
    SDL_PauseAudioDevice(dev, 0); /* start audio playing. */
  }
}

/* XXX EXPORT FUNCTIONS */

int audio_load_sfx(char *data, size_t in_len)
{
  glob_sfx_list = realloc(glob_sfx_list, sizeof(*glob_sfx_list) * (glob_sfx_list_len + 1));
  struct sfx_list *sfx = &glob_sfx_list[glob_sfx_list_len];
  SDL_RWops *src = SDL_RWFromConstMem(data, in_len);
  SDL_AudioSpec spec;
  SDL_LoadWAV_RW(src, 1, &spec, &sfx->buffer, &sfx->len);
  SDL_AudioCVT cvt;
  SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
                          glob_audio_spec.format, glob_audio_spec.channels, glob_audio_spec.freq);
  cvt.len = in_len;
  cvt.buf = realloc(sfx->buffer, cvt.len * cvt.len_mult);
  SDL_ConvertAudio(&cvt);
  sfx->len = cvt.len_cvt;
  sfx->buffer = realloc(cvt.buf, cvt.len_cvt);
  return glob_sfx_list_len++;
}

int audio_load_sfx_from_file(char *file_name)
{
  char *data = NULL;
  size_t data_len = 0;
  int ret = -1;

  if (NULL == (data = load_file(file_name, &data_len))) {
    printf("audio_load_sfx_from_file: error opening: %s", file_name);
  } else {
    ret = audio_load_sfx(data, data_len);
    free(data);
  }
  return ret;
}

void audio_play_sfx(int id)
{
  glob_sfx_channels[glob_sfx_channel_pos].on = 1;
  glob_sfx_channels[glob_sfx_channel_pos].id = id;
  glob_sfx_channels[glob_sfx_channel_pos].pos = 0;
  glob_sfx_channel_pos++;
  glob_sfx_channel_pos %= MAX_AUDIO_CHANNELS;
}

void audio_stop_sfx(void) {
  for (int i = 0; i < MAX_AUDIO_CHANNELS; ++i) {
    glob_sfx_channels[i].on = 0;
  }
}

int audio_load_xm(char *data, size_t len)
{
  char music_load_error_msg[64] = "";
  struct data mod_data = { (char*) data, len };
  glob_xm_list = realloc(glob_xm_list, sizeof(*glob_xm_list) * (glob_xm_list_length + 1));
  glob_xm_list[glob_xm_list_length] = module_load(&mod_data, (char*)&music_load_error_msg);
  return glob_xm_list_length ++;
}


int audio_load_xm_from_file(char *file_name)
{
  char *data = NULL;
  size_t data_len = 0;
  int ret = -1;

  if (NULL == (data = load_file(file_name, &data_len))) {
    printf("audio_load_sfx_from_file: error opening: %s", file_name);
  } else {
    ret = audio_load_xm(data, data_len);
    free(data);
  }
  return ret;
}

void audio_play_xm(int id) {
  if (glob_xm_replay_id == id) {
    return;
  }
  if (glob_xm_replay) {
    dispose_replay(glob_xm_replay);
  }
  glob_xm_replay = new_replay(glob_xm_list[id], DEFAULT_AUDIO_FREQ, 0);
  glob_xm_replay_id = id;
}
void audio_stop_xm(void)
{
  if (glob_xm_replay) {
    dispose_replay(glob_xm_replay);
    glob_xm_replay = NULL;
    glob_xm_replay_id = -1;
  }
}


/* END AUDIO */


