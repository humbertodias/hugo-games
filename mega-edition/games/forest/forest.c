#include "types.h"
#include "draw.h"
#include "audio.h"
#include "state.h"
#include "forest.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef enum {
    STATE_FOREST_NONE,
    STATE_FOREST_BRANCH_ANIMATION,
    STATE_FOREST_BRANCH_TALKING,
    STATE_FOREST_FLYING_FALLING,
    STATE_FOREST_FLYING_FALLING_HANG_ANIMATION,
    STATE_FOREST_FLYING_FALLING_HANG_TALKING,
    STATE_FOREST_FLYING_START,
    STATE_FOREST_FLYING_TALKING,
    STATE_FOREST_ROCK_ANIMATION,
    STATE_FOREST_ROCK_HIT_ANIMATION,
    STATE_FOREST_ROCK_TALKING,
    STATE_FOREST_TRAP_ANIMATION,
    STATE_FOREST_TRAP_TALKING,
    STATE_FOREST_PLAYING,
    STATE_FOREST_SCYLLA_BUTTON,
    STATE_FOREST_TALKING_AFTER_HURT,
    STATE_FOREST_TALKING_GAME_OVER,
    STATE_FOREST_WAIT_INTRO,
    STATE_FOREST_WIN_TALKING,
    STATE_FOREST_END
} ForestState;

typedef enum {
    OBS_NONE = 0,
    OBS_CATAPULT = 1,
    OBS_TRAP = 2,
    OBS_ROCK = 3,
    OBS_TREE = 4
} ObstacleType;

typedef struct {
    int score;
    int lives;
    double parallax_pos;
    ObstacleType obstacles[FOREST_MAX_TIME];
    int sacks[FOREST_MAX_TIME];
    int leaves[FOREST_MAX_TIME];
    bool arrow_up_focus;
    bool arrow_down_focus;
    double hugo_jumping_time;
    double hugo_crawling_time;
    double last_time;
    int old_second;

    // Cave game state (shared context)
    int cave_selected_rope;
    int cave_win_type;
    int rolling_score;
} ForestContext;

static ForestContext game_ctx = {0};
static ForestState current_forest_state = STATE_FOREST_WAIT_INTRO;
static StateMetadata state_metadata;
static bool debug_show_collisions = false;

#define FOREST_BG_SPEED_MULTIPLIER 1.0

static double new_mod(double a, double b) {
    double res = fmod(a, b);
    if (res == 0.0) return res;
    return (a < 0.0) ? res - b : res;
}


// -----------------------------------------------------------------------------
// Obstacle / sack generation
// -----------------------------------------------------------------------------

void generate_obstacles() {
    // Match Python: empty 65%, others 8.75% each (35%/4 = 8.75%)
    // 0 = none, 1 = catapult, 2 = trap, 3 = rock, 4 = tree
    // Using 10000 scale for more precision: 6500, 875, 875, 875, 875
    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        int r = rand() % 10000;
        if (r < 6500) {
            game_ctx.obstacles[i] = OBS_NONE;
        } else if (r < 6500 + 875) {        // catapult: 8.75%
            game_ctx.obstacles[i] = OBS_CATAPULT;
        } else if (r < 6500 + 1750) {       // trap: 8.75%
            game_ctx.obstacles[i] = OBS_TRAP;
        } else if (r < 6500 + 2625) {       // rock: 8.75%
            game_ctx.obstacles[i] = OBS_ROCK;
        } else {                            // tree: 8.75%
            game_ctx.obstacles[i] = OBS_TREE;
        }
    }

    // Clear first few positions
    if (FOREST_MAX_TIME > 0) game_ctx.obstacles[0] = OBS_NONE;
    if (FOREST_MAX_TIME > 1) game_ctx.obstacles[1] = OBS_NONE;
    if (FOREST_MAX_TIME > 2) game_ctx.obstacles[2] = OBS_NONE;
    if (FOREST_MAX_TIME > 3) game_ctx.obstacles[3] = OBS_NONE;

    // Ensure no consecutive obstacles
    for (int i = 0; i < FOREST_MAX_TIME - 1; i++) {
        if (game_ctx.obstacles[i] != OBS_NONE) {
            game_ctx.obstacles[i + 1] = OBS_NONE;
        }
    }
}

void generate_sacks() {
    // Match Python probabilities exactly:
    // empty 70%, normal 21% (other_prob = 0.3 * 0.7 = 0.21), gold 9% (other_prob_high = 0.3 * 0.3 = 0.09)
    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        int r = rand() % 100;
        if (r < 70) {
            game_ctx.sacks[i] = 0; // No sack (70%)
        } else if (r < 91) {  // 70 + 21 = 91
            game_ctx.sacks[i] = 1; // Normal sack (21%)
        } else {
            game_ctx.sacks[i] = 2; // Gold sack (9%)
        }
    }
}

void generate_leaves() {
    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        int r = rand() % 100;
        game_ctx.leaves[i] = (r < 50) ? 1 : 2;
    }

    for (int i = 0; i < FOREST_MAX_TIME - 1; i++) {
        if (game_ctx.leaves[i] == 2) {
            game_ctx.leaves[i + 1] = 0;
        }
    }
}

void init_game_context() {
    // Optional: seed RNG once (if not done elsewhere)
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    game_ctx.score = 0;
    game_ctx.lives = START_LIVES;
    game_ctx.parallax_pos = 0;
    game_ctx.arrow_up_focus = false;
    game_ctx.arrow_down_focus = false;
    game_ctx.hugo_jumping_time = -1;
    game_ctx.hugo_crawling_time = -1;
    game_ctx.last_time = get_game_time();
    game_ctx.old_second = -1;

    // Cave game initialization
    game_ctx.cave_selected_rope = -1;
    game_ctx.cave_win_type = -1;
    game_ctx.rolling_score = 0;

    generate_obstacles();
    generate_sacks();
    generate_leaves();
}

