#include "types.h"
#include "draw.h"
#include "audio.h"
#include <stdbool.h>
#include "state.h"
#include "cave.h"

typedef struct {
    // Cave game data
    int cave_selected_rope;  // 0, 1, or 2 for ropes 3, 6, 9
    int cave_win_type;       // 0=bird, 1=leaves, 2=ropes
    int score;               // Current score
    int rolling_score;       // Score for display animation
} CaveContext;

typedef enum {
    STATE_CAVE_NONE,
    STATE_CAVE_CLIMBING,
    STATE_CAVE_FAMILY_CAGE_OPENS,
    STATE_CAVE_FAMILY_HAPPY,
    STATE_CAVE_GOING_ROPE,
    STATE_CAVE_LOST,
    STATE_CAVE_LOST_SPRING,
    STATE_CAVE_SCYLLA_LOST,
    STATE_CAVE_SCYLLA_SPRING,
    STATE_CAVE_TALKING_BEFORE_CLIMB,
    STATE_CAVE_WAITING_BEFORE_TALKING,
    STATE_CAVE_WAITING_INPUT,
    STATE_CAVE_END
} CaveState;

static CaveContext cave_ctx = {0};
static CaveState current_cave_state = STATE_CAVE_WAITING_BEFORE_TALKING;
static bool sounding_score = false;
static int cave_score_counter_id = -1;
static StateMetadata state_metadata;

void render_cave_scoreboard(void) {
    if (cave_ctx.rolling_score < cave_ctx.score) {
        int diff = cave_ctx.score - cave_ctx.rolling_score;
        int step = diff > 10 ? 10 : diff;
        cave_ctx.rolling_score += step;
    }

    int value = cave_ctx.rolling_score;
    int thousands = value / 1000;
    int hundreds = (value % 1000) / 100;
    int tens = (value % 100) / 10;
    int ones = value % 10;

    // Render score digits using cave score font
    int x_score = 243;
    int y_score = 203;
    int x_space = 16;

    /* SCORE.cgf frame 10 = dark score plate; covers orange STAIRS TIL slot. */
    forest_draw_cgf_at(&textures.cave_score_font, 10, 234, 193, 1);
    forest_draw_rect_fill(272, 201, 38, 28, 11, 11, 11, 255);

    forest_draw_cgf_digit_white(&textures.cave_score_font, (Uint32)thousands, x_score + x_space * 0, y_score);
    forest_draw_cgf_digit_white(&textures.cave_score_font, (Uint32)hundreds, x_score + x_space * 1, y_score);
    forest_draw_cgf_digit_white(&textures.cave_score_font, (Uint32)tens, x_score + x_space * 2, y_score);
    forest_draw_cgf_digit_white(&textures.cave_score_font, (Uint32)ones, x_score + x_space * 3, y_score);
}

CaveState process_cave_waiting_before_talking(InputState state) { 
    if (get_state_time(&state_metadata) > 2.5) {
        return STATE_CAVE_TALKING_BEFORE_CLIMB;
    }
    return STATE_CAVE_NONE;
}

void render_cave_waiting_before_talking() {
    forest_draw_anim_frame(&textures.cave_talks, 12, 0, 0);
    render_cave_scoreboard();
}

void on_enter_cave_waiting_before_talking(){
    forest_play(&audio.cave_her_er_vi);
    cave_ctx.rolling_score = cave_ctx.score;
}

//--------------------------

CaveState process_cave_talking_before_climb(InputState state) {
    if (one_shot(&state_metadata, 4.0, 0)) {
        forest_play(&audio.cave_trappe_grin);
    }
    
    if (get_frame_index(&state_metadata) >= textures.cave_talks.sync.sz) {
        return STATE_CAVE_CLIMBING;
    }
    return STATE_CAVE_NONE;
}

