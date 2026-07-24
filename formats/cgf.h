#include <string.h>
typedef struct CGFH
{
  char name[4];
  Uint32 bt;
  Uint32 num;
  Uint32 sizedata;
  Uint32 size;
  Uint32 numpal;
  Uint32 unk;
} cgfhead;

typedef struct CGFD
{
  Uint32 posx;
  Uint32 posy;
  Uint32 width;
  Uint32 height;
  Uint32 unk;
  Uint32 offset;
} cgfdata;

typedef struct CGFL
{
  FILE *fp;
  cgfhead head;
  Uint32 Timer;
  Uint8 *pal;
  cgfdata *data;
} cgfinfo;

cgfinfo LoadCGF(char *fname)
{
  cgfinfo info;
  info.fp = fopen(fname,"rb");
  if (info.fp)
  {
    fread(&(info.head),sizeof(info.head),1,info.fp);
    if (!strncmp(info.head.name,"CGFF",4))
    {
      info.data = (cgfdata *)malloc(sizeof(cgfdata)*(info.head.num+1));
      fread(info.data,sizeof(cgfdata),info.head.num,info.fp);
      if (info.head.numpal != 0)
      {
        info.pal = (Uint8 *)malloc(sizeof(Uint8)*info.head.numpal*4);
        fseek(info.fp,0x1C+info.head.sizedata+info.head.size,SEEK_SET);
        fread(info.pal,sizeof(Uint8),info.head.numpal*4,info.fp);
      }
      info.data[info.head.num].offset = info.head.size;
    }
    else
    {
      fclose(info.fp);
      info.fp = NULL;
    }
  }
  return info;
}
void MoveLeftCGF(cgfinfo *info, Uint32 lshift)
{
  Uint32 i;
  for (i = 0; i<(*info).head.num; ++i)
    (*info).data[i].posx = (*info).data[i].posx-lshift;
}

void MoveRightCGF(cgfinfo *info, Uint32 rshift)
{
  Uint32 i;
  for (i = 0; i<(*info).head.num; ++i)
    (*info).data[i].posx = (*info).data[i].posx+rshift;
}

void MoveUpCGF(cgfinfo *info, Uint32 ushift)
{
  Uint32 i;
  for (i = 0; i<(*info).head.num; ++i)
    (*info).data[i].posy = (*info).data[i].posy-ushift;
}

void MoveDownCGF(cgfinfo *info, Uint32 dshift)
{
  Uint32 i;
  for (i = 0; i<(*info).head.num; ++i)
    (*info).data[i].posy = (*info).data[i].posy+dshift;
}

Uint8 testbyte(Uint8 byte, Uint32 antitint)
{
  Uint8 tmp = byte*antitint;
  if (tmp<byte)
  {
    return 255;
  }
  else
  {
    return tmp;
  }
}

