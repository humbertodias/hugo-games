#include <string.h>

typedef struct RAWH
{
  char name[6];
  Uint16 version;
  Uint16 width;
  Uint16 height;
  Uint16 numpal;
  Uint16 unk[9];
} rawhead;

typedef struct RAWL
{
  rawhead head;
  FILE *fp;
  Uint8 *pal;
} rawinfo;

rawinfo LoadRAW(char *fname)
{
  rawinfo info;
  info.fp = fopen(fname,"rb");
  if (info.fp)
  {
    fread(&(info.head),sizeof(info.head),1,info.fp);
    if (!strncmp(info.head.name,"mhwanh",6))
    {
      info.head.version = (info.head.version%256)*256+(info.head.version/256);
      info.head.width = (info.head.width%256)*256+(info.head.width/256);
      info.head.height = (info.head.height%256)*256+(info.head.height/256);
      info.head.numpal = (info.head.numpal%256)*256+(info.head.numpal/256);
      info.pal = (Uint8 *)malloc(info.head.numpal*3*sizeof(Uint8));
      fread(info.pal,sizeof(Uint8),info.head.numpal*3,info.fp);
    }
    else
    {
      fclose(info.fp);
      info.fp = NULL;
    }
  }
  return info;
}

void DrawRAW(rawinfo info, Uint32 posx, Uint32 posy, Uint32 startx, Uint32 starty, Uint8 scale, SDL_Surface *screen, Uint32 Tint)
{
  Uint32 i,j;
  Uint8 *buf;
  buf = (Uint8 *)malloc(info.head.height*info.head.width*sizeof(Uint8));
  fseek(info.fp,0x20+info.head.numpal*3,SEEK_SET);
  fread(buf,sizeof(Uint8),info.head.height*info.head.width,info.fp);
  for (i = starty; i<info.head.height; ++i)
    for (j = startx; j<info.head.width; ++j)
      if (scale)
      {
        if ((posy*2+i*2<480)&&(posx*2+j*2<640))
        {
          set_pixel(screen,posx+j*2,posy+i*2,info.pal[buf[i*info.head.width+j]*3]/Tint,info.pal[buf[i*info.head.width+j]*3+1]/Tint,info.pal[buf[i*info.head.width+j]*3+2]/Tint,255);
          set_pixel(screen,posx+j*2+1,posy+i*2,info.pal[buf[i*info.head.width+j]*3]/Tint,info.pal[buf[i*info.head.width+j]*3+1]/Tint,info.pal[buf[i*info.head.width+j]*3+2]/Tint,255);
          set_pixel(screen,posx+j*2,posy+i*2+1,info.pal[buf[i*info.head.width+j]*3]/Tint,info.pal[buf[i*info.head.width+j]*3+1]/Tint,info.pal[buf[i*info.head.width+j]*3+2]/Tint,255);
          set_pixel(screen,posx+j*2+1,posy+i*2+1,info.pal[buf[i*info.head.width+j]*3]/Tint,info.pal[buf[i*info.head.width+j]*3+1]/Tint,info.pal[buf[i*info.head.width+j]*3+2]/Tint,255);
        }
      }
      else
      {
        if ((posy+i<480)&&(posx+j<640))
          set_pixel(screen,posx+j,posy+i,info.pal[buf[i*info.head.width+j]*3]/Tint,info.pal[buf[i*info.head.width+j]*3+1]/Tint,info.pal[buf[i*info.head.width+j]*3+2]/Tint,255);
      }
  free(buf);
}

void FreeRAW(rawinfo info)
{
  free(info.pal);
  fclose(info.fp);
}