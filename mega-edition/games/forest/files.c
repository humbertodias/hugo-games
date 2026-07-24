#include "files.h"
#include "audio.h"
#include <stdio.h>
#include <string.h>

static void forest_anim_init_til(ForestAnim *anim, const char *relpath, int start, int end, const char *sync_relpath)
{
  memset(anim, 0, sizeof(*anim));
  anim->is_til = 1;
  anim->til = LoadTIL(datapath(relpath));
  anim->start = start;
  anim->end = end;
  if (sync_relpath)
    anim->sync = LoadOOS(datapath(sync_relpath));
}

static void forest_anim_init_cgf(ForestAnim *anim, const char *relpath, int start, int end)
{
  memset(anim, 0, sizeof(*anim));
  anim->is_til = 0;
  anim->cgf = LoadCGF(datapath(relpath));
  anim->start = start;
  anim->end = end;
}

static void forest_load_sync(oosinfo *sync, const char *relpath)
{
  *sync = LoadOOS(datapath(relpath));
}

static int forest_load_sound_field(ForestSound *sound, const char *relpath)
{
  char *path = datapath(relpath);
  if (forest_load_sound(sound, path))
  {
    printf("(Forest Error)Could not load audio: %s (%s)\n", path, Mix_GetError());
    return 1;
  }
  return 0;
}

#define LOAD_CGF(field, relpath) \
  textures.field = LoadCGF(datapath(relpath)); \
  if (textures.field.fp == NULL) { \
    printf("(Forest Error)File not found: %s\n", datapath(relpath)); \
    return 1; \
  }

#define LOAD_SOUND(field, relpath) \
  if (forest_load_sound_field(&audio.field, relpath)) return 1

