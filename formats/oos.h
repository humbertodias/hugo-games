typedef struct OOSH
{
  char name[8];
  Uint32 ver;
  Uint32 unk1;
  Uint32 unk2;
  Uint32 ofst;
  Uint32 start;
} ooshead;

typedef struct OOSL
{
  Uint8 *buf;
  Uint32 sz;
  ooshead head;
} oosinfo;

oosinfo LoadOOS(char *fname)
{
 oosinfo info;
 FILE *fp = fopen(fname,"rb");
 info.buf = NULL;
 if (fp)
 {
   fread(&(info.head),sizeof(ooshead),1,fp);
   if (!strncmp(info.head.name,"SYNC2000",8))
   {
     fseek(fp,info.head.ofst,SEEK_SET);
     fread(&(info.sz),sizeof(Uint32),1,fp);
     fseek(fp,info.head.start,SEEK_SET);
     info.buf = (Uint8 *)malloc(sizeof(Uint8)*info.sz);
     fread(info.buf,sizeof(Uint8),info.sz,fp);
   }
   fclose(fp);
 }
 return info;
}

void FreeOOS(oosinfo info)
{
  free(info.buf);
}