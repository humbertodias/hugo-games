#ifndef FOREST_GAME_H
#define FOREST_GAME_H

#include "types.h"

GameState process_forest(InputState state);
void render_forest(void);
void on_enter_forest(void);
int get_forest_score(void);

#endif
