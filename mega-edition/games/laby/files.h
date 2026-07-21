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
  labyintropic = LoadRAW("./data/LabyrinthData/gfx/INTROPIC.RAW");
  if (labyintropic.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/INTROPIC.RAW\n");
    return 1;
  }
  labyhugosync = LoadCGF("./data/LabyrinthData/gfx/Hugosync.cgf");
  if (labyhugosync.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/Hugosync.cgf\n");
    return 1;
  }
  laby01sync = LoadOOS("./data/LabyrinthData/Sync/001-01.oos");
  if (laby01sync.buf == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/Sync/001-01.oos\n");
    return 1;
  }
  if (SDL_LoadWAV("./data/LabyrinthData/Sfx/001-01.wav",&laby01spec,&laby01buf,&laby01len) == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/Sfx/001-01.wav\n");
    return 1;
  }
  laby01dev = SDL_OpenAudioDevice(NULL,0,&laby01spec,NULL,0);
  labybaggr1 = LoadTIL("./data/LabyrinthData/gfx/Baggrund_m.Sand4.til");
  if (labybaggr1.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/Baggrund_m.Sand4.til\n");
    return 1;
  }
  labybaggr2 = LoadTIL("./data/LabyrinthData/gfx/Baggrund_m.Sand&Skinner.til");
  if (labybaggr2.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/Baggrund_m.Sand&Skinner.til\n");
    return 1;
  }
  labyhugowalksright = LoadCGF("./data/LabyrinthData/gfx/HugoWalksRight2.cgf");
  if (labyhugowalksright.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/HugoWalksRight2.cgf\n");
    return 1;
  }
  labyhugowalksleft = LoadCGF("./data/LabyrinthData/gfx/HugoWalksLeft2.cgf");
  if (labyhugowalksleft.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/HugoWalksLeft2.cgf\n");
    return 1;
  }
  labyhugojumpsright = LoadCGF("./data/LabyrinthData/gfx/HugoJumpsRight.cgf");
  if (labyhugojumpsright.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/HugoJumpsRight.cgf\n");
    return 1;
  }
  labyhugojumpsleft = LoadCGF("./data/LabyrinthData/gfx/HugoJumpsLeft.cgf");
  if (labyhugojumpsleft.fp == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/gfx/HugoJumpsLeft.cgf\n");
    return 1;
  }
  if (SDL_LoadWAV("./data/LabyrinthData/Sfx/Run2-lp.wav",&labyrun2spec,&labyrun2buf,&labyrun2len) == NULL)
  {
    printf("(Labyrinth Error)File not found: ./data/LabyrinthData/Sfx/Run2-lp.wav\n");
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