Uint32 numopt = 0;

void listfilmopt (SDL_Surface *screen, SDL_Surface *bkgr, SDL_Window *window, Uint32 pos)
{
  Uint32 Timer, i,Timefilm;
  SDL_ClearQueuedAudio(filmmdev);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_QueueAudio(filmmdev,filmmbuf,filmmlen);
  SDL_PauseAudioDevice(filmmdev,0);
  Timefilm = SDL_GetTicks();
  while (film.data[0].posy!=0xD6-numopt*0x80)
  {
    Timer = SDL_GetTicks();
    if (pos == 1)
      MoveUpCGF(&film,0x10);
    else
      MoveDownCGF(&film,0x10);
    SDL_BlitSurface(bkgr,NULL,screen,NULL);
    DrawCGF(arrows,0,0,screen,TintDown,0);
    if (numopt !=1) DrawCGF(arrows,2,0,screen,TintLeft,0);
    if (numopt !=1) DrawCGF(arrows,4,0,screen,TintRight,0);
    DrawCGF(arrows,6,0,screen,TintUp,0);
    for (i = 0; i<3; ++i)
    {
      if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
      {
        if (optionraw[i].fp)
        {
          DrawRAW(optionraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
          printtext(ultrafont,ultramap,optionsname[i],0x1BA,film.data[0].posy+4,strlen(optionsname[i]),screen,1,0);
        }
        DrawCGF(film,0,0,screen,1,0);
      }
      MoveDownCGF(&film,0x80);
    }
    MoveUpCGF(&film,0x80*3);

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
void hidefilmopt (SDL_Surface *screen, SDL_Surface *bkgr, SDL_Window *window)
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
    if (numopt !=1) DrawCGF(arrows,2,0,screen,TintLeft,0);
    if (numopt !=1) DrawCGF(arrows,4,0,screen,TintRight,0);
    DrawCGF(arrows,6,0,screen,TintUp,0);
    for (i = 0; i<3; ++i)
    {
      if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
      {
        if (optionraw[i].fp)
        {
          DrawRAW(optionraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
          printtext(ultrafont,ultramap,optionsname[i],0x1BA,film.data[0].posy+4,strlen(optionsname[i]),screen,1,0);
        }
        DrawCGF(film,0,0,screen,1,0);
      }
      MoveDownCGF(&film,0x80);
    }
    MoveUpCGF(&film,0x80*3);
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

void startoption(Uint32 *timer, SDL_Window *window, SDL_Surface *bkgr)
{
  Uint8 optexit = 0;
  Uint32 Timewav = *timer;
  Uint32 CreditTimer;
  Uint32 TestTime;
  Uint32 i;
  Uint32 selectopt = 0;
  char stbuf[256];
  SDL_Surface *screen = SDL_GetWindowSurface(window);
  SDL_Event e;
  numopt = 0;
  listfilmopt(screen,bkgr,window,1);
  while (SDL_PollEvent(&e));
  while (!optexit)
  {
    SDL_BlitSurface(bkgr,NULL,screen,NULL);
    DrawCGF(arrows,0,0,screen,TintDown,0);
    if (numopt !=1) DrawCGF(arrows,2,0,screen,TintLeft,0);
    if (numopt !=1) DrawCGF(arrows,4,0,screen,TintRight,0);
    DrawCGF(arrows,6,0,screen,TintUp,0);
    for (i = 0; i<3; ++i)
    {
      if ((film.data[0].posy+0x80<0x480)||(film.data[0].posy<0x480))
      {
        if (optionraw[i].fp)
        {
          if ((!selectopt)&&(numopt == i))
            DrawRAW(optionraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,1);
          else
            DrawRAW(optionraw[i],0x1BA,film.data[0].posy+4,0,0,0,screen,4);
          printtext(ultrafont,ultramap,optionsname[i],0x1BA,film.data[0].posy+4,strlen(optionsname[i]),screen,1,0);
        }
        DrawCGF(film,0,0,screen,1,0);
      }
      MoveDownCGF(&film,0x80);
    }
    MoveUpCGF(&film,0x80*3);
    if (numopt == 0)
    {
      stbuf[0] = 0;
      strcat(stbuf,optdisplay[0]);
      strcat(stbuf,": ");
      strcat(stbuf,optdisplay[options.display+1]);
      if (!selectopt)
        printtext(menufont,menumap,stbuf,(0x1A2-widthtext(menufont,menumap,stbuf,strlen(stbuf)))/2,200,strlen(stbuf),screen,1,1);
      else
        printtext(menufont,menumap,stbuf,(0x1A2-widthtext(menufont,menumap,stbuf,strlen(stbuf)))/2,200,strlen(stbuf),screen,3,1);
    }
    else if (numopt == 1)
    {
      TestTime = SDL_GetTicks()-CreditTimer;
      if (((TestTime/2000)%9 == 0)||((TestTime/2000)%9 == 1)||((TestTime/2000)%9 == 2))
      {
        printtext(menufont,menumap,itework[0],(0x1A2-widthtext(menufont,menumap,itework[0],strlen(itework[0])))/2,200,strlen(itework[0]),screen,1,1);
        for (i = 0; i<3; ++i)
        {
          printtext(creditfont,creditmap,programmers[((TestTime/2000)%9)*3+i],(0x1A2-widthtext(creditfont,creditmap,programmers[((TestTime/2000)%9)*3+i],strlen(programmers[((TestTime/2000)%9)*3+i])))/2,250+35*i,strlen(programmers[((TestTime/2000)%9)*3+i]),screen,1,1);
        }
      }
      else if (((TestTime/2000)%9 == 3)||((TestTime/2000)%9 == 4)||((TestTime/2000)%9 == 5))
      {
        printtext(menufont,menumap,itework[1],(0x1A2-widthtext(menufont,menumap,itework[1],strlen(itework[1])))/2,200,strlen(itework[1]),screen,1,1);
        if (((TestTime/2000)%9 == 3)||((TestTime/2000)%9 == 4))
        {
          for (i = 0; i<3; ++i)
          {
            printtext(creditfont,creditmap,graphics[((TestTime/2000)%9-3)*3+i],(0x1A2-widthtext(creditfont,creditmap,graphics[((TestTime/2000)%9-3)*3+i],strlen(graphics[((TestTime/2000)%9-3)*3+i])))/2,250+35*i,strlen(graphics[((TestTime/2000)%9-3)*3+i]),screen,1,1);
          }
        }
        else
        {
          for (i = 0; i<2; ++i)
          {
            printtext(creditfont,creditmap,graphics[((TestTime/2000)%9-3)*3+i],(0x1A2-widthtext(creditfont,creditmap,graphics[((TestTime/2000)%9-3)*3+i],strlen(graphics[((TestTime/2000)%9-3)*3+i])))/2,250+35*i,strlen(graphics[((TestTime/2000)%9-3)*3+i]),screen,1,1);
          }
        }
      }
      else if ((TestTime/2000)%9 == 6)
      {
        printtext(menufont,menumap,itework[2],(0x1A2-widthtext(menufont,menumap,itework[2],strlen(itework[2])))/2,200,strlen(itework[2]),screen,1,1);
        for (i = 0; i<2; ++i)
        {
          printtext(creditfont,creditmap,sounds[i],(0x1A2-widthtext(creditfont,creditmap,sounds[i],strlen(sounds[i])))/2,250+35*i,strlen(sounds[i]),screen,1,1);
        }
      }
      else if ((TestTime/2000)%9 == 7)
      {
        printtext(menufont,menumap,itework[3],(0x1A2-widthtext(menufont,menumap,itework[3],strlen(itework[3])))/2,200,strlen(itework[3]),screen,1,1);
        for (i = 0; i<1; ++i)
        {
          printtext(creditfont,creditmap,producers[i],(0x1A2-widthtext(creditfont,creditmap,producers[i],strlen(producers[i])))/2,250+35*i,strlen(producers[i]),screen,1,1);
        }
      }
      else if ((TestTime/2000)%9 == 8)
      {
        printtext(menufont,menumap,itework[4],(0x1A2-widthtext(menufont,menumap,itework[4],strlen(itework[4])))/2,200,strlen(itework[4]),screen,1,1);
        for (i = 0; i<1; ++i)
        {
          printtext(creditfont,creditmap,eproducers[i],(0x1A2-widthtext(creditfont,creditmap,eproducers[i],strlen(eproducers[i])))/2,250+35*i,strlen(eproducers[i]),screen,1,1);
        }
      }
    }
    else if (numopt == 2)
    {
      printtext(menufont,menumap,minigames[0],(0x1A2-widthtext(menufont,menumap,minigames[0],strlen(minigames[0])))/2,200,strlen(minigames[0]),screen,1,1);
      if (!selectopt)
        printtext(creditfont,creditmap,minigames[options.numminigame+1],(0x1A2-widthtext(creditfont,creditmap,minigames[options.numminigame+1],strlen(minigames[options.numminigame+1])))/2,240,strlen(minigames[options.numminigame+1]),screen,1,1);
      else
        printtext(creditfont,creditmap,minigames[options.numminigame+1],(0x1A2-widthtext(creditfont,creditmap,minigames[options.numminigame+1],strlen(minigames[options.numminigame+1])))/2,240,strlen(minigames[options.numminigame+1]),screen,3,1);
    }
    SDL_UpdateWindowSurface(window);
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          optexit = 1;
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN)||(e.key.keysym.scancode == SDL_SCANCODE_KP_2))
        {
          if (!selectopt)
          {
            TintUp = 4;
            TintLeft = 4;
            if (numopt == 2)
            {
              numopt = 0;
              listfilmopt(screen,bkgr,window,2);
            }
            else
            {
              numopt++;
              listfilmopt(screen,bkgr,window,1);
            }
            TintUp = 1;
            TintLeft = 1;
            if (numopt == 1)
              CreditTimer = SDL_GetTicks();
          }
          else
          {
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_UP)||(e.key.keysym.scancode == SDL_SCANCODE_KP_8))
        {
          if (!selectopt)
          {
            TintUp = 4;
            TintLeft = 4;
            if (numopt == 0)
            {
              numopt = 2;
              listfilmopt(screen,bkgr,window,1);
            }
            else
            {
              numopt--;
              listfilmopt(screen,bkgr,window,2);
            }
            TintUp = 1;
            TintLeft = 1;
            if (numopt == 1)
              CreditTimer = SDL_GetTicks();
          }
          else
          {
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT)||(e.key.keysym.scancode == SDL_SCANCODE_KP_4))
        {
          if (!selectopt)
          {
            TintLeft = 4;
            TintRight = 1;
            selectopt = 1;
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT)||(e.key.keysym.scancode == SDL_SCANCODE_KP_6))
        {
          if (selectopt)
          {
            TintLeft = 1;
            TintRight = 4;
            selectopt = 0;
          }
        }
        else if ((e.key.keysym.scancode == SDL_SCANCODE_RETURN)||(e.key.keysym.scancode == SDL_SCANCODE_KP_ENTER))
        {
          if (selectopt)
          {
            if (numopt == 0)
            {
              SDL_ClearQueuedAudio(toggdev);
              SDL_QueueAudio(toggdev,toggbuf,togglen);
              SDL_PauseAudioDevice(toggdev,0);
              if (options.display == 0)
              {
                options.display = 1;
                SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
              }
              else
              {
                options.display = 0;
                SDL_SetWindowFullscreen(window,0);
              }
            }
            else if (numopt == 2)
            {
              SDL_ClearQueuedAudio(toggdev);
              SDL_QueueAudio(toggdev,toggbuf,togglen);
              SDL_PauseAudioDevice(toggdev,0);
              if (options.numminigame == 6)
                options.numminigame = 0;
              else
                options.numminigame++;
            }
          }
        }
      }
    }
    if (SDL_GetTicks()-Timewav>=(wavlen*1000)/wavspec.freq)
    {
      SDL_QueueAudio(wavdev,wavbuf,wavlen);
      Timewav = SDL_GetTicks();
    }
    screen = SDL_GetWindowSurface(window);
  }
  *timer = Timewav;
  hidefilmopt(screen,bkgr,window);
  while (SDL_PollEvent(&e));
}