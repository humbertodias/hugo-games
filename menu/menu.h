Uint8 defaultpal[768];
rawinfo mainpix;
rawinfo hugologo;
cgfinfo arrows;
cgfinfo film;
SDL_AudioSpec wavspec;
SDL_AudioDeviceID wavdev;
Uint32 wavlen;
Uint8 *wavbuf;
SDL_AudioSpec filmmspec;
SDL_AudioDeviceID filmmdev;
Uint32 filmmlen;
Uint8 *filmmbuf;
SDL_AudioSpec filmsspec;
SDL_AudioDeviceID filmsdev;
Uint32 filmslen;
Uint8 *filmsbuf;
Uint32 numgame = 0;
rawinfo gameraw[21];
Uint32 TintLeft = 1, TintRight = 4, TintUp = 1, TintDown = 1;
cgfinfo creditfont;
cgfinfo menufont;
cgfinfo ultrafont;
char *creditmap;
char *menumap;
char *ultramap;
FILE *charmap;
FILE *txt;
char gamesname[21][255];
char temptxt[0x200];
char menuname[5][255];
Uint8 selectmenu = 0;
Uint32 nummenu = 0;
SDL_AudioSpec cursspec;
SDL_AudioDeviceID cursdev;
Uint32 curslen;
Uint8 *cursbuf;
Uint8 gamemode = 0, playermode = 0;
cgfinfo modeicon,playersicon;
SDL_AudioSpec toggspec;
SDL_AudioDeviceID toggdev;
Uint32 togglen;
Uint8 *toggbuf;
SDL_AudioSpec wipespec;
SDL_AudioDeviceID wipedev;
Uint32 wipelen;
Uint8 *wipebuf;
rawinfo optionraw[5];
char optionsname[21][255];
char optdisplay[3][255];
char minigames[8][255];
char itework[5][255];
char programmers[9][255];
char graphics[8][255];
char sounds[2][255];
char producers[1][255];
char eproducers[1][255];
rawinfo hisc;
rawinfo hiscback;
cgfinfo hiscreens;
char hiclean[2][255];

