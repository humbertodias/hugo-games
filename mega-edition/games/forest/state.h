#ifndef FOREST_STATE_H
#define FOREST_STATE_H

#include "types.h"

double get_state_time(StateMetadata *metadata);
int get_frame_index(StateMetadata *metadata);
int get_frame_index_fast(StateMetadata *metadata);
double get_game_time(void);
bool one_shot(StateMetadata *metadata, double delta, int idx);
bool every(StateMetadata *metadata, double delta, int idx, double offset);
void reset_state(StateMetadata *metadata);

#endif