int ForestOpenFiles(void)
{
  char path[512];

  if (forest_audio_init())
    return 1;

  memset(&textures, 0, sizeof(textures));
  memset(&audio, 0, sizeof(audio));

  LOAD_CGF(hugo_side, "ForestData/gfx/hugoside.cgf");
  LOAD_CGF(hugo_jump, "ForestData/gfx/hugohop.cgf");
  LOAD_CGF(hugo_crawl, "ForestData/gfx/kravle.cgf");
  forest_anim_init_cgf(&textures.hugo_telllives, "ForestData/gfx/hugo_hello.cgf", 0, 15);
  LOAD_CGF(hugo_hand1, "ForestData/gfx/hand1.cgf");
  LOAD_CGF(hugo_hand2, "ForestData/gfx/hand2.cgf");

  LOAD_CGF(catapult, "ForestData/gfx/catapult.cgf");
  LOAD_CGF(trap, "ForestData/gfx/faelde.cgf");
  LOAD_CGF(rock, "ForestData/gfx/stone.cgf");
  LOAD_CGF(tree, "ForestData/gfx/branch-swing.cgf");
  LOAD_CGF(sack, "ForestData/gfx/saek.cgf");
  LOAD_CGF(lone_tree, "ForestData/gfx/lonetree.cgf");

  LOAD_CGF(bg_hillsday, "ForestData/gfx/hillsday.cgf");
  LOAD_CGF(bg_trees, "ForestData/gfx/paratrees.cgf");
  LOAD_CGF(bg_ground, "ForestData/gfx/paraground.cgf");
  LOAD_CGF(grass, "ForestData/gfx/GRASS.cgf");
  LOAD_CGF(leaves1, "ForestData/gfx/LEAVES1.cgf");
  LOAD_CGF(leaves2, "ForestData/gfx/LEAVES2.cgf");
  LOAD_CGF(end_mountain, "ForestData/gfx/WALL.cgf");
  LOAD_CGF(score_numbers, "ForestData/gfx/SCORES.cgf");
  LOAD_CGF(hugo_lives, "ForestData/gfx/HUGOSTAT.cgf");

  textures.scoreboard = SDL_LoadBMP(datapath("ForestData/gfx/SCOREBRD.bmp"));
  if (!textures.scoreboard)
  {
    printf("(Forest Error)File not found: %s\n", datapath("ForestData/gfx/SCOREBRD.bmp"));
    return 1;
  }
  {
    SDL_Surface *converted = SDL_ConvertSurfaceFormat(textures.scoreboard, SDL_PIXELFORMAT_RGBA32, 0);
    if (converted)
    {
      SDL_FreeSurface(textures.scoreboard);
      textures.scoreboard = converted;
    }
  }

  LOAD_CGF(arrows, "ForestData/gfx/arrows.cgf");

  textures.bg_gradient = SDL_LoadBMP(datapath("resources/fixed_assets/gradient.bmp"));
  if (!textures.bg_gradient)
    printf("(Forest Warning)File not found: %s\n", datapath("resources/fixed_assets/gradient.bmp"));

  forest_anim_init_til(&textures.hugohitlog, "ForestData/gfx/BRANCH-GROGGY.til", 0, 42, NULL);
  forest_anim_init_til(&textures.hugohitlog_talk, "ForestData/gfx/BRANCH-SPEAK.til", 0, 17, NULL);
  forest_anim_init_til(&textures.catapult_fly, "ForestData/gfx/HGKATFLY.til", 0, 113, NULL);
  forest_anim_init_til(&textures.catapult_fall, "ForestData/gfx/HGKATFLY.til", 115, 189, NULL);
  forest_anim_init_til(&textures.catapult_airtalk, "ForestData/gfx/CATAPULT-SPEAK.til", 0, 15, NULL);
  forest_anim_init_til(&textures.catapult_hang, "ForestData/gfx/HGKATHNG.TIL", 0, 12, NULL);
  forest_anim_init_cgf(&textures.catapult_hangspeak, "ForestData/gfx/hanging_mouth.cgf", 0, 11);
  forest_anim_init_til(&textures.hugo_lookrock, "ForestData/gfx/hugo-rock.til", 0, 14, NULL);
  forest_anim_init_til(&textures.hit_rock, "ForestData/gfx/HGROCK.TIL", 0, 60, NULL);
  forest_anim_init_til(&textures.hugo_traphurt, "ForestData/gfx/TRAP-HURTS.til", 0, 9, NULL);

  forest_load_sync(&textures.sync_start, "ForestData/Syncs/005-01.oos");
  forest_load_sync(&textures.sync_rock, "ForestData/Syncs/005-02.oos");
  forest_load_sync(&textures.sync_dieonce, "ForestData/Syncs/005-03.oos");
  forest_load_sync(&textures.sync_trap, "ForestData/Syncs/005-04.oos");
  forest_load_sync(&textures.sync_lastlife, "ForestData/Syncs/005-05.oos");
  forest_load_sync(&textures.sync_catapult_talktop, "ForestData/Syncs/005-08.oos");
  forest_load_sync(&textures.sync_catapult_hang, "ForestData/Syncs/005-10.oos");
  forest_load_sync(&textures.sync_hitlog, "ForestData/Syncs/005-11.oos");
  forest_load_sync(&textures.sync_gameover, "ForestData/Syncs/005-12.oos");
  forest_load_sync(&textures.sync_levelcompleted, "ForestData/Syncs/005-13.oos");

  forest_anim_init_til(&textures.cave_talks, "RopeOutroData/gfx/STAIRS.TIL", 0, 12,
    "RopeOutroData/Syncs/002-06.oos");
  forest_anim_init_til(&textures.cave_climbs, "RopeOutroData/gfx/STAIRS.TIL", 11, 51, NULL);
  forest_anim_init_til(&textures.cave_first_rope, "RopeOutroData/gfx/CASELIVE.TIL", 0, 32, NULL);
  forest_anim_init_til(&textures.cave_second_rope, "RopeOutroData/gfx/CASELIVE.TIL", 33, 72, NULL);
  forest_anim_init_til(&textures.cave_third_rope, "RopeOutroData/gfx/CASELIVE.TIL", 73, 121, NULL);
  forest_anim_init_til(&textures.cave_scylla_leaves, "RopeOutroData/gfx/CASELIVE.TIL", 122, 177, NULL);
  forest_anim_init_til(&textures.cave_scylla_bird, "RopeOutroData/gfx/CASELIVE.TIL", 178, 240, NULL);
  forest_anim_init_til(&textures.cave_scylla_ropes, "RopeOutroData/gfx/CASELIVE.TIL", 241, 283, NULL);
  forest_anim_init_til(&textures.cave_scylla_spring, "RopeOutroData/gfx/CASELIVE.TIL", 284, 318, NULL);
  forest_anim_init_til(&textures.cave_family_cage, "RopeOutroData/gfx/CASELIVE.TIL", 319, 352, NULL);
  forest_anim_init_til(&textures.cave_hugo_puff_first, "RopeOutroData/gfx/CASEDIE.TIL", 122, 166, NULL);
  forest_anim_init_til(&textures.cave_hugo_puff_second, "RopeOutroData/gfx/CASEDIE.TIL", 167, 211, NULL);
  forest_anim_init_til(&textures.cave_hugo_puff_third, "RopeOutroData/gfx/CASEDIE.TIL", 212, 256, NULL);
  forest_anim_init_til(&textures.cave_hugo_spring, "RopeOutroData/gfx/CASEDIE.TIL", 257, 295, NULL);
  forest_anim_init_til(&textures.cave_happy, "RopeOutroData/gfx/HAPPY.TIL", 0, 111, NULL);
  LOAD_CGF(cave_hugo_sprite, "RopeOutroData/gfx/hugo.cgf");
  LOAD_CGF(cave_score_font, "RopeOutroData/gfx/SCORE.cgf");

  LOAD_SOUND(speak_start, "ForestData/speaks/005-01.wav");
  LOAD_SOUND(speak_rock, "ForestData/speaks/005-02.wav");
  LOAD_SOUND(speak_dieonce, "ForestData/speaks/005-03.wav");
  LOAD_SOUND(speak_trap, "ForestData/speaks/005-04.wav");
  LOAD_SOUND(speak_lastlife, "ForestData/speaks/005-05.wav");
  LOAD_SOUND(speak_catapult_up, "ForestData/speaks/005-06.wav");
  LOAD_SOUND(speak_catapult_hit, "ForestData/speaks/005-07.wav");
  LOAD_SOUND(speak_catapult_talktop, "ForestData/speaks/005-08.wav");
  LOAD_SOUND(speak_catapult_down, "ForestData/speaks/005-09.wav");
  LOAD_SOUND(speak_catapult_hang, "ForestData/speaks/005-10.wav");
  LOAD_SOUND(speak_hitlog, "ForestData/speaks/005-11.wav");
  LOAD_SOUND(speak_gameover, "ForestData/speaks/005-12.wav");
  LOAD_SOUND(speak_levelcompleted, "ForestData/speaks/005-13.wav");

  LOAD_SOUND(sfx_bg_atmosphere, "ForestData/sfx/atmos-lp.wav");
  LOAD_SOUND(sfx_lightning_warning, "ForestData/sfx/warning.wav");
  LOAD_SOUND(sfx_hugo_knock, "ForestData/sfx/knock.wav");
  LOAD_SOUND(sfx_hugo_hittrap, "ForestData/sfx/crunch.wav");
  LOAD_SOUND(sfx_hugo_launch, "ForestData/sfx/skriid.wav");
  LOAD_SOUND(sfx_sack_normal, "ForestData/sfx/sack-norm.wav");
  LOAD_SOUND(sfx_sack_bonus, "ForestData/sfx/sack.wav");
  LOAD_SOUND(sfx_tree_swush, "ForestData/sfx/wush.wav");
  LOAD_SOUND(sfx_hugo_hitlog, "ForestData/sfx/bell.wav");
  LOAD_SOUND(sfx_catapult_eject, "ForestData/sfx/fjeder.wav");
  LOAD_SOUND(sfx_birds, "ForestData/sfx/birds-lp.wav");
  LOAD_SOUND(sfx_hugo_screenklir, "ForestData/sfx/klirr.wav");
  LOAD_SOUND(sfx_hugo_crash, "ForestData/sfx/kineser.wav");
  LOAD_SOUND(sfx_hugo_hangstart, "ForestData/sfx/knage-start.wav");
  LOAD_SOUND(sfx_hugo_hang, "ForestData/sfx/knage-lp.wav");
  for (int i = 0; i < 5; i++)
  {
    snprintf(path, sizeof(path), "ForestData/sfx/fumle%d.wav", i);
    if (forest_load_sound_field(&audio.sfx_hugo_walk[i], path))
      return 1;
  }

  LOAD_SOUND(cave_her_er_vi, "RopeOutroData/speak/002-05.wav");
  LOAD_SOUND(cave_trappe_snak, "RopeOutroData/speak/002-06.wav");
  LOAD_SOUND(cave_nu_kommer_jeg, "RopeOutroData/speak/002-07.wav");
  LOAD_SOUND(cave_afskylia_snak, "RopeOutroData/speak/002-08.wav");
  LOAD_SOUND(cave_hugo_katapult, "RopeOutroData/speak/002-09.wav");
  LOAD_SOUND(cave_hugo_skyd_ud, "RopeOutroData/speak/002-10.wav");
  LOAD_SOUND(cave_afskylia_skyd_ud, "RopeOutroData/speak/002-11.wav");
  LOAD_SOUND(cave_hugoline_tak, "RopeOutroData/speak/002-12.wav");
  LOAD_SOUND(cave_fodtrin1, "RopeOutroData/SFX/BA-15.WAV");
  LOAD_SOUND(cave_fodtrin2, "RopeOutroData/SFX/BA-16.WAV");
  LOAD_SOUND(cave_hiv_i_reb, "RopeOutroData/SFX/BA-17.WAV");
  LOAD_SOUND(cave_fjeder, "RopeOutroData/SFX/BA-18.WAV");
  LOAD_SOUND(cave_pre_puf, "RopeOutroData/SFX/BA-21.WAV");
  LOAD_SOUND(cave_puf, "RopeOutroData/SFX/BA-22.WAV");
  LOAD_SOUND(cave_tast_trykket, "RopeOutroData/SFX/BA-24.WAV");
  LOAD_SOUND(cave_pre_fanfare, "RopeOutroData/SFX/BA-101.WAV");
  LOAD_SOUND(cave_fanfare, "RopeOutroData/SFX/BA-102.WAV");
  LOAD_SOUND(cave_fugle_skrig, "RopeOutroData/SFX/BA-104.WAV");
  LOAD_SOUND(cave_trappe_grin, "RopeOutroData/SFX/HEXHAHA.WAV");
  LOAD_SOUND(cave_skrig, "RopeOutroData/SFX/SKRIG.WAV");
  LOAD_SOUND(cave_score_counter, "RopeOutroData/SFX/COUNTER.WAV");

  return 0;
}

