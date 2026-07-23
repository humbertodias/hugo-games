#ifndef FOREST_AUDIO_H
#define FOREST_AUDIO_H

#include "types.h"

#define FOREST_SOUND_READY(s) ((s).chunk != NULL)

int forest_audio_init(void);
void forest_audio_quit(void);
int forest_load_sound(ForestSound *sound, const char *path);
void forest_free_sound(ForestSound *sound);
void forest_play(ForestSound *sound);
int forest_play_loop(ForestSound *sound);
void forest_stop_loop(ForestSound *sound);
void forest_pause_all(void);

#endif
