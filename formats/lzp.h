typedef struct LZPH{
  Uint32 num;
  Uint32 width;
  Uint32 height;
  Uint32 fps;
  Uint32 unk[4];
  Uint8 pal[768];
} lzphead;

typedef struct LZPI{
  lzphead head;
  FILE *fp;
  Uint32 *offset;
  Uint32 Timer;
} lzpinfo;

lzpinfo LoadLZP(char *fname)
{
  lzpinfo info;
  info.fp = fopen(fname,"rb");
  if (info.fp)
  {
    fread(&(info.head),sizeof(lzphead),1,info.fp);
    fseek(info.fp,0,SEEK_END);
    info.offset = (Uint32 *)malloc((info.head.num+1)*sizeof(Uint32));
    info.offset[info.head.num] = ftell(info.fp)-info.head.num*sizeof(Uint32);
    fseek(info.fp,info.offset[info.head.num],SEEK_SET);
    fread(info.offset,sizeof(Uint32),info.head.num,info.fp);
  }
  return info;
}

void StartTimerLZP(lzpinfo *info)
{
  (*info).Timer = SDL_GetTicks();
}

Uint32 GetFrameNumberLZP(lzpinfo info)
{
  Uint32 Now = (SDL_GetTicks()-info.Timer)*info.head.fps;
  Now = Now/1000;
  return Now;
}

void LoadFrameLZP(lzpinfo info, Uint32 frnum, SDL_Surface *screen)
{
  Uint8 *image;
  Uint32 sz;
  Uint8 *buf;
  Uint8 *zbuf;
  Uint32 bt, b1, b2, rz;
  Uint32 i = 0, j = 0, k = 0xFEE;
  fseek(info.fp,info.offset[frnum],SEEK_SET);
  fread(&sz,sizeof(Uint32),1,info.fp);
  image = (Uint8 *)malloc(info.head.width*info.head.height*sizeof(Uint8));
  zbuf = (Uint8 *)malloc(0x1000*sizeof(Uint8));
  buf = (Uint8 *)malloc((sz+1)*sizeof(Uint8));
  fread(buf,sizeof(Uint8),sz,info.fp);
  while (j<info.head.width*info.head.height)
  {
    bt = buf[i]+0xFF00;
    ++i;
    while ((bt>0xFF) && (j<info.head.width*info.head.height))
    {
      if ((bt&1) != 0)
      {
        bt = bt/2;
        image[j] = buf[i];
        zbuf[k] = buf[i];
        ++i;
        ++j;
        ++k;
        k = k & 0xFFF;
      }
      else
      {
        bt = bt/2;
        b1 = buf[i];
        b2 = buf[i+1];
        i = i+2;
        rz = b2/16;
        b1 = b1+0x100*rz;
        b2 = b2%16;
        b2 = b2+3;
        while (b2!=0)
        {
          image[j] = zbuf[b1];
          zbuf[k] = zbuf[b1];
          ++j;
          ++k;
          --b2;
          ++b1;
          k = k&0xFFF;
          b1 = b1&0xFFF;
        }
      }
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
  free(zbuf);
  free(image);
}

void UnloadLZP(lzpinfo info)
{
  free(info.offset);
  fclose(info.fp);
}