int OpenFiles()
{
  char tmps[10];
  FILE *pal;
  Uint32 i,j,k,tsz;
  pal = fopen("./data/MenuData/mainpal.pal","rb");
  if (pal == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/mainpal.pal\n");
    return 1;
  }
  fread(tmps,sizeof(Uint8),10,pal);
  if (strncmp("CPAL768STD",tmps,10))
  {
    fclose(pal);
    printf("(Menu Error)Unknow format of palette: ./data/MenuData/mainpal.pal\n");
    return 1;
  }
  fseek(pal,10,SEEK_SET);
  fread(defaultpal,sizeof(Uint8),768,pal);
  fclose(pal);
  mainpix = LoadRAW("./data/MenuData/mainpix.raw");
  if (mainpix.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/mainpix.raw\n");
    return 1;
  }
  hugologo = LoadRAW("./data/MenuData/hugologo.raw");
  if (hugologo.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/hugologo.raw\n");
    return 1;
  }
  arrows = LoadCGF("./data/MenuData/Arrows.cgf");
  if (arrows.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Arrows.cgf\n");
    return 1;
  }
  arrows.head.numpal = 0x100;
  arrows.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    arrows.pal[i*4] = defaultpal[i*3];
    arrows.pal[i*4+1] = defaultpal[i*3+1];
    arrows.pal[i*4+2] = defaultpal[i*3+2];
  }
  film = LoadCGF("./data/MenuData/Film.cgf");
  if (film.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Film.cgf\n");
    return 1;
  }
  film.head.numpal = 0x100;
  film.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    film.pal[i*4] = defaultpal[i*3];
    film.pal[i*4+1] = defaultpal[i*3+1];
    film.pal[i*4+2] = defaultpal[i*3+2];
  }
  if (SDL_LoadWAV("./data/MenuData/Menyhed-df.wav",&wavspec,&wavbuf,&wavlen) == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Menyhed-df.wav\n");
    return 1;
  }
  wavdev = SDL_OpenAudioDevice(NULL,0,&wavspec,NULL,0);
  if (SDL_LoadWAV("./data/MenuData/Filmmove.wav",&filmmspec,&filmmbuf,&filmmlen) == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Filmmove.wav\n");
    return 1;
  }
  if (SDL_LoadWAV("./data/MenuData/Filmstop.wav",&filmsspec,&filmsbuf,&filmslen) == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Filmstop.wav\n");
    return 1;
  }
  filmmdev = SDL_OpenAudioDevice(NULL,0,&filmmspec,NULL,0);
  filmsdev = SDL_OpenAudioDevice(NULL,0,&filmsspec,NULL,0);
  gameraw[0] = LoadRAW("./data/MenuData/LABY.RAW");
  if (gameraw[0].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/LABY.RAW\n");
    return 1;
  }
  gameraw[1] = LoadRAW("./data/MenuData/train.raw");
  if (gameraw[1].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/train.raw\n");
    return 1;
  }
  gameraw[2] = LoadRAW("./data/MenuData/plane.raw");
  if (gameraw[2].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/plane.raw\n");
    return 1;
  }
  gameraw[3] = LoadRAW("./data/MenuData/forest.raw");
  if (gameraw[3].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/forest.raw\n");
    return 1;
  }
  gameraw[4] = LoadRAW("./data/MenuData/mountain.raw");
  if (gameraw[4].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/mountain.raw\n");
    return 1;
  }
  gameraw[5] = LoadRAW("./data/MenuData/minetrack.raw");
  if (gameraw[5].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/minetrack.raw\n");
    return 1;
  }
  gameraw[6] = LoadRAW("./data/MenuData/SCUBA.RAW");
  if (gameraw[6].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/SCUBA.RAW\n");
    return 1;
  }
  gameraw[7] = LoadRAW("./data/MenuData/icecav.raw");
  if (gameraw[7].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/icecav.raw\n");
    return 1;
  }
  gameraw[8] = LoadRAW("./data/MenuData/lumberjack.raw");
  if (gameraw[8].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/lumberjack.raw\n");
    return 1;
  }
  gameraw[9] = LoadRAW("./data/MenuData/skateboard.raw");
  if (gameraw[9].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/skateboard.raw\n");
    return 1;
  }
  gameraw[10] = LoadRAW("./data/MenuData/screen001.raw");
  if (gameraw[10].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen001.raw\n");
    return 1;
  }
  gameraw[11] = LoadRAW("./data/MenuData/screen002.raw");
  if (gameraw[11].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen002.raw\n");
    return 1;
  }
  gameraw[12] = LoadRAW("./data/MenuData/moore.raw");
  if (gameraw[12].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/moore.raw\n");
    return 1;
  }
  gameraw[13] = LoadRAW("./data/MenuData/screen000.raw");
  if (gameraw[13].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen000.raw\n");
    return 1;
  }
  gameraw[14] = LoadRAW("./data/MenuData/screen003.raw");
  if (gameraw[14].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen003.raw\n");
    return 1;
  }
  gameraw[15] = LoadRAW("./data/MenuData/screen005.raw");
  if (gameraw[15].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen005.raw\n");
    return 1;
  }
  gameraw[16] = LoadRAW("./data/MenuData/screen007.raw");
  if (gameraw[16].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen007.raw\n");
    return 1;
  }
  gameraw[17] = LoadRAW("./data/MenuData/screen004.raw");
  if (gameraw[17].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen004.raw\n");
    return 1;
  }
  gameraw[18] = LoadRAW("./data/MenuData/screen006.raw");
  if (gameraw[18].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/screen006.raw\n");
    return 1;
  }
  gameraw[19] = LoadRAW("./data/MenuData/stones.raw");
  if (gameraw[19].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/stones.raw\n");
    return 1;
  }
  gameraw[20] = LoadRAW("./data/MenuData/river.raw");
  if (gameraw[20].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/river.raw\n");
    return 1;
  }
  ultrafont = LoadCGF("./data/MenuData/UltraFont.cgf");
  if (ultrafont.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/UltraFont.cgf\n");
    return 1;
  }
  ultramap = (char *)malloc(sizeof(char)*ultrafont.head.num);
  charmap = fopen("./data/MenuData/charmapultra.txt","rb");
  if (charmap == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/charmapultra.txt\n");
    return 1;
  }
  fread(ultramap,sizeof(char),ultrafont.head.num,charmap);
  fclose(charmap);
  creditfont = LoadCGF("./data/MenuData/CreditFont.cgf");
  if (creditfont.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/CreditFont.cgf\n");
    return 1;
  }
  creditmap = (char *)malloc(sizeof(char)*creditfont.head.num);
  charmap = fopen("./data/MenuData/charmapcredit.txt","rb");
  if (charmap == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/charmapcredit.txt\n");
    return 1;
  }
  fread(creditmap,sizeof(char),creditfont.head.num,charmap);
  fclose(charmap);
  menufont = LoadCGF("./data/MenuData/MenuFont.cgf");
  if (menufont.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/MenuFont.cgf\n");
    return 1;
  }
  menumap = (char *)malloc(sizeof(char)*menufont.head.num);
  charmap = fopen("./data/MenuData/charmapmenu.txt","rb");
  if (charmap == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/charmapmenu.txt\n");
    return 1;
  }
  fread(menumap,sizeof(char),menufont.head.num,charmap);
  fclose(charmap);
  ultrafont.head.numpal = 0x100;
  ultrafont.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    ultrafont.pal[i*4] = defaultpal[i*3];
    ultrafont.pal[i*4+1] = defaultpal[i*3+1];
    ultrafont.pal[i*4+2] = defaultpal[i*3+2];
  }
  creditfont.head.numpal = 0x100;
  creditfont.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    creditfont.pal[i*4] = defaultpal[i*3];
    creditfont.pal[i*4+1] = defaultpal[i*3+1];
    creditfont.pal[i*4+2] = defaultpal[i*3+2];
  }
  for (i = 0; i<creditfont.head.num; ++i)
  {
    creditfont.data[i].posx = 0;
  }
  menufont.head.numpal = 0x100;
  menufont.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    menufont.pal[i*4] = defaultpal[i*3];
    menufont.pal[i*4+1] = defaultpal[i*3+1];
    menufont.pal[i*4+2] = defaultpal[i*3+2];
  }
  for (i = 0; i<menufont.head.num; ++i)
  {
    menufont.data[i].posx = 0;
  }
  for (i = 0; i<ultrafont.head.num; ++i)
  {
    ultrafont.data[i].posx = 0;
  }
  txt = fopen("./data/MenuData/games.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/games.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<21; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      gamesname[i][k] = temptxt[j];
      k++;
      j++;
    }
    gamesname[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/menutext.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/menutext.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<5; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      menuname[i][k] = temptxt[j];
      k++;
      j++;
    }
    menuname[i][k] = 0;
    j++;
  }
  if (SDL_LoadWAV("./data/MenuData/Cursor.wav",&cursspec,&cursbuf,&curslen) == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Cursor.wav\n");
    return 1;
  }
  cursdev = SDL_OpenAudioDevice(NULL,0,&cursspec,NULL,0);
  modeicon = LoadCGF("./data/MenuData/ModeIcon.cgf");
  if (modeicon.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/ModeIcon.cgf\n");
    return 1;
  }
  modeicon.head.numpal = 0x100;
  modeicon.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    modeicon.pal[i*4] = defaultpal[i*3];
    modeicon.pal[i*4+1] = defaultpal[i*3+1];
    modeicon.pal[i*4+2] = defaultpal[i*3+2];
  }
  playersicon = LoadCGF("./data/MenuData/PlayersIcon.cgf");
  if (playersicon.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/PlayersIcon.cgf\n");
    return 1;
  }
  playersicon.head.numpal = 0x100;
  playersicon.pal = (Uint8 *)malloc(sizeof(Uint8)*0x400);
  for (i = 0; i<0x100; ++i)
  {
    playersicon.pal[i*4] = defaultpal[i*3];
    playersicon.pal[i*4+1] = defaultpal[i*3+1];
    playersicon.pal[i*4+2] = defaultpal[i*3+2];
  }
  if (SDL_LoadWAV("./data/MenuData/Toggle.wav",&toggspec,&toggbuf,&togglen) == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Toggle.wav\n");
    return 1;
  }
  toggdev = SDL_OpenAudioDevice(NULL,0,&toggspec,NULL,0);
  if (SDL_LoadWAV("./data/MenuData/Wipe.wav",&wipespec,&wipebuf,&wipelen) == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/Wipe.wav\n");
    return 1;
  }
  wipedev = SDL_OpenAudioDevice(NULL,0,&wipespec,NULL,0);
  optionraw[0] = LoadRAW("./data/MenuData/keyboard.raw");
  if (optionraw[0].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/keyboard.raw\n");
    return 1;
  }
  optionraw[1] = LoadRAW("./data/MenuData/credits.raw");
  if (optionraw[1].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/credits.raw\n");
    return 1;
  }
  optionraw[2] = LoadRAW("./data/MenuData/intro_outro.raw");
  if (optionraw[2].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/intro_outro.raw\n");
    return 1;
  }
  optionraw[3] = LoadRAW("./data/MenuData/pad.raw");
  if (optionraw[3].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/pad.raw\n");
    return 1;
  }
  optionraw[4] = LoadRAW("./data/MenuData/volume.raw");
  if (optionraw[4].fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/volume.raw\n");
    return 1;
  }
  txt = fopen("./data/MenuData/options.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/options.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<5; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      optionsname[i][k] = temptxt[j];
      k++;
      j++;
    }
    optionsname[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/display.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/display.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<3; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      optdisplay[i][k] = temptxt[j];
      k++;
      j++;
    }
    optdisplay[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/minigames.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/minigames.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<8; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      minigames[i][k] = temptxt[j];
      k++;
      j++;
    }
    minigames[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/itework.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/itework.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<5; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      itework[i][k] = temptxt[j];
      k++;
      j++;
    }
    itework[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/programmers.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/programmers.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<9; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      programmers[i][k] = temptxt[j];
      k++;
      j++;
    }
    programmers[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/graphics.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/graphics.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<8; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      graphics[i][k] = temptxt[j];
      k++;
      j++;
    }
    graphics[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/sound.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/sound.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<2; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      sounds[i][k] = temptxt[j];
      k++;
      j++;
    }
    sounds[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/producer.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/producer.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<1; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      producers[i][k] = temptxt[j];
      k++;
      j++;
    }
    producers[i][k] = 0;
    j++;
  }
  txt = fopen("./data/MenuData/eproducer.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/eproducer.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<1; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      eproducers[i][k] = temptxt[j];
      k++;
      j++;
    }
    eproducers[i][k] = 0;
    j++;
  }
  hisc = LoadRAW("./data/MenuData/highscore.raw");
  if (hisc.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/highscore.raw\n");
    return 1;
  }
  hiscback = LoadRAW("./data/MenuData/hiscoreback.raw");
  if (hiscback.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/hiscoreback.raw\n");
    return 1;
  }
  hiscreens = LoadCGF("./data/MenuData/hiscreens.cgf");
  if (hiscreens.fp == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/hiscreens.cgf\n");
    return 1;
  }
  txt = fopen("./data/MenuData/hiclean.txt","rb");
  if (txt == NULL)
  {
    printf("(Menu Error)File not found: ./data/MenuData/hiclean.txt\n");
    return 1;
  }
  tsz = fread(temptxt,sizeof(char),0x200,txt);
  fclose(txt);
  j = 0;
  for (i = 0; i<2; ++i)
  {
    k = 0;
    while ((temptxt[j]!=0x0A)&&(j<tsz))
    {
      hiclean[i][k] = temptxt[j];
      k++;
      j++;
    }
    hiclean[i][k] = 0;
    j++;
  }
  return 0;
}

