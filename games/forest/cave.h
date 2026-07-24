#ifndef FOREST_CAVE_H
#define FOREST_CAVE_H

#include "types.h"

GameState process_cave(InputState state);
void render_cave(void);
void on_enter_cave(void);
void set_cave_score(int score);

#endif
