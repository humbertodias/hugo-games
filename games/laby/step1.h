Uint8 labystartstep1 (Uint32 *score, Uint8 labygamemode, Uint8 labynumplayers, SDL_Window *window)
{
  SDL_Surface *screen;
  Uint32 Timer, TimeWalk;
  Uint32 lbfr = 0, nmhgwlk = 0,hlr = 0;
  Uint32 labyexit = 0, i;
  SDL_Event e;
  cgfinfo hugowalk = labyhugowalksright;
  Timer = SDL_GetTicks();
  SDL_ClearQueuedAudio(labyrun2dev);
  SDL_QueueAudio(labyrun2dev,labyrun2buf,labyrun2len);
  SDL_PauseAudioDevice(labyrun2dev,0);
  TimeWalk = SDL_GetTicks();
  while (SDL_PollEvent(&e));
  while (!labyexit)
  {
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          labyexit = 1;
        }
        else if (((e.key.keysym.scancode == SDL_SCANCODE_KP_4)||(e.key.keysym.scancode == SDL_SCANCODE_LEFT)) && (hlr == 0))
        {
          hugowalk = labyhugowalksleft;
          hlr = 1;
          Timer = SDL_GetTicks();
          SDL_ClearQueuedAudio(labyrun2dev);
          for (i = 0; i<labyhugojumpsleft.head.num; ++i)
          {
            DrawFrameTIL(labybaggr1,lbfr,screen);
            DrawCGF(labyhugojumpsleft,i,1,screen,1,0);
            SDL_UpdateWindowSurface(window);
            lbfr = (lbfr+1)%labybaggr1.head.numfr;
            while (SDL_GetTicks()-Timer<60);
            Timer = SDL_GetTicks();
          }
          while (SDL_PollEvent(&e));
          SDL_ClearQueuedAudio(labyrun2dev);
          SDL_QueueAudio(labyrun2dev,labyrun2buf,labyrun2len);
        }
        else if (((e.key.keysym.scancode == SDL_SCANCODE_KP_6)||(e.key.keysym.scancode == SDL_SCANCODE_RIGHT)) && (hlr == 1))
        {
          hugowalk = labyhugowalksright;
          hlr = 0;
          Timer = SDL_GetTicks();
          SDL_ClearQueuedAudio(labyrun2dev);
          for (i = 0; i<labyhugojumpsright.head.num; ++i)
          {
            DrawFrameTIL(labybaggr1,lbfr,screen);
            DrawCGF(labyhugojumpsright,i,1,screen,1,0);
            SDL_UpdateWindowSurface(window);
            lbfr = (lbfr+1)%labybaggr1.head.numfr;
            while (SDL_GetTicks()-Timer<60);
            Timer = SDL_GetTicks();
          }
          while (SDL_PollEvent(&e));
          SDL_ClearQueuedAudio(labyrun2dev);
          SDL_QueueAudio(labyrun2dev,labyrun2buf,labyrun2len);
        }
      }
    }
    screen = SDL_GetWindowSurface(window);
    DrawFrameTIL(labybaggr1,lbfr,screen);
    DrawCGF(hugowalk,nmhgwlk,1,screen,1,0);
    SDL_UpdateWindowSurface(window);
    lbfr = (lbfr+1)%labybaggr1.head.numfr;
    nmhgwlk = (nmhgwlk+1)%hugowalk.head.num;
    if (SDL_GetTicks()-TimeWalk>(labyrun2len*1000)/labyrun2spec.freq)
    {
      SDL_ClearQueuedAudio(labyrun2dev);
      SDL_QueueAudio(labyrun2dev,labyrun2buf,labyrun2len);
      TimeWalk = SDL_GetTicks();
    }
    while (SDL_GetTicks()-Timer<60);
    Timer = SDL_GetTicks();
    if (SDL_GetTicks()-TimeWalk>(labyrun2len*1000)/labyrun2spec.freq)
    {
      SDL_ClearQueuedAudio(labyrun2dev);
      SDL_QueueAudio(labyrun2dev,labyrun2buf,labyrun2len);
      TimeWalk = SDL_GetTicks();
    }
  }
  return 0;
}