void UnloadFiles()
{
  Uint32 i;
  if (mainpix.fp)
  {
    FreeRAW(mainpix);
  }
  if (hugologo.fp)
  {
    FreeRAW(hugologo);
  }
  if (arrows.fp)
  {
    FreeCGF(arrows);
  }
  if (film.fp)
  {
    FreeCGF(film);
  }
  if (wavdev)
  {
    SDL_CloseAudioDevice(wavdev);
  }
  if (wavbuf)
  {
    SDL_FreeWAV(wavbuf);
  }
  if (filmmbuf)
  {
    SDL_FreeWAV(filmmbuf);
  }
  if (filmsdev)
  {
    SDL_CloseAudioDevice(filmmdev);
  }
  if (filmmdev)
  {
    SDL_CloseAudioDevice(filmmdev);
  }
  for (i = 0; i<21; ++i)
  {
    if (gameraw[i].fp)
    {
      FreeRAW(gameraw[i]);
    }
  }
  if (ultrafont.fp)
  {
    FreeCGF(ultrafont);
  }
  if (creditfont.fp)
  {
    FreeCGF(creditfont);
  }
  if (menufont.fp)
  {
    FreeCGF(menufont);
  }
  if (ultramap)
  {
    free(ultramap);
  }
  if (creditmap)
  {
    free(creditmap);
  }
  if (menumap)
  {
    free(menumap);
  }
  if (cursdev)
  {
    SDL_CloseAudioDevice(cursdev);
  }
  if (cursbuf)
  {
    SDL_FreeWAV(cursbuf);
  }
  if (modeicon.fp)
  {
    FreeCGF(modeicon);
  }
  if (playersicon.fp)
  {
    FreeCGF(playersicon);
  }
  if (toggdev)
  {
    SDL_CloseAudioDevice(toggdev);
  }
  if (toggbuf)
  {
    SDL_FreeWAV(toggbuf);
  }
  if (wipedev)
  {
    SDL_CloseAudioDevice(wipedev);
  }
  if (wipebuf)
  {
    SDL_FreeWAV(wipebuf);
  }
  for (i = 0; i<5; ++i)
  {
    if (optionraw[i].fp)
    {
      FreeRAW(optionraw[i]);
    }
  }
  if (hisc.fp)
  {
    FreeRAW(hisc);
  }
  if (hiscback.fp)
  {
    FreeRAW(hiscback);
  }
  if (hiscreens.fp)
  {
    FreeCGF(hiscreens);
  }
}