void render_cave_talking_before_climb() {
    forest_draw_sync_til(&textures.cave_talks, &textures.cave_talks.sync, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

void on_enter_cave_talking_before_climb() {
    forest_play(&audio.cave_trappe_snak);
}

//--------------------------

CaveState process_cave_climbing(InputState state) {
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(&textures.cave_climbs)) {
        return STATE_CAVE_WAITING_INPUT;
    }

    if(one_shot(&state_metadata, 1.0, 0)){
        forest_play(&audio.cave_nu_kommer_jeg);
    }
    if(one_shot(&state_metadata, 2.0, 1)){
        forest_play(&audio.cave_pre_fanfare);
    }

    return STATE_CAVE_NONE;
}

void render_cave_climbing() {
    forest_draw_anim_full(&textures.cave_climbs, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}


//--------------------------

CaveState process_cave_waiting_input(InputState state) {
    if(one_shot(&state_metadata, 0.5, 0)){
        forest_play(&audio.cave_afskylia_snak);
    }

    if(state.cave_rope_1_pressed) {
        cave_ctx.cave_selected_rope = 0;
        forest_play(&audio.cave_tast_trykket);
        return STATE_CAVE_GOING_ROPE;
    } else if(state.cave_rope_2_pressed) {
        cave_ctx.cave_selected_rope = 1;
        forest_play(&audio.cave_tast_trykket);
        return STATE_CAVE_GOING_ROPE;
    } else if(state.cave_rope_3_pressed) {
        cave_ctx.cave_selected_rope = 2;
        forest_play(&audio.cave_tast_trykket);
        return STATE_CAVE_GOING_ROPE;
    }
    return STATE_CAVE_NONE;
}

void render_cave_waiting_input() {
    forest_draw_anim_frame(&textures.cave_first_rope, 0, 0, 0);
    render_cave_scoreboard();
}


//--------------------------

ForestAnim *get_rope_animation(void){
    if (cave_ctx.cave_selected_rope == 0) {
        return &textures.cave_first_rope;
    } else if (cave_ctx.cave_selected_rope == 1) {
        return &textures.cave_second_rope;
    } else if (cave_ctx.cave_selected_rope == 2) {
        return &textures.cave_third_rope;
    }
    return &textures.cave_first_rope;
}

CaveState process_cave_going_rope(InputState state) {
    // Play hiv_i_reb sound at specific time based on selected rope
    double sound_times[] = {2.0, 3.0, 4.0};
    if (one_shot(&state_metadata, sound_times[cave_ctx.cave_selected_rope], 0)) {
        forest_play(&audio.cave_hiv_i_reb);
    }
    
    // Footstep sounds alternating every 0.4 seconds
    if (every(&state_metadata, 0.4, 1, 0.0)) {
        forest_play(&audio.cave_fodtrin1);
    }
    if (every(&state_metadata, 0.4, 2, 0.4)) {
        forest_play(&audio.cave_fodtrin2);
    }
    
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(get_rope_animation())) {
        // Calculate win or lose (25% lose, 75% win)
        int random_value = rand() % 4;
        
        if (random_value == 0) {
            // Lost
            return STATE_CAVE_LOST;
        } else {
            // Won - different multipliers (Scylla loses, Hugo wins)
            if (random_value == 1) {
                cave_ctx.cave_win_type = 0; // Bird
                cave_ctx.score *= 2;
            } else if (random_value == 2) {
                cave_ctx.cave_win_type = 1; // Leaves
                cave_ctx.score *= 3;
            } else {
                cave_ctx.cave_win_type = 2; // Ropes
                cave_ctx.score *= 4;
            }
            return STATE_CAVE_SCYLLA_LOST;
        }
    }
    return STATE_CAVE_NONE;
}

void render_cave_going_rope() {
    forest_draw_anim_full(get_rope_animation(), get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

//--------------------------

void on_enter_cave_family_happy(){
    forest_play(&audio.cave_fanfare);
}

void on_enter_cave_lost_spring(){
    forest_play(&audio.cave_hugo_katapult);
}


void render_cave_lost() {
    ForestAnim *puff_animation = NULL;
    
    if (cave_ctx.cave_selected_rope == 0) {
        puff_animation = &textures.cave_hugo_puff_first;
    } else if (cave_ctx.cave_selected_rope == 1) {
        puff_animation = &textures.cave_hugo_puff_second;
    } else if (cave_ctx.cave_selected_rope == 2) {
        puff_animation = &textures.cave_hugo_puff_third;
    }
    
    forest_draw_anim_full(puff_animation, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

void render_cave_lost_spring() {
    forest_draw_anim_full(&textures.cave_hugo_spring, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

void render_cave_scylla_lost() {
    ForestAnim *scylla_animation = NULL;
    
    if (cave_ctx.cave_win_type == 0) {
        scylla_animation = &textures.cave_scylla_bird;
    } else if (cave_ctx.cave_win_type == 1) {
        scylla_animation = &textures.cave_scylla_leaves;
    } else if (cave_ctx.cave_win_type == 2) {
        scylla_animation = &textures.cave_scylla_ropes;
    }
    
    forest_draw_anim_full(scylla_animation, get_frame_index(&state_metadata));
    
    // Render Hugo sprite on top
    int hugo_positions[3][2] = {{25, 105}, {97, 100}, {172, 102}};
    forest_draw_cgf_at(&textures.cave_hugo_sprite, 0, hugo_positions[cave_ctx.cave_selected_rope][0], hugo_positions[cave_ctx.cave_selected_rope][1], 1);
    render_cave_scoreboard();
}

void render_cave_scylla_spring() {
    forest_draw_anim_full(&textures.cave_scylla_spring, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

void render_cave_family_cage_opens() {
    forest_draw_anim_full(&textures.cave_family_cage, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

void render_cave_family_happy() {
    forest_draw_anim_full(&textures.cave_happy, get_frame_index(&state_metadata));
    render_cave_scoreboard();
}

// Legacy cave rendering functions (for compatibility)
void render_cave_win() {
    render_cave_scylla_lost();
}

void render_cave_win_spring() {
    render_cave_scylla_spring();
}

void render_cave_happy() {
    render_cave_family_happy();
}

CaveState process_cave_intro(InputState state) {
    if (get_state_time(&state_metadata) > 2.5) {
        return STATE_CAVE_CLIMBING;
    }
    return STATE_CAVE_NONE;
}

CaveState process_cave_lost(InputState state) {
    if (one_shot(&state_metadata, 1.0, 0)) {
        forest_play(&audio.cave_pre_puf);
    }
    
    if (one_shot(&state_metadata, 2.0, 1)) {
        forest_play(&audio.cave_puf);
    }
    
    // Use the correct animation length for the lose state
    ForestAnim *puff_animation = NULL;
    if (cave_ctx.cave_selected_rope == 0) {
        puff_animation = &textures.cave_hugo_puff_first;
    } else if (cave_ctx.cave_selected_rope == 1) {
        puff_animation = &textures.cave_hugo_puff_second;
    } else if (cave_ctx.cave_selected_rope == 2) {
        puff_animation = &textures.cave_hugo_puff_third;
    }
    
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(puff_animation)) {
        return STATE_CAVE_LOST_SPRING;
    }
    return STATE_CAVE_NONE;
}

CaveState process_cave_lost_spring(InputState state) {
    if (one_shot(&state_metadata, 2.5, 0)) {
        forest_play(&audio.cave_hugo_skyd_ud);
    }
    
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(&textures.cave_hugo_spring)) {
        return STATE_CAVE_END;
    }
    return STATE_CAVE_NONE;
}

CaveState process_cave_scylla_lost(InputState state) {
    if (cave_ctx.cave_win_type == 0) {
        // Bird sound
        if (one_shot(&state_metadata, 0.5, 0)) {
            forest_play(&audio.cave_fugle_skrig);
        }
    } else if (cave_ctx.cave_win_type == 1) {
        // Leaves sound
        if (one_shot(&state_metadata, 0.5, 0)) {
            forest_play(&audio.cave_skrig);
        }
    } else if (cave_ctx.cave_win_type == 2) {
        // Ropes sounds
        if (one_shot(&state_metadata, 1.0, 0)) {
            forest_play(&audio.cave_pre_puf);
        }
        if (one_shot(&state_metadata, 2.0, 1)) {
            forest_play(&audio.cave_puf);
        }
    }
    
    // Use the correct animation length based on win type
    ForestAnim *scylla_animation = NULL;
    if (cave_ctx.cave_win_type == 0) {
        scylla_animation = &textures.cave_scylla_bird;
    } else if (cave_ctx.cave_win_type == 1) {
        scylla_animation = &textures.cave_scylla_leaves;
    } else if (cave_ctx.cave_win_type == 2) {
        scylla_animation = &textures.cave_scylla_ropes;
    }
    
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(scylla_animation)) {
        if (cave_ctx.cave_win_type == 2) {
            // Only ropes path goes through spring
            return STATE_CAVE_SCYLLA_SPRING;
        } else {
            return STATE_CAVE_FAMILY_CAGE_OPENS;
        }
    }
    return STATE_CAVE_NONE;
}

CaveState process_cave_scylla_spring(InputState state) {
    if (one_shot(&state_metadata,1.5, 0)) {
        forest_play(&audio.cave_fjeder);
    }
    
    if (one_shot(&state_metadata, 2.0, 1)) {
        forest_play(&audio.cave_afskylia_skyd_ud);
    }
    
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(&textures.cave_scylla_spring)) {
        return STATE_CAVE_FAMILY_CAGE_OPENS;
    }
    return STATE_CAVE_NONE;
}

CaveState process_cave_family_cage_opens(InputState state) {
    if (one_shot(&state_metadata, 0.5, 0)) {
        forest_play(&audio.cave_hiv_i_reb);
    }
    
    if (one_shot(&state_metadata, 1.0, 1)) {
        forest_play(&audio.cave_hugoline_tak);
    }
    
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(&textures.cave_family_cage)) {
        return STATE_CAVE_FAMILY_HAPPY;
    }
    return STATE_CAVE_NONE;
}

CaveState process_cave_family_happy(InputState state) {
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(&textures.cave_happy)) {
        return STATE_CAVE_END;
    }
    return STATE_CAVE_NONE;
}

// State lifecycle management
void on_enter_cave_state(CaveState state) {
    switch(state) {
        case STATE_CAVE_WAITING_BEFORE_TALKING:
            on_enter_cave_waiting_before_talking();
            break;
        case STATE_CAVE_TALKING_BEFORE_CLIMB:
            on_enter_cave_talking_before_climb();
            break;
        case STATE_CAVE_LOST_SPRING:
            on_enter_cave_lost_spring();
            break;
        case STATE_CAVE_FAMILY_HAPPY:
            on_enter_cave_family_happy();
            break;
        default:
            break;            
    }
}

GameState process_cave(InputState state) {
    // Process current state
    CaveState next_state = STATE_CAVE_NONE;
    
    switch(current_cave_state) {
        case STATE_CAVE_WAITING_BEFORE_TALKING:
            next_state = process_cave_waiting_before_talking(state);
            break;
        case STATE_CAVE_TALKING_BEFORE_CLIMB:
            next_state = process_cave_talking_before_climb(state);
            break;
        case STATE_CAVE_CLIMBING:
            next_state = process_cave_climbing(state);
            break;
        case STATE_CAVE_WAITING_INPUT:
            next_state = process_cave_waiting_input(state);
            break;
        case STATE_CAVE_GOING_ROPE:
            next_state = process_cave_going_rope(state);
            break;
        case STATE_CAVE_LOST:
            next_state = process_cave_lost(state);
            break;
        case STATE_CAVE_LOST_SPRING:
            next_state = process_cave_lost_spring(state);
            break;
        case STATE_CAVE_SCYLLA_LOST:
            next_state = process_cave_scylla_lost(state);
            break;
        case STATE_CAVE_SCYLLA_SPRING:
            next_state = process_cave_scylla_spring(state);
            break;
        case STATE_CAVE_FAMILY_CAGE_OPENS:
            next_state = process_cave_family_cage_opens(state);
            break;
        case STATE_CAVE_FAMILY_HAPPY:
            next_state = process_cave_family_happy(state);
            break;
        default:
            break;
    }
    
    // Handle state transitions
    if (next_state != STATE_CAVE_NONE) {
        
        if (next_state == STATE_CAVE_END) {
            return FOREST_STATE_END;
        }
        
        current_cave_state = next_state;
        on_enter_cave_state(current_cave_state);
        reset_state(&state_metadata);
    }
    
    // Handle score counting audio
    if (cave_ctx.score != cave_ctx.rolling_score && !sounding_score) {
        cave_score_counter_id = forest_play_loop(&audio.cave_score_counter);
        sounding_score = true;
    } else if (cave_ctx.score == cave_ctx.rolling_score && sounding_score) {
        forest_stop_loop(&audio.cave_score_counter);
        sounding_score = false;
    }
    
    return FOREST_STATE_NONE;
}

void render_cave() {
    // Render current state
    switch(current_cave_state) {
        case STATE_CAVE_WAITING_BEFORE_TALKING:
            render_cave_waiting_before_talking();
            break;
        case STATE_CAVE_TALKING_BEFORE_CLIMB:
            render_cave_talking_before_climb();
            break;
        case STATE_CAVE_CLIMBING:
            render_cave_climbing();
            break;
        case STATE_CAVE_WAITING_INPUT:
            render_cave_waiting_input();
            break;
        case STATE_CAVE_GOING_ROPE:
            render_cave_going_rope();
            break;
        case STATE_CAVE_LOST:
            render_cave_lost();
            break;
        case STATE_CAVE_LOST_SPRING:
            render_cave_lost_spring();
            break;
        case STATE_CAVE_SCYLLA_LOST:
            render_cave_scylla_lost();
            break;
        case STATE_CAVE_SCYLLA_SPRING:
            render_cave_scylla_spring();
            break;
        case STATE_CAVE_FAMILY_CAGE_OPENS:
            render_cave_family_cage_opens();
            break;
        case STATE_CAVE_FAMILY_HAPPY:
            render_cave_family_happy();
            break;
        default:
            break;            
    }
}

void on_enter_cave() {
    reset_state(&state_metadata);
    current_cave_state = STATE_CAVE_WAITING_BEFORE_TALKING;
    sounding_score = false;
    cave_score_counter_id = -1;
    cave_ctx.rolling_score = cave_ctx.score;
    on_enter_cave_state(current_cave_state);
}

void set_cave_score(int score) {
    cave_ctx.score = score;
}

