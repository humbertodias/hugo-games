#ifndef FOREST_DRAW_H
#define FOREST_DRAW_H

#include "types.h"

extern SDL_Surface *forest_screen;

int forest_anim_frame_count(ForestAnim *anim);
int forest_cgf_width(cgfinfo *info);
int forest_cgf_height(cgfinfo *info);
void forest_draw_rect_fill(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void forest_draw_rect_outline(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void forest_clear_screen(void);
void forest_blit_surface(SDL_Surface *src, int x, int y);
void forest_draw_cgf_at(cgfinfo *info, Uint32 frame, int x, int y, Uint32 tint);
void forest_draw_cgf_digit_white(cgfinfo *info, Uint32 frame, int x, int y);
void forest_draw_cgf_sheet_digit(cgfinfo *info, int digit, int x, int y);
void forest_draw_anim_frame(ForestAnim *anim, int local_frame, int x, int y);
void forest_draw_anim_full(ForestAnim *anim, int local_frame);
void forest_draw_sync_cgf(ForestAnim *anim, oosinfo *sync, int tick, int x, int y);
void forest_draw_sync_til(ForestAnim *anim, oosinfo *sync, int tick);

#endif