// ---------------- HUD / BOTTOM ----------------

void render_forest_bottom() {
    if (textures.scoreboard) {
        forest_blit_surface(textures.scoreboard, 0, 184);
    }

    // Lives indicators
    for (int i = 0; i < game_ctx.lives; i++) {
        if (textures.hugo_lives.fp) {
            forest_draw_cgf_at(&textures.hugo_lives, 0, 32 + i * 40, 188, 1);
        }
    }

    // Score digits (spritesheet 5x2; 32x33 cells with 1px spacing)
    int x_score = 200;
    int y_score = 194;
    int x_space = 24;
    int thousands = game_ctx.score / 1000;
    int hundreds = (game_ctx.score % 1000) / 100;
    int tens     = (game_ctx.score % 100) / 10;
    int ones     = game_ctx.score % 10;

    int vals[4] = { thousands, hundreds, tens, ones };
    for (int i = 0; i < 4; i++) {
        forest_draw_cgf_sheet_digit(&textures.score_numbers, vals[i], x_score + x_space * i, y_score);
    }
}

// ---------------- RENDERING: BACKGROUND / OBSTACLES / SACKS / HUGO / CONTROLS ----------------

void render_obstacles() {
    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        ObstacleType obs = game_ctx.obstacles[i];
        if (obs == OBS_NONE) continue;

        /* Rocks roll left faster than the ground scroll until off-screen. */
        double obstacle_pos = (obs == OBS_ROCK)
            ? ((double)i - game_ctx.parallax_pos) * ROCK_ROLL_SPEED
            : ((double)i - game_ctx.parallax_pos) * FOREST_GROUND_SPEED;

        if (obstacle_pos < -50 || obstacle_pos > FOREST_SCREEN_WIDTH + 50) {
            continue;
        }

        switch (obs) {
        case OBS_CATAPULT: {
            /* Rest compressed; spring only when Hugo is on the platform middle. */
            int nframes = (int)textures.catapult.head.num;
            if (nframes < 1) nframes = 1;
            int idx = 0;
            int cat_w = forest_cgf_width(&textures.catapult);
            int hugo_w = forest_cgf_width(&textures.hugo_side);
            if (cat_w <= 0) cat_w = 51;
            if (hugo_w <= 0) hugo_w = 58;
            int cat_x = (int)(obstacle_pos - 8);
            int hugo_cx = HUGO_X_POS + hugo_w / 2;
            int cat_cx = cat_x + cat_w / 2;
            int delta = hugo_cx - cat_cx; /* <0 approaching, 0 = on middle */
            int start_dist = cat_w / 2;
            if (!game_ctx.arrow_up_focus && delta >= -start_dist && delta <= start_dist) {
                double t = (double)(delta + start_dist) / (double)start_dist;
                if (t < 0.0) t = 0.0;
                if (t > 1.0) t = 1.0;
                idx = (int)(t * (nframes - 1) + 1e-6);
                if (idx >= nframes) idx = nframes - 1;
            }
            int dy[8] = { 45, 43, 39, 34, 29, 22, 14, 1 };
            int y = 112 + dy[idx % 8];
            forest_draw_cgf_at(&textures.catapult, idx, cat_x, y, 1);
            break;
        }
        case OBS_TRAP: {
            /* Rest closed; snap only when Hugo is about to hit (not jumping). */
            int nframes = (int)textures.trap.head.num;
            if (nframes < 1) nframes = 1;
            int idx = 0;
            double eta = (double)i - game_ctx.parallax_pos;
            const double anim_start = 1.35;
            const double anim_duration = 0.35;
            if (!game_ctx.arrow_up_focus && eta < anim_start) {
                double t = (anim_start - eta) / anim_duration;
                if (t < 0.0) t = 0.0;
                if (t > 1.0) t = 1.0;
                idx = (int)(t * (nframes - 1) + 1e-6);
                if (idx >= nframes) idx = nframes - 1;
            }
            int dy[6] = { 176, 173, 169, 165, 176, 176 };
            int y = dy[idx % 6] - 24;
            forest_draw_cgf_at(&textures.trap, idx, (int)(obstacle_pos - 8), y, 1);
            break;
        }
        case OBS_ROCK: {
            /* Spin while translating left across the screen. */
            int nframes = (int)textures.rock.head.num;
            if (nframes < 1) nframes = 1;
            double traveled = (double)FOREST_SCREEN_WIDTH + 50.0 - obstacle_pos;
            if (traveled < 0.0) traveled = 0.0;
            int idx = ((int)(traveled / 6.0)) % nframes;
            if (idx < 0) idx = 0;
            forest_draw_cgf_at(&textures.rock, idx, (int)obstacle_pos, 120, 1);
            break;
        }
        case OBS_TREE: {
            /* Hang still until Hugo is close, then swing once as he approaches. */
            int nframes = (int)textures.tree.head.num;
            if (nframes > 0 && textures.tree.data &&
                textures.tree.data[nframes - 1].width == 0) {
                nframes--; /* last CGF entry is an empty terminator */
            }
            if (nframes < 1) nframes = 1;

            int idx = 0;
            double eta = (double)i - game_ctx.parallax_pos; /* seconds until this slot */
            const double swing_start = 1.5;    /* start swinging when this close */
            const double swing_duration = 0.5; /* shorter = faster swing */
            if (eta < swing_start) {
                double t = (swing_start - eta) / swing_duration;
                if (t < 0.0) t = 0.0;
                if (t > 1.0) t = 1.0;
                idx = (int)(t * (nframes - 1) + 1e-6);
                if (idx >= nframes) idx = nframes - 1;
            }

            forest_draw_cgf_at(&textures.lone_tree, 0, (int)(obstacle_pos - 52), -40, 1);
            forest_draw_cgf_at(&textures.tree, idx, (int)obstacle_pos, 52 + 10, 1);
            break;
        }
        default:
            break;
        }
    }
}

