typedef struct HISCHEAD
{
  char name[17];
  Uint32 score;
  Uint8 gamemode;
} hiscore;

char sti[] = "0000";

char *strto4int(Uint32 vall)
{
  Uint32 tmp = vall, i = 0, zn = 1000;
  for (i = 0; i<4; ++i)
  {
    sti[i] = '0';
  }
  i = 0;
  while (i<4)
  {
    sti[i] = sti[i]+(tmp/zn);
    tmp = tmp%zn;
    zn = zn/10;
    ++i;
  }
  i = 0;
  while (sti[i]=='0')
  {
    sti[i] = ' ';
    ++i;
  }
  return sti;
}

void starthiscore(SDL_Window *window, Uint32 *timr)
{
  SDL_Surface *screen = SDL_GetWindowSurface(window);
  Uint32 i,j;
  Uint8 hiscexit = 0;
  Uint8 hinumclear = 0;
  Uint32 hinumgame = 0;
  Uint32 Timewav = *timr;
  hiscore higames[21][6];
  char hitmp[256];
  SDL_Event e;
  char numhirecord[] = "1.\0";
  for (i = 0; i<21; ++i)
    for (j = 0; j<6; ++j)
    {
      higames[i][j].name[0] = 0;
      strcat(higames[i][j].name,"NONAME");
      higames[i][j].score = 100-10*j;
      higames[i][j].gamemode = 0;
    }
  MoveRightCGF(&hiscreens,0x1d9);
  MoveDownCGF(&hiscreens,0x113);
  MoveRightCGF(&arrows,0x1db);
  MoveDownCGF(&arrows,0xc3);
  while (SDL_PollEvent(&e));
  while (!hiscexit)
  {
    DrawRAW(hiscback,0,0,0,0,0,screen,1);
    DrawRAW(hisc,0x26,6,0,0,0,screen,1);
    DrawCGF(arrows,6,0,screen,1,0);
    MoveDownCGF(&arrows,0x7d);
    DrawCGF(arrows,0,0,screen,1,0);
    MoveUpCGF(&arrows,0x7d);
    DrawCGF(hiscreens,hinumgame,0,screen,1,0);
    for (i = 0; i<6; ++i)
    {
      printtext(creditfont,creditmap,numhirecord,0x14,0xc8+0x2d*i,2,screen,1,0);
      printtext(creditfont,creditmap,higames[hinumgame][i].name,0x32,0xc8+0x2d*i,strlen(higames[hinumgame][i].name),screen,1,0);
      printtext(creditfont,creditmap,strto4int(higames[hinumgame][i].score),0x1ae -widthtext(creditfont,creditmap,strto4int(higames[hinumgame][i].score),4),0xc8+0x2d*i,4,screen,1,0);
      MoveRightCGF(&modeicon,0x4C);
      MoveDownCGF(&modeicon,0x1C+i*0x2d);
      DrawCGF(modeicon,higames[hinumgame][i].gamemode,0,screen,1,0);
      MoveLeftCGF(&modeicon,0x4C);
      MoveUpCGF(&modeicon,0x1C+i*0x2d);
      numhirecord[0]++;
    }
    numhirecord[0] = numhirecord[0]-6;
    hitmp[0] = 0;
    if (!hinumclear)
    {
      strcat(hitmp,"F10 - ");
    }
    strcat(hitmp,hiclean[hinumclear]);
    printtext(ultrafont,ultramap,hitmp,7,3,strlen(hitmp),screen,1,0);
    printtext(menufont,menumap,gamesname[hinumgame],0x13b -widthtext(menufont,menumap,gamesname[hinumgame],strlen(gamesname[hinumgame]))/2,0x10,strlen(gamesname[hinumgame]),screen,1,0);
    SDL_UpdateWindowSurface(window);
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          hiscexit = 1;
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN)||(e.key.keysym.scancode == SDL_SCANCODE_KP_2))
        {
          SDL_ClearQueuedAudio(cursdev);
          SDL_QueueAudio(cursdev,cursbuf,curslen);
          SDL_PauseAudioDevice(cursdev,0);
          if (hinumgame == 20)
            hinumgame = 0;
          else
            hinumgame++;
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_UP)||(e.key.keysym.scancode == SDL_SCANCODE_KP_8))
        {
          SDL_ClearQueuedAudio(cursdev);
          SDL_QueueAudio(cursdev,cursbuf,curslen);
          SDL_PauseAudioDevice(cursdev,0);
          if (hinumgame == 0)
            hinumgame = 20;
          else
            hinumgame--;
        }
        else if (e.key.keysym.scancode == SDL_SCANCODE_F10)
        {
          if (hinumclear)
          {
            hinumclear = 0;
            for (i = 0; i<6; ++i)
            {
              higames[hinumgame][i].name[0] = 0;
              strcat(higames[hinumgame][i].name,"NONAME");
              higames[hinumgame][i].score = 100-10*i;
              higames[hinumgame][i].gamemode = 0;
            }
          }
          else
          {
            hinumclear = 1;
          }
        }
      }
    }
    if (SDL_GetTicks()-Timewav>=(wavlen*1000)/wavspec.freq)
    {
      SDL_QueueAudio(wavdev,wavbuf,wavlen);
      Timewav = SDL_GetTicks();
    }
  }
  MoveLeftCGF(&arrows,0x1db);
  MoveUpCGF(&arrows,0xc3);
  MoveLeftCGF(&hiscreens,0x1d9);
  MoveUpCGF(&hiscreens,0x113);
  *timr = Timewav;
}
