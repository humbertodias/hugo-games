#include "../menu/menu.h"

int StartGame(SDL_Window *window)
{
  SDL_Surface *screen = SDL_GetWindowSurface(window);
  SDL_Surface *bgr = SDL_CreateRGBSurface(0,640,480,32,0,0,0,0);
  SDL_Event e;
  Uint8 st;
  lzpinfo itemedia;
  rawinfo intro2;
  Uint32 i;
  SDL_AudioSpec wavspec;
  SDL_AudioDeviceID wavdev;
  Uint32 wavlen;
  Uint8 *wavbuf;
  itemedia = LoadLZP(datapath("IntroData/ite-media.lzp"));
  if (itemedia.fp==NULL)
  {
    printf("(Intro Error)File not found: %s\n", datapath("IntroData/ite-media.lzp"));
    return 1;
  }
  if (SDL_LoadWAV(datapath("IntroData/ite-media.wav"),&wavspec,&wavbuf,&wavlen) == NULL)
  {
    printf("(Intro Error)File not found: %s\n", datapath("IntroData/ite-media.wav"));
    UnloadLZP(itemedia);
    return 1;
  }
  wavdev = SDL_OpenAudioDevice(NULL,0,&wavspec,NULL,0);
  SDL_QueueAudio(wavdev,wavbuf,wavlen);
  SDL_PauseAudioDevice(wavdev,0);
  StartTimerLZP(&itemedia);
  i = GetFrameNumberLZP(itemedia);
  st = 0;
  while ((i<itemedia.head.num)&&(!st) )
  {
    LoadFrameLZP(itemedia,i,screen);
    SDL_UpdateWindowSurface(window);
    i = GetFrameNumberLZP(itemedia);
    if (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          st = 1;
        }
      }
    }
  }
  UnloadLZP(itemedia);
  if (!st)
    SDL_Delay(4000);
  SDL_CloseAudioDevice(wavdev);
  SDL_FreeWAV(wavbuf);
  if (bgr == NULL)
  {
     printf("(Intro Error)Not load background surface\n");
     return 1;
  }
  st = 0;
  intro2 = LoadRAW(datapath("IntroData/INTRO.RAW"));
  if (intro2.fp == NULL)
  {
    printf("(Intro Error)File not found: %s\n", datapath("IntroData/INTRO.RAW"));
    return 1;
  }
  for (i = 256; (i!=0) && (!st); i = i/2)
  {
    DrawRAW(intro2,0,0,0,0,1,screen,i);
    SDL_UpdateWindowSurface(window);
    if (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          st = 1;
        }
      }
    }
  }
  if (!st)
    SDL_Delay(1000);
  for (i = 1; (i<=256) && (!st); i = i*2)
  {
    DrawRAW(intro2,0,0,0,0,1,screen,i);
    SDL_UpdateWindowSurface(window);
    if (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          st = 1;
        }
      }
    }
  }
  FreeRAW(intro2);
  SDL_FreeSurface(bgr);
  return StartMenu(window);
}