void render_sacks() {
    (void)get_frame_index(&state_metadata); // frame not used but kept for parity

    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        if (game_ctx.sacks[i] == 0) continue;

        double sack_pos = (i - game_ctx.parallax_pos) * FOREST_GROUND_SPEED;
        if (sack_pos < -50 || sack_pos > FOREST_SCREEN_WIDTH + 50) {
            continue;
        }

        forest_draw_cgf_at(&textures.sack, 0, (int)(sack_pos - 16), 32, 1);
    }
}

void render_leaves() {
    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        if (game_ctx.leaves[i] == 0) continue;

        double leave_pos = (i - game_ctx.parallax_pos) * FOREST_GROUND_SPEED;
        if (leave_pos < -50 || leave_pos > FOREST_SCREEN_WIDTH + 50) {
            continue;
        }

        if (game_ctx.leaves[i] == 1) {
            forest_draw_cgf_at(&textures.leaves2, 0, (int)(leave_pos - 16), -10, 1);
        } else if (game_ctx.leaves[i] == 2) {
            forest_draw_cgf_at(&textures.leaves1, 0, (int)(leave_pos - 16), -10, 1);
        }
    }
}

void render_hugo() {
    int frame = get_frame_index(&state_metadata);
    int hugo_x = HUGO_X_POS;
    double now = get_game_time();

    if (game_ctx.arrow_up_focus && game_ctx.hugo_jumping_time >= 0) {
        double dt = (now - game_ctx.hugo_jumping_time) / 0.75;
        double dy = -250 * dt * dt + 250 * dt - 22.5;
        int y = (int)(40 - dy);
        int idx = frame % 3;
        forest_draw_cgf_at(&textures.hugo_jump, idx, hugo_x, y, 1);
    } else if (game_ctx.arrow_down_focus) {
        int y = 105;
        int idx = frame % 8;
        forest_draw_cgf_at(&textures.hugo_crawl, idx, hugo_x, y, 1);
    } else {
        int y = 90;
        int idx = frame % 8;
        forest_draw_cgf_at(&textures.hugo_side, idx, hugo_x, y, 1);
    }
}

void render_controls() {
    // arrows[0] up normal, [1] up pressed, [2] down normal, [3] down pressed
    if (game_ctx.arrow_up_focus) {
        forest_blit_surface(textures.arrows[1], 256, 17);
    } else if (textures.arrows[0]) {
        forest_blit_surface(textures.arrows[0], 256 + 2, 16 + 3);
    }

    if (game_ctx.arrow_down_focus) {
        forest_blit_surface(textures.arrows[3], 256, 54);
    } else if (textures.arrows[2]) {
        forest_blit_surface(textures.arrows[2], 256 + 2, 54 + 2);
    }
}

static void hugo_debug_bounds(int *out_x, int *out_y, int *out_w, int *out_h)
{
    int hugo_x = HUGO_X_POS;
    double now = get_game_time();
    cgfinfo *sprite = &textures.hugo_side;
    int y = 90;

    if (game_ctx.arrow_up_focus && game_ctx.hugo_jumping_time >= 0) {
        double dt = (now - game_ctx.hugo_jumping_time) / 0.75;
        double dy = -250 * dt * dt + 250 * dt - 22.5;
        y = (int)(40 - dy);
        sprite = &textures.hugo_jump;
    } else if (game_ctx.arrow_down_focus) {
        y = 105;
        sprite = &textures.hugo_crawl;
    }

    *out_x = hugo_x;
    *out_y = y;
    *out_w = forest_cgf_width(sprite);
    if (*out_w <= 0)
        *out_w = 32;
    *out_h = forest_cgf_height(sprite);
    if (*out_h <= 0)
        *out_h = 48;
}