#undef LOAD_CGF
#undef LOAD_SOUND

static void forest_free_cgf(cgfinfo *info)
{
  if (info->fp)
    FreeCGF(*info);
  memset(info, 0, sizeof(*info));
}

static void forest_free_anim(ForestAnim *anim)
{
  if (anim->is_til && anim->til.fp)
    FreeTIL(anim->til);
  else if (!anim->is_til && anim->cgf.fp)
    FreeCGF(anim->cgf);
  if (anim->sync.buf)
    FreeOOS(anim->sync);
  memset(anim, 0, sizeof(*anim));
}

static void forest_free_sync(oosinfo *sync)
{
  if (sync->buf)
    FreeOOS(*sync);
  memset(sync, 0, sizeof(*sync));
}

void ForestUnloadFiles(void)
{
  forest_pause_all();

  forest_free_cgf(&textures.hugo_side);
  forest_free_cgf(&textures.hugo_jump);
  forest_free_cgf(&textures.hugo_crawl);
  forest_free_anim(&textures.hugo_telllives);
  forest_free_cgf(&textures.hugo_hand1);
  forest_free_cgf(&textures.hugo_hand2);
  forest_free_cgf(&textures.catapult);
  forest_free_cgf(&textures.trap);
  forest_free_cgf(&textures.rock);
  forest_free_cgf(&textures.tree);
  forest_free_cgf(&textures.sack);
  forest_free_cgf(&textures.lone_tree);
  forest_free_cgf(&textures.bg_hillsday);
  forest_free_cgf(&textures.bg_trees);
  forest_free_cgf(&textures.bg_ground);
  forest_free_cgf(&textures.grass);
  forest_free_cgf(&textures.leaves1);
  forest_free_cgf(&textures.leaves2);
  forest_free_cgf(&textures.end_mountain);
  forest_free_cgf(&textures.score_numbers);
  forest_free_cgf(&textures.hugo_lives);
  forest_free_cgf(&textures.arrows);
  if (textures.scoreboard)
    SDL_FreeSurface(textures.scoreboard);
  forest_free_cgf(&textures.cave_hugo_sprite);
  forest_free_cgf(&textures.cave_score_font);

  if (textures.bg_gradient)
    SDL_FreeSurface(textures.bg_gradient);

  forest_free_anim(&textures.hugohitlog);
  forest_free_anim(&textures.hugohitlog_talk);
  forest_free_anim(&textures.catapult_fly);
  forest_free_anim(&textures.catapult_fall);
  forest_free_anim(&textures.catapult_airtalk);
  forest_free_anim(&textures.catapult_hang);
  forest_free_anim(&textures.catapult_hangspeak);
  forest_free_anim(&textures.hugo_lookrock);
  forest_free_anim(&textures.hit_rock);
  forest_free_anim(&textures.hugo_traphurt);

  forest_free_sync(&textures.sync_start);
  forest_free_sync(&textures.sync_rock);
  forest_free_sync(&textures.sync_dieonce);
  forest_free_sync(&textures.sync_trap);
  forest_free_sync(&textures.sync_lastlife);
  forest_free_sync(&textures.sync_catapult_talktop);
  forest_free_sync(&textures.sync_catapult_hang);
  forest_free_sync(&textures.sync_hitlog);
  forest_free_sync(&textures.sync_gameover);
  forest_free_sync(&textures.sync_levelcompleted);

  forest_free_anim(&textures.cave_talks);
  forest_free_anim(&textures.cave_climbs);
  forest_free_anim(&textures.cave_first_rope);
  forest_free_anim(&textures.cave_second_rope);
  forest_free_anim(&textures.cave_third_rope);
  forest_free_anim(&textures.cave_scylla_leaves);
  forest_free_anim(&textures.cave_scylla_bird);
  forest_free_anim(&textures.cave_scylla_ropes);
  forest_free_anim(&textures.cave_scylla_spring);
  forest_free_anim(&textures.cave_family_cage);
  forest_free_anim(&textures.cave_hugo_puff_first);
  forest_free_anim(&textures.cave_hugo_puff_second);
  forest_free_anim(&textures.cave_hugo_puff_third);
  forest_free_anim(&textures.cave_hugo_spring);
  forest_free_anim(&textures.cave_happy);

#define FREE_SOUND(field) forest_free_sound(&audio.field)
  FREE_SOUND(speak_start);
  FREE_SOUND(speak_rock);
  FREE_SOUND(speak_dieonce);
  FREE_SOUND(speak_trap);
  FREE_SOUND(speak_lastlife);
  FREE_SOUND(speak_catapult_up);
  FREE_SOUND(speak_catapult_hit);
  FREE_SOUND(speak_catapult_talktop);
  FREE_SOUND(speak_catapult_down);
  FREE_SOUND(speak_catapult_hang);
  FREE_SOUND(speak_hitlog);
  FREE_SOUND(speak_gameover);
  FREE_SOUND(speak_levelcompleted);
  FREE_SOUND(sfx_bg_atmosphere);
  FREE_SOUND(sfx_lightning_warning);
  FREE_SOUND(sfx_hugo_knock);
  FREE_SOUND(sfx_hugo_hittrap);
  FREE_SOUND(sfx_hugo_launch);
  FREE_SOUND(sfx_sack_normal);
  FREE_SOUND(sfx_sack_bonus);
  FREE_SOUND(sfx_tree_swush);
  FREE_SOUND(sfx_hugo_hitlog);
  FREE_SOUND(sfx_catapult_eject);
  FREE_SOUND(sfx_birds);
  FREE_SOUND(sfx_hugo_screenklir);
  FREE_SOUND(sfx_hugo_crash);
  FREE_SOUND(sfx_hugo_hangstart);
  FREE_SOUND(sfx_hugo_hang);
  for (int i = 0; i < 5; i++)
    forest_free_sound(&audio.sfx_hugo_walk[i]);
  FREE_SOUND(cave_her_er_vi);
  FREE_SOUND(cave_trappe_snak);
  FREE_SOUND(cave_nu_kommer_jeg);
  FREE_SOUND(cave_afskylia_snak);
  FREE_SOUND(cave_hugo_katapult);
  FREE_SOUND(cave_hugo_skyd_ud);
  FREE_SOUND(cave_afskylia_skyd_ud);
  FREE_SOUND(cave_hugoline_tak);
  FREE_SOUND(cave_fodtrin1);
  FREE_SOUND(cave_fodtrin2);
  FREE_SOUND(cave_hiv_i_reb);
  FREE_SOUND(cave_fjeder);
  FREE_SOUND(cave_pre_puf);
  FREE_SOUND(cave_puf);
  FREE_SOUND(cave_tast_trykket);
  FREE_SOUND(cave_pre_fanfare);
  FREE_SOUND(cave_fanfare);
  FREE_SOUND(cave_fugle_skrig);
  FREE_SOUND(cave_trappe_grin);
  FREE_SOUND(cave_skrig);
  FREE_SOUND(cave_score_counter);
#undef FREE_SOUND

  memset(&textures, 0, sizeof(textures));
  memset(&audio, 0, sizeof(audio));
  forest_audio_quit();
}
