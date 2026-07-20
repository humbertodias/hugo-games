#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

typedef struct OPTHUGO
{
  Uint8 display;
  Uint8 numminigame;
} mainopt;
mainopt options;

#include "mysufcfunc.h"
#include "formats/lzp.h"
#include "formats/raw.h"
#include "formats/til.h"
#include "formats/cgf.h"
#include "formats/oos.h"
#include "loading/intro.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main (int argc, char **args)
{
  SDL_Window* window = NULL;
  Uint32 status;
  options.display = 0;
  options.numminigame = 0;
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) !=0 )
  {
    return 1;
  }
  window = SDL_CreateWindow("Hugo. Mega Edition",SDL_WINDOWPOS_UNDEFINED,
           SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
  if (window==NULL)
  {
    SDL_Quit();
    return 1;
  }
  status = StartGame(window);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return status;
}
