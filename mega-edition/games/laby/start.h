#include "files.h"
#include "../speakeroos.h"
#include "step1.h"

Uint8 labystart (Uint32 *score, Uint8 labygamemode, Uint8 labynumplayers, SDL_Window *window)
{
  SDL_Surface *labystbk;
  Uint32 opn = LabyOpenFiles();
  Uint8 sts = 0;
  labystbk = SDL_CreateRGBSurface(0,640,480,32,0,0,0,0);
  if (opn)
  {
    LabyUnloadFiles();
    return 2;
  }
  DrawRAW(labyintropic,0,0,0,0,1,labystbk,1);
  MoveLeftCGF(&labyhugosync,0x42);
  MoveUpCGF(&labyhugosync,0x10);
  speakooscgf(0, labyhugosync, laby01sync, laby01spec, laby01dev, laby01len, laby01buf, window, labystbk);
  MoveRightCGF(&labyhugosync,0x42);
  MoveDownCGF(&labyhugosync,0x10);
  SDL_FreeSurface(labystbk);
  sts = labystartstep1(score,labygamemode,labynumplayers,window);
  LabyUnloadFiles();
  return sts;
}