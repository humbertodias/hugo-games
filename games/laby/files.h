rawinfo labyintropic;
cgfinfo labyhugosync;
oosinfo laby01sync;
SDL_AudioSpec laby01spec;
SDL_AudioDeviceID laby01dev;
Uint32 laby01len;
Uint8 *laby01buf;
tilinfo labybaggr1;
tilinfo labybaggr2;
cgfinfo labyhugowalksright;
cgfinfo labyhugowalksleft;
cgfinfo labyhugojumpsright;
cgfinfo labyhugojumpsleft;
SDL_AudioSpec labyrun2spec;
SDL_AudioDeviceID labyrun2dev;
Uint32 labyrun2len;
Uint8 *labyrun2buf;

Uint32 LabyOpenFiles()
{
  labyintropic = LoadRAW(datapath("LabyrinthData/gfx/INTROPIC.RAW"));
  if (labyintropic.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/INTROPIC.RAW"));
    return 1;
  }
  labyhugosync = LoadCGF(datapath("LabyrinthData/gfx/Hugosync.cgf"));
  if (labyhugosync.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/Hugosync.cgf"));
    return 1;
  }
  laby01sync = LoadOOS(datapath("LabyrinthData/Sync/001-01.oos"));
  if (laby01sync.buf == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/Sync/001-01.oos"));
    return 1;
  }
  if (SDL_LoadWAV(datapath("LabyrinthData/Sfx/001-01.wav"),&laby01spec,&laby01buf,&laby01len) == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/Sfx/001-01.wav"));
    return 1;
  }
  laby01dev = SDL_OpenAudioDevice(NULL,0,&laby01spec,NULL,0);
  labybaggr1 = LoadTIL(datapath("LabyrinthData/gfx/Baggrund_m.Sand4.til"));
  if (labybaggr1.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/Baggrund_m.Sand4.til"));
    return 1;
  }
  labybaggr2 = LoadTIL(datapath("LabyrinthData/gfx/Baggrund_m.Sand&Skinner.til"));
  if (labybaggr2.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/Baggrund_m.Sand&Skinner.til"));
    return 1;
  }
  labyhugowalksright = LoadCGF(datapath("LabyrinthData/gfx/HugoWalksRight2.cgf"));
  if (labyhugowalksright.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/HugoWalksRight2.cgf"));
    return 1;
  }
  labyhugowalksleft = LoadCGF(datapath("LabyrinthData/gfx/HugoWalksLeft2.cgf"));
  if (labyhugowalksleft.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/HugoWalksLeft2.cgf"));
    return 1;
  }
  labyhugojumpsright = LoadCGF(datapath("LabyrinthData/gfx/HugoJumpsRight.cgf"));
  if (labyhugojumpsright.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/HugoJumpsRight.cgf"));
    return 1;
  }
  labyhugojumpsleft = LoadCGF(datapath("LabyrinthData/gfx/HugoJumpsLeft.cgf"));
  if (labyhugojumpsleft.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/gfx/HugoJumpsLeft.cgf"));
    return 1;
  }
  if (SDL_LoadWAV(datapath("LabyrinthData/Sfx/Run2-lp.wav"),&labyrun2spec,&labyrun2buf,&labyrun2len) == NULL)
  {
    printf("(Labyrinth Error)File not found: %s\n", datapath("LabyrinthData/Sfx/Run2-lp.wav"));
    return 1;
  }
  labyrun2dev = SDL_OpenAudioDevice(NULL,0,&labyrun2spec,NULL,0);
  return 0;
}

void LabyUnloadFiles()
{
  if (labyintropic.fp)
  {
    FreeRAW(labyintropic);
  }
  if (labyhugosync.fp)
  {
    FreeCGF(labyhugosync);
  }
  if (laby01sync.buf)
  {
    FreeOOS(laby01sync);
  }
  if (laby01dev)
  {
    SDL_CloseAudioDevice(laby01dev);
  }
  if (laby01buf)
  {
    SDL_FreeWAV(laby01buf);
  }
  if (labybaggr1.fp)
  {
    FreeTIL(labybaggr1);
  }
  if (labybaggr2.fp)
  {
    FreeTIL(labybaggr2);
  }
  if (labyhugowalksright.fp)
  {
    FreeCGF(labyhugowalksright);
  }
  if (labyhugowalksleft.fp)
  {
    FreeCGF(labyhugowalksleft);
  }
  if (labyhugojumpsright.fp)
  {
    FreeCGF(labyhugojumpsright);
  }
  if (labyhugojumpsleft.fp)
  {
    FreeCGF(labyhugojumpsleft);
  }
  if (labyrun2dev)
  {
    SDL_CloseAudioDevice(labyrun2dev);
  }
  if (labyrun2buf)
  {
    SDL_FreeWAV(labyrun2buf);
  }
}