static void render_collision_debug(void)
{
    int check_idx;
    int trigger_x;
    int hx, hy, hw, hh;
    bool hugo_safe_jump;
    bool hugo_safe_duck;

    if (!debug_show_collisions)
        return;

    check_idx = (int)floor(game_ctx.parallax_pos) + 1;
    if (check_idx >= FOREST_MAX_TIME)
        check_idx = FOREST_MAX_TIME - 1;

    trigger_x = (int)((check_idx - game_ctx.parallax_pos) * FOREST_GROUND_SPEED);
    forest_draw_rect_outline(trigger_x - 1, 0, 3, FOREST_SCREEN_HEIGHT, 255, 255, 0, 255);

    hugo_debug_bounds(&hx, &hy, &hw, &hh);
    hugo_safe_jump = game_ctx.arrow_up_focus;
    hugo_safe_duck = game_ctx.arrow_down_focus;
    forest_draw_rect_outline(hx, hy, hw, hh, 0, 220, 255, 255);

    for (int i = 0; i < FOREST_MAX_TIME; i++) {
        double obstacle_pos = (i - game_ctx.parallax_pos) * FOREST_GROUND_SPEED;
        int x, y, w, h;
        bool would_hit;
        ObstacleType obs;
        Uint8 fill_r, fill_g, fill_b;

        if (obstacle_pos < -80 || obstacle_pos > FOREST_SCREEN_WIDTH + 80)
            continue;

        obs = game_ctx.obstacles[i];
        if (obs != OBS_NONE) {
            x = 0;
            y = 0;
            w = 32;
            h = 32;
            would_hit = false;

            switch (obs) {
            case OBS_CATAPULT:
                w = forest_cgf_width(&textures.catapult);
                h = forest_cgf_height(&textures.catapult);
                if (w <= 0) w = 40;
                if (h <= 0) h = 40;
                x = (int)(obstacle_pos - 8);
                y = 112;
                would_hit = !hugo_safe_jump;
                break;
            case OBS_TRAP:
                w = forest_cgf_width(&textures.trap);
                h = forest_cgf_height(&textures.trap);
                if (w <= 0) w = 40;
                if (h <= 0) h = 40;
                x = (int)(obstacle_pos - 8);
                y = 152;
                would_hit = !hugo_safe_jump;
                break;
            case OBS_ROCK: {
                w = forest_cgf_width(&textures.rock);
                h = forest_cgf_height(&textures.rock);
                if (w <= 0) w = 40;
                if (h <= 0) h = 40;
                x = (int)((i - game_ctx.parallax_pos) * ROCK_ROLL_SPEED);
                y = 120;
                would_hit = !hugo_safe_jump;
                break;
            }
            case OBS_TREE:
                w = forest_cgf_width(&textures.tree);
                h = forest_cgf_height(&textures.tree);
                if (w <= 0) w = 48;
                if (h <= 0) h = 48;
                x = (int)obstacle_pos;
                y = 62;
                would_hit = !hugo_safe_duck;
                break;
            default:
                break;
            }

            if (would_hit) {
                fill_r = 220; fill_g = 40; fill_b = 40;
            } else {
                fill_r = 40; fill_g = 180; fill_b = 80;
            }

            forest_draw_rect_outline(x, y, w, h, fill_r, fill_g, fill_b, 255);
            if (i == check_idx)
                forest_draw_rect_outline(x - 2, y - 2, w + 4, h + 4, 255, 255, 0, 255);
        }

        if (game_ctx.sacks[i] != 0) {
            int sw = forest_cgf_width(&textures.sack);
            int sh = forest_cgf_height(&textures.sack);
            int sx = (int)(obstacle_pos - 16);
            int sy = 32;
            bool collectible = hugo_safe_jump;

            if (sw <= 0) sw = 24;
            if (sh <= 0) sh = 24;
            if (collectible) {
                fill_r = 220; fill_g = 180; fill_b = 40;
            } else {
                fill_r = 180; fill_g = 80; fill_b = 255;
            }
            forest_draw_rect_outline(sx, sy, sw, sh, fill_r, fill_g, fill_b, 255);
        }
    }

    forest_draw_rect_fill(0, 0, FOREST_SCREEN_WIDTH, 12, 0, 0, 0, 140);
    forest_draw_rect_outline(4, 2, 8, 8, 255, 40, 40, 255);
    forest_draw_rect_outline(16, 2, 8, 8, 40, 220, 80, 255);
    forest_draw_rect_outline(28, 2, 8, 8, 0, 220, 255, 255);
    forest_draw_rect_outline(40, 2, 8, 8, 255, 255, 0, 255);
}

// Render background (match Python parallax as close as possible)
void render_forest_background() {
    double hills_speed = 6.0 * FOREST_BG_SPEED_MULTIPLIER;
    double trees_speed = 12.0 * FOREST_BG_SPEED_MULTIPLIER;
    double grass_speed = 30.0 * FOREST_BG_SPEED_MULTIPLIER;

    // Query texture widths each frame so scrolling repeats with the real asset size
    int hills_width  = forest_cgf_width(&textures.bg_hillsday);
    int trees_width  = forest_cgf_width(&textures.bg_trees);
    int grass_width  = forest_cgf_width(&textures.grass);
    int ground_width = forest_cgf_width(&textures.bg_ground);

    double p = game_ctx.parallax_pos;

    int hills_x   = (int)new_mod(-p * hills_speed,          (double)hills_width);
    int trees_x   = (int)new_mod(-p * trees_speed,          (double)trees_width);
    int grass_x0  = (int)new_mod(-p * grass_speed,          (double)grass_width);
    int ground_x0 = (int)new_mod(-p * FOREST_GROUND_SPEED,  (double)ground_width);

    int mountain_x = 320 - 96 - (int)((p - FOREST_MAX_TIME) * FOREST_GROUND_SPEED);

    // Gradient sky
    if (textures.bg_gradient) {
        forest_blit_surface(textures.bg_gradient, 0, 0);
    }

    // Hills
    if (textures.bg_hillsday.fp) {
        forest_draw_cgf_at(&textures.bg_hillsday, 0, hills_x, 0, 1);
        forest_draw_cgf_at(&textures.bg_hillsday, 0, hills_x + hills_width, 0, 1);
    }

    // Trees
    if (textures.bg_trees.fp) {
        forest_draw_cgf_at(&textures.bg_trees, 0, trees_x, -24, 1);
        forest_draw_cgf_at(&textures.bg_trees, 0, trees_x + trees_width, -24, 1);
    }

    // Ground tiles
    if (textures.bg_ground.fp) {
        for (int i = 0; i < 6; i++) {
            int x = ground_x0 + i * ground_width;
            forest_draw_cgf_at(&textures.bg_ground, 0, x, 158, 1);
        }
    }

    // Grass tiles
    if (textures.grass.fp) {
        for (int i = 0; i < 13; i++) {
            int x = grass_x0 + i * grass_width;
            forest_draw_cgf_at(&textures.grass, 0, x, 172, 1);
        }
    }

    // End mountain
    if (textures.end_mountain.fp) {
        forest_draw_cgf_at(&textures.end_mountain, 0, mountain_x, -16, 1);
    }
}

void render_forest_playing_content() {
    render_obstacles();
    render_sacks();
    render_leaves();
    render_hugo();
    render_controls();
    render_collision_debug();
}

void render_forest_playing() {
    render_forest_background();
    render_forest_playing_content();
    render_forest_bottom();
}

static void on_resume_forest_playing(void)
{
    game_ctx.arrow_up_focus = false;
    game_ctx.arrow_down_focus = false;
    game_ctx.hugo_jumping_time = -1;
    game_ctx.hugo_crawling_time = -1;
    game_ctx.last_time = get_game_time();
}

// ---------------- HURT / TALKING RENDERING ----------------

