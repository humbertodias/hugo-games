#include "audio.h"
#include <stdio.h>
#include <string.h>

static int mixer_open = 0;

int forest_audio_init(void)
{
  if (mixer_open)
    return 0;
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    printf("(Forest Error)SDL_mixer init failed: %s\n", Mix_GetError());
    return 1;
  }
  Mix_AllocateChannels(16);
  mixer_open = 1;
  return 0;
}

void forest_audio_quit(void)
{
  if (!mixer_open)
    return;
  Mix_HaltChannel(-1);
  Mix_CloseAudio();
  mixer_open = 0;
}

int forest_load_sound(ForestSound *sound, const char *path)
{
  memset(sound, 0, sizeof(*sound));
  sound->loop_channel = -1;
  sound->chunk = Mix_LoadWAV(path);
  if (!sound->chunk)
    return 1;
  return 0;
}

void forest_free_sound(ForestSound *sound)
{
  if (!sound)
    return;
  if (sound->loop_channel >= 0)
    Mix_HaltChannel(sound->loop_channel);
  if (sound->chunk)
    Mix_FreeChunk(sound->chunk);
  memset(sound, 0, sizeof(*sound));
}

void forest_play(ForestSound *sound)
{
  if (!sound || !sound->chunk)
    return;
  Mix_PlayChannel(-1, sound->chunk, 0);
}

int forest_play_loop(ForestSound *sound)
{
  if (!sound || !sound->chunk)
    return -1;
  sound->loop_channel = Mix_PlayChannel(-1, sound->chunk, -1);
  return sound->loop_channel;
}

void forest_stop_loop(ForestSound *sound)
{
  if (!sound || sound->loop_channel < 0)
    return;
  Mix_HaltChannel(sound->loop_channel);
  sound->loop_channel = -1;
}

void forest_pause_all(void)
{
  Mix_HaltChannel(-1);
}