void DrawCGF(cgfinfo info, Uint32 numpic, Uint8 scale, SDL_Surface *screen, Uint32 Tint, Uint8 anti)
{
  Uint32 i,j,k,sz,nmb,tmpx,tmpy;
  Uint8 *image;
  Uint8 *buf;
  i = 0;
  j = 0;
  sz = info.data[numpic+1].offset-info.data[numpic].offset;
  buf = (Uint8 *)malloc(sizeof(Uint8)*sz);
  image = (Uint8 *)malloc(sizeof(Uint8)*info.data[numpic].width*info.data[numpic].height*2);
  fseek(info.fp,0x1C+info.head.sizedata+info.data[numpic].offset,SEEK_SET);
  fread(buf,sizeof(Uint8),sz,info.fp);
  while (i<info.data[numpic].width*info.data[numpic].height)
  {
    sz = buf[j]+buf[j+1]*256+buf[j+2]*256*256+buf[j+3]*256*256*256;
    k = 4;
    while (k<sz)
    {
      if (buf[j+k]==0)
      {
        k++;
        nmb = buf[j+k];
        k++;
        if ((nmb == 0) && (sz == 6))
          nmb = 1;
        while (nmb!=0)
        {
          image[i*2] = 0;
          image[i*2+1] = 0;
          ++i;
          --nmb;
        }
      }
      else if (buf[j+k]==1)
      {
        k++;
        nmb = buf[j+k];
        k++;
        while (nmb!=0)
        {
          image[i*2] = buf[j+k];
          image[i*2+1] = buf[j+k+1];
          ++i;
          k = k+2;
          --nmb;
        }
      }
      else if (buf[j+k]==2)
      {
        k++;
        nmb = buf[j+k];
        k++;
        while (nmb!=0)
        {
          image[i*2] = buf[j+k];
          image[i*2+1] = buf[j+k+1];
          ++i;
          --nmb;
        }
        k = k+2;
      }
      else if (buf[j+k]==3)
      {
        k++;
        nmb = buf[j+k];
        k++;
        while (nmb!=0)
        {
          image[i*2] = buf[j+k];
          image[i*2+1] = 255;
          ++i;
          ++k;
          --nmb;
        }
      }
      else if (buf[j+k]==4)
      {
        k++;
        nmb = buf[j+k];
        k++;
        while (nmb!=0)
        {
          image[i*2] = buf[j+k];
          image[i*2+1] = 255;
          ++i;
          --nmb;
        }
        ++k;
      }
    }
    j = j+k;
    while (i%info.data[numpic].width!=0)
    {
      image[i*2] = 0;
      image[i*2+1] = 0;
      ++i;
    }
  }
  for (i = 0; i<info.data[numpic].height; ++i)
    for (j = 0; j<info.data[numpic].width; ++j)
    {
      tmpx = info.data[numpic].posx+j;
      tmpy = info.data[numpic].posy+i;
      if (scale)
      {
        if ((tmpx*2<640)&&(tmpy*2<480))
        {
          if (!anti)
          {
            set_pixel(screen,tmpx*2,tmpy*2,info.pal[image[(i*info.data[numpic].width+j)*2]*4+2]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4+1]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4]/Tint,image[(i*info.data[numpic].width+j)*2+1]);
            set_pixel(screen,tmpx*2+1,tmpy*2,info.pal[image[(i*info.data[numpic].width+j)*2]*4+2]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4+1]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4]/Tint,image[(i*info.data[numpic].width+j)*2+1]);
            set_pixel(screen,tmpx*2,tmpy*2+1,info.pal[image[(i*info.data[numpic].width+j)*2]*4+2]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4+1]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4]/Tint,image[(i*info.data[numpic].width+j)*2+1]);
            set_pixel(screen,tmpx*2+1,tmpy*2+1,info.pal[image[(i*info.data[numpic].width+j)*2]*4+2]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4+1]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4]/Tint,image[(i*info.data[numpic].width+j)*2+1]);
          }
          else
          {
            set_pixel(screen,tmpx*2,tmpy*2,testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+2],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+1],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4],Tint),image[(i*info.data[numpic].width+j)*2+1]);
            set_pixel(screen,tmpx*2+1,tmpy*2,testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+2],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+1],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4],Tint),image[(i*info.data[numpic].width+j)*2+1]);
            set_pixel(screen,tmpx*2,tmpy*2+1,testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+2],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+1],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4],Tint),image[(i*info.data[numpic].width+j)*2+1]);
            set_pixel(screen,tmpx*2+1,tmpy*2+1,testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+2],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+1],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4],Tint),image[(i*info.data[numpic].width+j)*2+1]);
          }
        }
      }
      else
      {
        if ((tmpx<640)&&(tmpy<480))
        {
          if (!anti)
          {
            set_pixel(screen,tmpx,tmpy,info.pal[image[(i*info.data[numpic].width+j)*2]*4+2]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4+1]/Tint,info.pal[image[(i*info.data[numpic].width+j)*2]*4]/Tint,image[(i*info.data[numpic].width+j)*2+1]);
          }
          else
          {
            set_pixel(screen,tmpx,tmpy,testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+2],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4+1],Tint),testbyte(info.pal[image[(i*info.data[numpic].width+j)*2]*4],Tint),image[(i*info.data[numpic].width+j)*2+1]);
          }
        }
      }
    }
  free(image);
  free(buf);
}

void FreeCGF(cgfinfo info)
{
  if (info.head.numpal != 0)
    free(info.pal);
  free(info.data);
  if (info.fp)
    fclose(info.fp);
}