void render_forest_branch_animation() {
    forest_draw_anim_full(&textures.hugohitlog, get_frame_index_fast(&state_metadata));
}

void render_forest_branch_talking() {
    forest_draw_sync_til(&textures.hugohitlog_talk, &textures.sync_hitlog,
                         get_frame_index(&state_metadata));
}

void render_forest_flying_start() {
    forest_draw_anim_full(&textures.catapult_fly, get_frame_index_fast(&state_metadata));
}

void render_forest_flying_talking() {
    forest_draw_sync_til(&textures.catapult_airtalk, &textures.sync_catapult_talktop,
                         get_frame_index(&state_metadata));
}

void render_forest_flying_falling() {
    forest_draw_anim_full(&textures.catapult_fall, get_frame_index_fast(&state_metadata));
}

void render_forest_flying_falling_hang_animation() {
    forest_draw_anim_full(&textures.catapult_hang, get_frame_index_fast(&state_metadata));
}

void render_forest_flying_falling_hang_talking() {
    // Static hang frame 12 as background
    forest_draw_anim_frame(&textures.catapult_hang, 12, 0, 0);
    forest_draw_sync_cgf(&textures.catapult_hangspeak, &textures.sync_catapult_hang,
                         get_frame_index(&state_metadata), 115, 117);
}

void render_forest_rock_animation() {
    forest_draw_anim_full(&textures.hugo_lookrock, get_frame_index_fast(&state_metadata));
}

void render_forest_rock_hit_animation() {
    forest_draw_anim_full(&textures.hit_rock, get_frame_index_fast(&state_metadata));
}

void render_forest_rock_talking() {
    forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_rock, get_frame_index(&state_metadata), 0, 0);
}

void render_forest_trap_animation() {
    forest_draw_anim_full(&textures.hugo_traphurt, get_frame_index(&state_metadata));
}

void render_forest_trap_talking() {
    forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_trap, get_frame_index(&state_metadata), 0, 0);
}

void render_forest_scylla_button() {
    // Placeholder: GameTextures doesn't define scylla hand textures yet;
    // skip to avoid compilation error.
    // Implement when textures are added to GameTextures.
}

void render_forest_talking_after_hurt() {
    int frame = get_frame_index(&state_metadata);

    // Use appropriate sync data based on lives remaining
    if (game_ctx.lives == 1)
        forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_lastlife, frame, 128, -16);
    else
        forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_dieonce, frame, 128, -16);

    // Hand animation
    if (frame < 8 && frame % 4 == 0) {
        forest_draw_cgf_at(&textures.hugo_hand2, 0, 96, 78, 1);
    } else {
        forest_draw_cgf_at(&textures.hugo_hand1, 0, 96, 83, 1);
    }
}

void render_forest_talking_game_over() {
    forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_gameover,
                         get_frame_index(&state_metadata), 128, -16);
}

void render_forest_win_talking() {
    forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_levelcompleted,
                         get_frame_index(&state_metadata), 128, -16);
}

void render_forest_wait_intro() {
    forest_draw_sync_cgf(&textures.hugo_telllives, &textures.sync_start,
                         get_frame_index(&state_metadata), 128, -16);
}

// ---------------- COLLISION ----------------

bool check_collision(int obstacle_idx) {
    ObstacleType obs = game_ctx.obstacles[obstacle_idx];
    if (obs == OBS_NONE) return false;

    // Catapult, trap, rock need jump; tree needs duck
    if (obs == OBS_CATAPULT || obs == OBS_TRAP || obs == OBS_ROCK) {
        return !game_ctx.arrow_up_focus;
    } else if (obs == OBS_TREE) {
        return !game_ctx.arrow_down_focus;
    }
    return false;
}

// ---------------- STATE PROCESSING: PLAYING / HURT / TALKING / WIN / GAME OVER ----------------

