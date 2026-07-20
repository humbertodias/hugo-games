typedef struct TILH
{
  Uint32 size;
  Uint16 unk1;
  Uint16 numfr;
  Uint16 width;
  Uint16 height;
  Uint16 minfps;
  Uint16 fps;
  Uint8 unk2[4];
  Uint16 wi2;
  Uint16 he2;
  Uint8 unk3[8];
  Uint8 pal[0x300];
} tilhead;

typedef struct TILL
{
  FILE *fp;
  tilhead head;
  Uint32 Timer;
} tilinfo;

tilinfo LoadTIL(char *fname)
{
  tilinfo info;
  info.fp = fopen(fname,"rb");
  if (info.fp)
  {
    fread(&(info.head),sizeof(tilhead),1,info.fp);
    info.head.pal[0] = 0;
    info.head.pal[1] = 0;
    info.head.pal[2] = 0;
    info.head.wi2 = info.head.wi2/256+(info.head.he2%256)*256;
    info.head.he2 = info.head.he2/256+(info.head.unk3[0]%256)*256;
  }
  return info;
}

void StartTimerTIL(tilinfo *info)
{
  (*info).Timer = SDL_GetTicks();
}

Uint32 GetFrameNumberTIL(tilinfo info)
{
  Uint32 Now = (SDL_GetTicks()-info.Timer)*info.head.fps;
  Now = Now/1000;
  return Now;
}

void DrawFrameTIL(tilinfo info, Uint32 frnum, SDL_Surface *screen)
{
  Uint16 *buf;
  Uint8 *image;
  Uint32 i,j,k;
  Uint16 z;
  buf = (Uint16 *)malloc(info.head.wi2*info.head.he2*sizeof(Uint16));
  image = (Uint8 *)malloc(info.head.width*info.head.height*sizeof(Uint8));
  fseek(info.fp,0x320+frnum*info.head.wi2*info.head.he2*2,SEEK_SET);
  fread(buf,sizeof(Uint16),info.head.wi2*info.head.he2,info.fp);
  for (i = 0; i<info.head.he2; ++i)
    for (j = 0; j<info.head.wi2; ++j)
    {
      z = buf[i*info.head.wi2+j];
      for (k = 0; k<0x10; ++k)
      {
        fseek(info.fp,0x320+(info.head.numfr+2)*info.head.wi2*info.head.he2*2+k*16+z*256,SEEK_SET);
        fread(image+info.head.width*i*16+j*16+k*info.head.width,sizeof(Uint8),0x10,info.fp);
      }
    }
  for (i = 0; i<info.head.height; ++i)
    for (j = 0; j<info.head.width; ++j)
    {
      set_pixel(screen,j*2,i*2,info.head.pal[image[i*info.head.width+j]*3],info.head.pal[image[i*info.head.width+j]*3+1],info.head.pal[image[i*info.head.width+j]*3+2],255);
      set_pixel(screen,j*2,i*2+1,info.head.pal[image[i*info.head.width+j]*3],info.head.pal[image[i*info.head.width+j]*3+1],info.head.pal[image[i*info.head.width+j]*3+2],255);
      set_pixel(screen,j*2+1,i*2,info.head.pal[image[i*info.head.width+j]*3],info.head.pal[image[i*info.head.width+j]*3+1],info.head.pal[image[i*info.head.width+j]*3+2],255);
      set_pixel(screen,j*2+1,i*2+1,info.head.pal[image[i*info.head.width+j]*3],info.head.pal[image[i*info.head.width+j]*3+1],info.head.pal[image[i*info.head.width+j]*3+2],255);
    }
  free(buf);
  free(image);
}

void FreeTIL(tilinfo info)
{
  fclose(info.fp);
}