void printtext(cgfinfo font, char *charmaps, char *str, Uint32 w, Uint32 h, Uint32 sz, SDL_Surface *screen, Uint32 Tint, Uint8 anti)
{
  Uint32 i, j, sm = 0;
  char tmp;
  for (i = 0; i<font.head.num; ++i)
    font.data[i].posx = 0;
  MoveRightCGF(&font,w);
  MoveDownCGF(&font,h);
  for (i = 0; i<sz; ++i)
  {
    tmp = str[i];
    if ((tmp>='a')&&(tmp<='z'))
      tmp = tmp+'A'-'a';
    j = 0;
    while ((tmp!=charmaps[j])&&(j<font.head.num))
    {
      j++;
    }
    if (j == font.head.num)
    {
      if (tmp != 0x20)
        printf("Warning: symbol 0x%X not found\n",tmp);
      MoveRightCGF(&font,font.data[0].width);
      sm = sm+font.data[0].width;
    }
    else
    {
      DrawCGF(font,j,0,screen,Tint,anti);
      MoveRightCGF(&font,font.data[j].width);
      sm = sm+font.data[j].width;
    }
  }
  MoveLeftCGF(&font,w+sm);
  MoveUpCGF(&font,h);
}

Uint32 widthtext(cgfinfo font, char *charmaps, char *str, Uint32 sz)
{
  Uint32 i, j, sm = 0;
  char tmp;
  for (i = 0; i<sz; ++i)
  {
    tmp = str[i];
    if ((tmp>='a')&&(tmp<='z'))
      tmp = tmp+'A'-'a';
    j = 0;
    while ((tmp!=charmaps[j])&&(j<font.head.num))
    {
      j++;
    }
    if (j == font.head.num)
    {
      if (tmp != 0x20)
        printf("Warning: symbol 0x%X not found\n",tmp);
      sm = sm+font.data[0].width;
    }
    else
    {
      sm = sm+font.data[j].width;
    }
  }
  return sm;
}