ForestState process_forest_playing(InputState state) {
    // Jump: single timed action on key press
    if (!game_ctx.arrow_up_focus && !game_ctx.arrow_down_focus) {
        if (state.key_up) {
            game_ctx.arrow_up_focus = true;
            game_ctx.hugo_jumping_time = get_game_time();
        }
    }

    /* Crawl: hold down to stay ducked; stand up only when key is released. */
    if (!game_ctx.arrow_up_focus) {
        if (state.key_down) {
            if (!game_ctx.arrow_down_focus) {
                game_ctx.arrow_down_focus = true;
                game_ctx.hugo_crawling_time = get_game_time();
            }
        } else if (game_ctx.arrow_down_focus) {
            game_ctx.arrow_down_focus = false;
            game_ctx.hugo_crawling_time = -1;
        }
    }

    // End condition
    if (game_ctx.parallax_pos > FOREST_MAX_TIME) {
        game_ctx.parallax_pos = FOREST_MAX_TIME;
        return STATE_FOREST_WIN_TALKING;
    }

    // Update parallax position
    double current_time = get_game_time();
    game_ctx.parallax_pos += current_time - game_ctx.last_time;
    game_ctx.last_time = current_time;

    // Integer / fractional position
    double int_part;
    modf(game_ctx.parallax_pos, &int_part);
    int integer = (int)int_part + 1;
    if (integer >= FOREST_MAX_TIME) {
        integer = FOREST_MAX_TIME - 1;
    }

    if (game_ctx.old_second < 0) {
        game_ctx.old_second = (int)floor(game_ctx.parallax_pos);
    }

    // Update jump / crawl timers
    if (game_ctx.arrow_up_focus && game_ctx.hugo_jumping_time >= 0) {
        if (current_time - game_ctx.hugo_jumping_time > 0.75) {
            game_ctx.hugo_jumping_time = -1;
            game_ctx.arrow_up_focus = false;
        }
    }

    /* Catapult launches when Hugo is standing on its middle (not jumping). */
    if (!game_ctx.arrow_up_focus) {
        int hugo_w = forest_cgf_width(&textures.hugo_side);
        int cat_w = forest_cgf_width(&textures.catapult);
        int rock_w = forest_cgf_width(&textures.rock);
        if (hugo_w <= 0) hugo_w = 58;
        if (cat_w <= 0) cat_w = 51;
        if (rock_w <= 0) rock_w = 45;
        int hugo_cx = HUGO_X_POS + hugo_w / 2;
        int mid_tol = cat_w / 5;
        if (mid_tol < 6) mid_tol = 6;

        for (int i = 0; i < FOREST_MAX_TIME; i++) {
            ObstacleType obs = game_ctx.obstacles[i];
            if (obs == OBS_CATAPULT) {
                double obstacle_pos = (i - game_ctx.parallax_pos) * FOREST_GROUND_SPEED;
                int cat_cx = (int)(obstacle_pos - 8) + cat_w / 2;
                if (abs(hugo_cx - cat_cx) <= mid_tol) {
                    if (FOREST_SOUND_READY(audio.sfx_hugo_launch)) forest_play(&audio.sfx_hugo_launch);
                    if (FOREST_SOUND_READY(audio.sfx_catapult_eject)) forest_play(&audio.sfx_catapult_eject);
                    game_ctx.obstacles[i] = OBS_NONE;
                    return STATE_FOREST_FLYING_START;
                }
            } else if (obs == OBS_ROCK) {
                double rock_x = ((double)i - game_ctx.parallax_pos) * ROCK_ROLL_SPEED;
                int rock_cx = (int)rock_x + rock_w / 2;
                int hit_tol = (hugo_w + rock_w) / 4;
                if (abs(hugo_cx - rock_cx) <= hit_tol) {
                    if (FOREST_SOUND_READY(audio.sfx_hugo_hitlog)) forest_play(&audio.sfx_hugo_hitlog);
                    game_ctx.obstacles[i] = OBS_NONE;
                    return STATE_FOREST_ROCK_ANIMATION;
                }
            }
        }
    }

    // New second: handle collisions and sacks
    int current_second = (int)floor(game_ctx.parallax_pos);
    if (game_ctx.old_second != current_second) {

        ObstacleType obs = game_ctx.obstacles[integer];
        if (obs != OBS_NONE) {
            if (obs == OBS_TRAP && !game_ctx.arrow_up_focus) {
                if (FOREST_SOUND_READY(audio.sfx_hugo_hittrap)) forest_play(&audio.sfx_hugo_hittrap);
                game_ctx.obstacles[integer] = OBS_NONE;
                return STATE_FOREST_TRAP_ANIMATION;
            } else if (obs == OBS_TREE) {
                if (game_ctx.arrow_down_focus) {
                    if (FOREST_SOUND_READY(audio.sfx_tree_swush)) forest_play(&audio.sfx_tree_swush);
                } else {
                    if (FOREST_SOUND_READY(audio.sfx_hugo_hitlog)) forest_play(&audio.sfx_hugo_hitlog);
                    game_ctx.obstacles[integer] = OBS_NONE;
                    return STATE_FOREST_BRANCH_ANIMATION;
                }
            }
        }

        // Sacks when jumping
        if (game_ctx.arrow_up_focus && game_ctx.sacks[integer] != 0) {
            if (game_ctx.sacks[integer] == 1) {
                game_ctx.score += 100;
                if (FOREST_SOUND_READY(audio.sfx_sack_normal)) forest_play(&audio.sfx_sack_normal);
                game_ctx.sacks[integer] = 0;
            } else if (game_ctx.sacks[integer] == 2) {
                game_ctx.score += 250;
                if (FOREST_SOUND_READY(audio.sfx_sack_bonus)) forest_play(&audio.sfx_sack_bonus);
                game_ctx.sacks[integer] = 0;
            }
        }

        game_ctx.old_second = current_second;
    }

    // Footstep SFX every 8 frames if not jumping
    if (get_frame_index(&state_metadata) % 8 == 0 && !game_ctx.arrow_up_focus) {
        int r = rand() % 5;
        if (FOREST_SOUND_READY(audio.sfx_hugo_walk[r]))
            forest_play(&audio.sfx_hugo_walk[r]);
    }

    return STATE_FOREST_NONE;
}

// Helper: reduce lives and go to appropriate talking/game over
ForestState reduce_lives_and_transition() {
    game_ctx.lives--;
    printf("Hit obstacle! Lives remaining: %d\n", game_ctx.lives);

    if (game_ctx.lives <= 0) {
        return STATE_FOREST_TALKING_GAME_OVER;
    } else {
        return STATE_FOREST_TALKING_AFTER_HURT;
    }
}

// ---------------- HURT STATE PROCESSING ----------------

ForestState process_forest_branch_animation(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.sfx_birds)) forest_play(&audio.sfx_birds);
    }

    if (get_frame_index_fast(&state_metadata) >= forest_anim_frame_count(&textures.hugohitlog)) {
        return STATE_FOREST_BRANCH_TALKING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_branch_talking(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_hitlog)) forest_play(&audio.speak_hitlog);
    }

    // Use sync data to determine transition
    if (textures.sync_hitlog.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_hitlog.sz) {
        return reduce_lives_and_transition();
    }

    return STATE_FOREST_NONE;
}

ForestState process_forest_flying_start(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_catapult_up)) forest_play(&audio.speak_catapult_up);
    }

    if (one_shot(&state_metadata, 2.7, 1)) {
        if (FOREST_SOUND_READY(audio.sfx_hugo_screenklir)) forest_play(&audio.sfx_hugo_screenklir);
    }

    if (one_shot(&state_metadata, 2.7, 2)) {
        if (FOREST_SOUND_READY(audio.speak_catapult_hit)) forest_play(&audio.speak_catapult_hit);
    }

    if (get_frame_index_fast(&state_metadata) >= forest_anim_frame_count(&textures.catapult_fly)) {
        return STATE_FOREST_FLYING_TALKING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_flying_talking(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_catapult_talktop)) forest_play(&audio.speak_catapult_talktop);
    }

    // Use sync data to determine transition
    if (textures.sync_catapult_talktop.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_catapult_talktop.sz) {
        return STATE_FOREST_FLYING_FALLING;
    }

    return STATE_FOREST_NONE;
}

