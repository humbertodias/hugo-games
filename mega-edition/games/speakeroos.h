void speakooscgf(Uint32 addnumoos, cgfinfo cgfspk, oosinfo oosspk, SDL_AudioSpec spksndspec, SDL_AudioDeviceID spksnddev, Uint32 spksndlen, Uint8 *spksndbuf, SDL_Window *window, SDL_Surface *spkbkgr)
{
  SDL_Surface *screen = SDL_GetWindowSurface(window);
  Uint32 timeshift = 120;//((spksndlen*1000)/spksndspec.freq)/oosspk.sz;
  Uint32 starttime = SDL_GetTicks();
  SDL_ClearQueuedAudio(spksnddev);
  SDL_QueueAudio(spksnddev,spksndbuf,spksndlen);
  SDL_PauseAudioDevice(spksnddev,0);
  while ((SDL_GetTicks()-starttime)/timeshift<oosspk.sz)
  {
    SDL_BlitSurface(spkbkgr,NULL,screen,NULL);
    DrawCGF(cgfspk,oosspk.buf[(SDL_GetTicks()-starttime)/timeshift]-1,1,screen,1,0);
    SDL_UpdateWindowSurface(window);
  }
}