void listfilm (SDL_Surface *screen, SDL_Surface *bkgr, SDL_Window *window, Uint32 pos)
{
  Uint32 Timer, i,Timefilm;
  SDL_ClearQueuedAudio(filmmdev);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_PauseAudioDevice(filmmdev,0);
  Timefilm = SDL_GetTicks();
  while (film.data[0].posy!=0xD6-numgame*0x80)
  {
    Timer = SDL_GetTicks();
    if (pos == 1)
      MoveUpCGF(&film,0x10);
    else
      MoveDownCGF(&film,0x10);
    SDL_BlitSurface(bkgr,NULL,screen,NULL);
    DrawCGF(arrows,0,0,screen,TintDown,0);
    DrawCGF(arrows,2,0,screen,TintLeft,0);
    DrawCGF(arrows,4,0,screen,TintRight,0);
    DrawCGF(arrows,6,0,screen,TintUp,0);
    for (i = 0; i<21; ++i)
    {
      if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
      {
        if (gameraw[i].fp)
        {
          DrawRAW(gameraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
          printtext(ultrafont,ultramap,gamesname[i],0x1BA,film.data[0].posy+4,strlen(gamesname[i]),screen,1,0);
        }
        DrawCGF(film,0,0,screen,1,0);
      }
      MoveDownCGF(&film,0x80);
    }
    MoveUpCGF(&film,0x80*21);
    for (i = 0; i<5; ++i)
    {
      printtext(menufont,menumap,menuname[i],0x14,0xA8+i*0x2A,strlen(menuname[i]),screen,1,1);
    }
    DrawCGF(modeicon,gamemode,0,screen,1,0);
    DrawCGF(playersicon,playermode,0,screen,1,0);
    SDL_UpdateWindowSurface(window);
    while(SDL_GetTicks()-Timer<30)
    {
      if (SDL_GetTicks()-Timefilm>=(filmmlen*1000)/filmmspec.freq)
      {
        //SDL_ClearQueuedAudio(filmmdev);
        SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
        SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
        Timefilm = SDL_GetTicks();
      }
    }
    if (SDL_GetTicks()-Timefilm>=(filmmlen*1000)/filmmspec.freq)
    {
      //SDL_ClearQueuedAudio(filmmdev);
      SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
      SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
      Timefilm = SDL_GetTicks();
    }
  }
  SDL_PauseAudioDevice(filmmdev,1);
  SDL_ClearQueuedAudio(filmmdev);
  SDL_QueueAudio(filmsdev,filmsbuf,filmslen);
  SDL_PauseAudioDevice(filmsdev,0);
}

void hidefilm (SDL_Surface *screen, SDL_Surface *bkgr, SDL_Window *window)
{
  Uint32 Timer, i,Timefilm;
  SDL_ClearQueuedAudio(filmmdev);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_PauseAudioDevice(filmmdev,0);
  Timefilm = SDL_GetTicks();
  while (film.data[0].posy!=0x1F6)
  {
    Timer = SDL_GetTicks();
    MoveDownCGF(&film,0x10);
    SDL_BlitSurface(bkgr,NULL,screen,NULL);
    DrawCGF(arrows,0,0,screen,TintDown,0);
    DrawCGF(arrows,2,0,screen,TintLeft,0);
    DrawCGF(arrows,4,0,screen,TintRight,0);
    DrawCGF(arrows,6,0,screen,TintUp,0);
    for (i = 0; i<21; ++i)
    {
      if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
      {
        if (gameraw[i].fp)
        {
          DrawRAW(gameraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
          printtext(ultrafont,ultramap,gamesname[i],0x1BA,film.data[0].posy+4,strlen(gamesname[i]),screen,1,0);
        }
        DrawCGF(film,0,0,screen,1,0);
      }
      MoveDownCGF(&film,0x80);
    }
    MoveUpCGF(&film,0x80*21);
    for (i = 0; i<5; ++i)
    {
      printtext(menufont,menumap,menuname[i],0x14,0xA8+i*0x2A,strlen(menuname[i]),screen,1,1);
    }
    DrawCGF(modeicon,gamemode,0,screen,1,0);
    DrawCGF(playersicon,playermode,0,screen,1,0);
    SDL_UpdateWindowSurface(window);
    while(SDL_GetTicks()-Timer<30)
    {
      if (SDL_GetTicks()-Timefilm>=(filmmlen*1000)/filmmspec.freq)
      {
        //SDL_ClearQueuedAudio(filmmdev);
        SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
        SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
        Timefilm = SDL_GetTicks();
      }
    }
    if (SDL_GetTicks()-Timefilm>=(filmmlen*1000)/filmmspec.freq)
    {
      //SDL_ClearQueuedAudio(filmmdev);
      SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
      SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
      Timefilm = SDL_GetTicks();
    }
  }
  SDL_PauseAudioDevice(filmmdev,1);
  SDL_ClearQueuedAudio(filmmdev);
  SDL_QueueAudio(filmsdev,filmsbuf,filmslen);
  SDL_PauseAudioDevice(filmsdev,0);
}

#include "options.h"
#include "hiscore.h"
#include "../games/laby/start.h"
#include "../games/forest/start.h"
#include "../games/forest/forest_impl.h"

int StartMenu(SDL_Window *window)
{
  Uint32 opn = OpenFiles();
  Uint32 i, Timewav;
  Uint32 exit = 0;
  Uint32 score;
  SDL_Surface *screen = SDL_GetWindowSurface(window);
  SDL_Surface *bkgr;
  SDL_Event e;
  if (opn)
  {
    UnloadFiles();
    return opn;
  }
  bkgr = SDL_CreateRGBSurface(0,640,480,32,0,0,0,0);
  DrawRAW(mainpix,0,0,0,0,0,bkgr,1);
  DrawRAW(hugologo,0x0A,0x0D,0,0,0,bkgr,1);
  SDL_BlitSurface(bkgr,NULL,screen,NULL);
  MoveRightCGF(&arrows,0x102);
  MoveDownCGF(&arrows,0x140);
  MoveRightCGF(&film,0x1A2);
  MoveDownCGF(&film,0x1F6);
  MoveRightCGF(&modeicon,0x16C);
  MoveDownCGF(&modeicon,0xAC);
  MoveRightCGF(&playersicon,0x16C);
  MoveDownCGF(&playersicon,0xD9);
  DrawCGF(arrows,0,0,screen,TintDown,0);
  DrawCGF(arrows,2,0,screen,TintLeft,0);
  DrawCGF(arrows,4,0,screen,TintRight,0);
  DrawCGF(arrows,6,0,screen,TintUp,0);
  for (i = 0; i<21; ++i)
  {
    if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
    {
      DrawCGF(film,0,0,screen,1,0);
      if (gameraw[i].fp)
      {
        if ((i == numgame)&&(!selectmenu))
          DrawRAW(gameraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,1);
        else
          DrawRAW(gameraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
        printtext(ultrafont,ultramap,gamesname[i],0x1BA,film.data[0].posy+4,strlen(gamesname[i]),screen,1,0);
      }
    }
    MoveDownCGF(&film,0x80);
  }
  MoveUpCGF(&film,0x80*21);
  for (i = 0; i<5; ++i)
  {
    printtext(menufont,menumap,menuname[i],0x14,0xA8+i*0x2A,strlen(menuname[i]),screen,1,1);
  }
  DrawCGF(modeicon,gamemode,0,screen,1,0);
  DrawCGF(playersicon,playermode,0,screen,1,0);
  SDL_UpdateWindowSurface(window);
  SDL_QueueAudio(wavdev,wavbuf,wavlen);
  Timewav = SDL_GetTicks();
  SDL_PauseAudioDevice(wavdev,0);
  listfilm(screen,bkgr,window,1);
  while (SDL_PollEvent(&e));
  while (!exit)
  {
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          selectmenu = 1;
          nummenu = 4;
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN)||(e.key.keysym.scancode == SDL_SCANCODE_KP_2))
        {
          if (!selectmenu)
          {
            TintUp = 4;
            TintLeft = 4;
            if (numgame == 20)
            {
              numgame = 0;
              listfilm(screen,bkgr,window,2);
            }
            else
            {
              numgame++;
              listfilm(screen,bkgr,window,1);
            }
            TintUp = 1;
            TintLeft = 1;
          }
          else
          {
            SDL_ClearQueuedAudio(cursdev);
            SDL_QueueAudio(cursdev,cursbuf,curslen);
            SDL_PauseAudioDevice(cursdev,0);
            if (nummenu == 4)
            {
              nummenu = 0;
            }
            else
            {
              nummenu++;
            }
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_UP)||(e.key.keysym.scancode == SDL_SCANCODE_KP_8))
        {
          if (!selectmenu)
          {
            TintDown = 4;
            TintLeft = 4;
            if (numgame == 0)
            {
              numgame = 20;
              listfilm(screen,bkgr,window,1);
            }
            else
            {
              numgame--;
              listfilm(screen,bkgr,window,2);
            }
            TintDown = 1;
            TintLeft = 1;
          }
          else
          {
            SDL_ClearQueuedAudio(cursdev);
            SDL_QueueAudio(cursdev,cursbuf,curslen);
            SDL_PauseAudioDevice(cursdev,0);
            if (nummenu == 0)
            {
              nummenu = 4;
            }
            else
            {
              nummenu--;
            }
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT)||(e.key.keysym.scancode == SDL_SCANCODE_KP_4))
        {
          if (!selectmenu)
          {
            selectmenu = 1;
            TintLeft = 4;
            TintRight = 1;
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT)||(e.key.keysym.scancode == SDL_SCANCODE_KP_6))
        {
          if (selectmenu)
          {
            selectmenu = 0;
            TintLeft = 1;
            TintRight = 4;
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_RETURN)||(e.key.keysym.scancode == SDL_SCANCODE_KP_ENTER))
        {
          if (selectmenu)
          {
            if (nummenu == 0)
            {
              SDL_ClearQueuedAudio(toggdev);
              SDL_QueueAudio(toggdev,toggbuf,togglen);
              SDL_PauseAudioDevice(toggdev,0);
              if (gamemode)
              {
                gamemode = 0;
              }
              else
              {
                gamemode = 1;
              }
            }
            else if (nummenu == 1)
            {
              SDL_ClearQueuedAudio(toggdev);
              SDL_QueueAudio(toggdev,toggbuf,togglen);
              SDL_PauseAudioDevice(toggdev,0);
              if (playermode)
              {
                playermode = 0;
              }
              else
              {
                playermode = 1;
              }
            }
            else if (nummenu == 2)
            {
              MoveLeftCGF(&arrows,0x102);
              MoveUpCGF(&arrows,0x140);
              SDL_ClearQueuedAudio(wipedev);
              SDL_QueueAudio(wipedev,wipebuf,wipelen);
              SDL_PauseAudioDevice(wipedev,0);
              starthiscore(window, &Timewav);
              MoveRightCGF(&arrows,0x102);
              MoveDownCGF(&arrows,0x140);
              SDL_ClearQueuedAudio(wipedev);
              SDL_QueueAudio(wipedev,wipebuf,wipelen);
              SDL_PauseAudioDevice(wipedev,0);
            }
            else if (nummenu == 3)
            {
              hidefilm(screen,bkgr,window);
              TintLeft = 1;
              TintRight = 4;
              startoption(&Timewav,window,bkgr);
              screen = SDL_GetWindowSurface(window);
              TintLeft = 4;
              TintRight = 1;
              listfilm(screen,bkgr,window,1);
            }
            else if (nummenu == 4)
            {
              exit = 1;
            }
          }
          else
          {
            UnloadFiles();
            if (numgame == 0)
            {
              labystart(&score,gamemode,playermode,window);
            }
            else if (numgame == 3)
            {
              foreststart(&score,gamemode,playermode,window);
            }
            OpenFiles();
            MoveRightCGF(&arrows,0x102);
            MoveDownCGF(&arrows,0x140);
            MoveRightCGF(&film,0x1A2);
            MoveDownCGF(&film,0x1F6);
            MoveRightCGF(&modeicon,0x16C);
            MoveDownCGF(&modeicon,0xAC);
            MoveRightCGF(&playersicon,0x16C);
            MoveDownCGF(&playersicon,0xD9);
            SDL_QueueAudio(wavdev,wavbuf,wavlen);
            Timewav = SDL_GetTicks();
            SDL_PauseAudioDevice(wavdev,0);
            numgame = 0;
            listfilm(screen,bkgr,window,1);
          }
        }
      }
      else if (e.type == SDL_QUIT)
      {
        exit = 1;
      }
      while (SDL_PollEvent(&e));
    }
    SDL_BlitSurface(bkgr,NULL,screen,NULL);
    DrawCGF(arrows,0,0,screen,TintDown,0);
    DrawCGF(arrows,2,0,screen,TintLeft,0);
    DrawCGF(arrows,4,0,screen,TintRight,0);
    DrawCGF(arrows,6,0,screen,TintUp,0);
    for (i = 0; i<21; ++i)
    {
      if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
      {
        DrawCGF(film,0,0,screen,1,0);
        if (gameraw[i].fp)
        {
          if ((i == numgame)&&(!selectmenu))
            DrawRAW(gameraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,1);
          else
            DrawRAW(gameraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
          printtext(ultrafont,ultramap,gamesname[i],0x1BA,film.data[0].posy+4,strlen(gamesname[i]),screen,1,0);
        }
      }
      MoveDownCGF(&film,0x80);
    }
    MoveUpCGF(&film,0x80*21);
    for (i = 0; i<5; ++i)
    {
      if ((selectmenu)&&(i == nummenu))
        printtext(menufont,menumap,menuname[i],0x14,0xA8+i*0x2A,strlen(menuname[i]),screen,3,1);
      else
        printtext(menufont,menumap,menuname[i],0x14,0xA8+i*0x2A,strlen(menuname[i]),screen,1,1);
    }
    DrawCGF(modeicon,gamemode,0,screen,1,0);
    DrawCGF(playersicon,playermode,0,screen,1,0);
    SDL_UpdateWindowSurface(window);
    if (SDL_GetTicks()-Timewav>=(wavlen*1000)/wavspec.freq)
    {
      SDL_QueueAudio(wavdev,wavbuf,wavlen);
      Timewav = SDL_GetTicks();
    }
  }
  SDL_FreeSurface(bkgr);
  UnloadFiles();
  return 0;
}