ForestState process_forest_flying_falling(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_catapult_down)) forest_play(&audio.speak_catapult_down);
        if (FOREST_SOUND_READY(audio.sfx_hugo_crash)) forest_play(&audio.sfx_hugo_crash);
    }

    if (get_frame_index_fast(&state_metadata) >= forest_anim_frame_count(&textures.catapult_fall)) {
        return STATE_FOREST_FLYING_FALLING_HANG_ANIMATION;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_flying_falling_hang_animation(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        forest_play(&audio.sfx_hugo_hangstart);
    }

    if (get_frame_index_fast(&state_metadata) >= forest_anim_frame_count(&textures.catapult_hang)) {
        return STATE_FOREST_FLYING_FALLING_HANG_TALKING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_flying_falling_hang_talking(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        forest_play(&audio.speak_catapult_hang);
        forest_play(&audio.sfx_hugo_hang);
    }

    // Use sync data to determine transition
    if (textures.sync_catapult_hang.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_catapult_hang.sz) {
        return reduce_lives_and_transition();
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_rock_animation(InputState state) {
    (void)state;
    if (get_frame_index_fast(&state_metadata) >= forest_anim_frame_count(&textures.hugo_lookrock)) {
        return STATE_FOREST_ROCK_HIT_ANIMATION;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_rock_hit_animation(InputState state) {
    (void)state;
    if (get_frame_index_fast(&state_metadata) >= forest_anim_frame_count(&textures.hit_rock)) {
        return STATE_FOREST_ROCK_TALKING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_rock_talking(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_rock)) forest_play(&audio.speak_rock);
    }

    // Use sync data if available
    if (textures.sync_rock.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_rock.sz) {
        return reduce_lives_and_transition();
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_trap_animation(InputState state) {
    (void)state;
    if (get_frame_index(&state_metadata) >= forest_anim_frame_count(&textures.hugo_traphurt)) {
        return STATE_FOREST_TRAP_TALKING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_trap_talking(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_trap)) forest_play(&audio.speak_trap);
    }

    // Use sync data if available
    if (textures.sync_trap.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_trap.sz) {
        return reduce_lives_and_transition();
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_scylla_button(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.5, 0)) {
        if (FOREST_SOUND_READY(audio.sfx_lightning_warning)) forest_play(&audio.sfx_lightning_warning);
    }

    if (get_state_time(&state_metadata) > 2.0) {
        return STATE_FOREST_PLAYING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_talking_after_hurt(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.5, 0)) {
        if (FOREST_SOUND_READY(audio.sfx_hugo_knock)) forest_play(&audio.sfx_hugo_knock);
    }

    if (one_shot(&state_metadata, 0.0, 1)) {
        if (FOREST_SOUND_READY(audio.sfx_hugo_knock)) forest_play(&audio.sfx_hugo_knock);
        if (game_ctx.lives == 1) {
            if (FOREST_SOUND_READY(audio.speak_lastlife)) forest_play(&audio.speak_lastlife);
        } else {
            if (FOREST_SOUND_READY(audio.speak_dieonce)) forest_play(&audio.speak_dieonce);
        }
    }

    // Use sync data to determine when to transition
    int sync_count = (game_ctx.lives == 1) ? textures.sync_lastlife.sz : textures.sync_dieonce.sz;
    if (sync_count > 0 && get_frame_index(&state_metadata) >= sync_count) {
        return STATE_FOREST_PLAYING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_talking_game_over(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_gameover)) forest_play(&audio.speak_gameover);
    }

    // Use sync data to determine when to transition
    if (textures.sync_gameover.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_gameover.sz) {
        return STATE_FOREST_END;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_wait_intro(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_start)) forest_play(&audio.speak_start);
    }

    // Use sync data to determine when to transition to playing
    if (textures.sync_start.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_start.sz) {
        return STATE_FOREST_PLAYING;
    }
    return STATE_FOREST_NONE;
}

ForestState process_forest_win_talking(InputState state) {
    (void)state;
    if (one_shot(&state_metadata, 0.0, 0)) {
        if (FOREST_SOUND_READY(audio.speak_levelcompleted)) forest_play(&audio.speak_levelcompleted);
    }

    // Use sync data to determine when to transition
    if (textures.sync_levelcompleted.sz > 0 && get_frame_index(&state_metadata) >= textures.sync_levelcompleted.sz) {
        return STATE_FOREST_END;
    }
    return STATE_FOREST_NONE;
}

// Legacy win / game over (kept for compatibility)

ForestState process_win(InputState state) {
    (void)state;
    if (get_state_time(&state_metadata) > 3.0) {
        return STATE_FOREST_END;
    }
    return STATE_FOREST_NONE;
}

ForestState process_game_over(InputState state) {
    (void)state;
    if (get_state_time(&state_metadata) > 5.0) {
        return STATE_FOREST_END;
    }
    return STATE_FOREST_NONE;
}


GameState process_forest(InputState state){

    if (state.debug_toggle) {
        debug_show_collisions = !debug_show_collisions;
        printf("Collision debug: %s\n", debug_show_collisions ? "ON" : "OFF");
    }

    ForestState next_state = STATE_FOREST_NONE;

    switch (current_forest_state) {
    case STATE_FOREST_WAIT_INTRO:
        next_state = process_forest_wait_intro(state);
        break;
    case STATE_FOREST_PLAYING:
        next_state = process_forest_playing(state);
        break;
    case STATE_FOREST_BRANCH_ANIMATION:
        next_state = process_forest_branch_animation(state);
        break;
    case STATE_FOREST_BRANCH_TALKING:
        next_state = process_forest_branch_talking(state);
        break;
    case STATE_FOREST_FLYING_START:
        next_state = process_forest_flying_start(state);
        break;
    case STATE_FOREST_FLYING_TALKING:
        next_state = process_forest_flying_talking(state);
        break;
    case STATE_FOREST_FLYING_FALLING:
        next_state = process_forest_flying_falling(state);
        break;
    case STATE_FOREST_FLYING_FALLING_HANG_ANIMATION:
        next_state = process_forest_flying_falling_hang_animation(state);
        break;
    case STATE_FOREST_FLYING_FALLING_HANG_TALKING:
        next_state = process_forest_flying_falling_hang_talking(state);
        break;
    case STATE_FOREST_ROCK_ANIMATION:
        next_state = process_forest_rock_animation(state);
        break;
    case STATE_FOREST_ROCK_HIT_ANIMATION:
        next_state = process_forest_rock_hit_animation(state);
        break;
    case STATE_FOREST_ROCK_TALKING:
        next_state = process_forest_rock_talking(state);
        break;
    case STATE_FOREST_TRAP_ANIMATION:
        next_state = process_forest_trap_animation(state);
        break;
    case STATE_FOREST_TRAP_TALKING:
        next_state = process_forest_trap_talking(state);
        break;
    case STATE_FOREST_SCYLLA_BUTTON:
        next_state = process_forest_scylla_button(state);
        break;
    case STATE_FOREST_TALKING_AFTER_HURT:
        next_state = process_forest_talking_after_hurt(state);
        break;
    case STATE_FOREST_TALKING_GAME_OVER:
        next_state = process_forest_talking_game_over(state);
        break;
    case STATE_FOREST_WIN_TALKING:
        next_state = process_forest_win_talking(state);
        break;
    default:
        break;
    }

     // Handle state transitions
    if (next_state != STATE_FOREST_NONE) {
        if (next_state == STATE_FOREST_END) {
            return FOREST_STATE_CAVE;
        }

        if (next_state == STATE_FOREST_PLAYING) {
            on_resume_forest_playing();
        }

        current_forest_state = next_state;
        reset_state(&state_metadata);
    }

    return FOREST_STATE_NONE;
}

void render_forest(){
    // States that need background and bottom rendered BEFORE (PRE)
    bool needs_pre_bg = false;
    bool needs_pre_bottom = false;
    // States that need bottom rendered AFTER (POST)
    bool needs_post_bottom = false;

    switch (current_forest_state) {
    case STATE_FOREST_WAIT_INTRO:
    case STATE_FOREST_PLAYING:
    case STATE_FOREST_TALKING_AFTER_HURT:
    case STATE_FOREST_TALKING_GAME_OVER:
    case STATE_FOREST_WIN_TALKING:
        needs_pre_bg = true;
        needs_pre_bottom = true;
        break;
    case STATE_FOREST_BRANCH_ANIMATION:
    case STATE_FOREST_BRANCH_TALKING:
    case STATE_FOREST_ROCK_ANIMATION:
    case STATE_FOREST_ROCK_HIT_ANIMATION:
    case STATE_FOREST_ROCK_TALKING:
        needs_post_bottom = true;
        break;
    default:
        // Flying and trap states: no background/bottom overlay
        break;
    }

    // Render PRE background
    if (needs_pre_bg) {
        render_forest_background();
    }

    // Render PRE bottom
    if (needs_pre_bottom) {
        render_forest_bottom();
    }

    // Render the state
    switch (current_forest_state) {
    case STATE_FOREST_WAIT_INTRO:
        render_forest_wait_intro();
        break;
    case STATE_FOREST_PLAYING:
        render_forest_playing_content();
        break;
    case STATE_FOREST_BRANCH_ANIMATION:
        render_forest_branch_animation();
        break;
    case STATE_FOREST_BRANCH_TALKING:
        render_forest_branch_talking();
        break;
    case STATE_FOREST_FLYING_START:
        render_forest_flying_start();
        break;
    case STATE_FOREST_FLYING_TALKING:
        render_forest_flying_talking();
        break;
    case STATE_FOREST_FLYING_FALLING:
        render_forest_flying_falling();
        break;
    case STATE_FOREST_FLYING_FALLING_HANG_ANIMATION:
        render_forest_flying_falling_hang_animation();
        break;
    case STATE_FOREST_FLYING_FALLING_HANG_TALKING:
        render_forest_flying_falling_hang_talking();
        break;
    case STATE_FOREST_ROCK_ANIMATION:
        render_forest_rock_animation();
        break;
    case STATE_FOREST_ROCK_HIT_ANIMATION:
        render_forest_rock_hit_animation();
        break;
    case STATE_FOREST_ROCK_TALKING:
        render_forest_rock_talking();
        break;
    case STATE_FOREST_TRAP_ANIMATION:
        render_forest_trap_animation();
        break;
    case STATE_FOREST_TRAP_TALKING:
        render_forest_trap_talking();
        break;
    case STATE_FOREST_SCYLLA_BUTTON:
        render_forest_scylla_button();
        break;
    case STATE_FOREST_TALKING_AFTER_HURT:
        render_forest_talking_after_hurt();
        break;
    case STATE_FOREST_TALKING_GAME_OVER:
        render_forest_talking_game_over();
        break;
    case STATE_FOREST_WIN_TALKING:
        render_forest_win_talking();
        break;
    default:
        break;
    }

    // Render POST bottom
    if (needs_post_bottom) {
        render_forest_bottom();
    }
}

void on_enter_forest(){
    current_forest_state = STATE_FOREST_WAIT_INTRO;
    reset_state(&state_metadata);
    init_game_context();
}

int get_forest_score() {
    return game_ctx.score;
}
