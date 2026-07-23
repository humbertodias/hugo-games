#include "draw.h"
#include <string.h>

SDL_Surface *forest_screen;

static void forest_set_pixel_scaled(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  if (!forest_screen || !forest_screen->pixels)
    return;
  if (x < 0 || y < 0 || x >= FOREST_SCREEN_WIDTH || y >= FOREST_SCREEN_HEIGHT)
    return;
  if (x * 2 + 1 >= 640 || y * 2 + 1 >= 480)
    return;
  set_pixel(forest_screen, (Uint32)(x * 2), (Uint32)(y * 2), r, g, b, a);
  set_pixel(forest_screen, (Uint32)(x * 2 + 1), (Uint32)(y * 2), r, g, b, a);
  set_pixel(forest_screen, (Uint32)(x * 2), (Uint32)(y * 2 + 1), r, g, b, a);
  set_pixel(forest_screen, (Uint32)(x * 2 + 1), (Uint32)(y * 2 + 1), r, g, b, a);
}

int forest_anim_frame_count(ForestAnim *anim)
{
  return anim->end - anim->start + 1;
}

int forest_cgf_width(cgfinfo *info)
{
  if (!info->fp || info->head.num == 0)
    return 0;
  return (int)info->data[0].width;
}

int forest_cgf_height(cgfinfo *info)
{
  if (!info->fp || info->head.num == 0)
    return 0;
  return (int)info->data[0].height;
}

void forest_draw_rect_fill(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  int px, py, x0, y0, x1, y1;
  if (!forest_screen || w <= 0 || h <= 0)
    return;
  x0 = x < 0 ? 0 : x;
  y0 = y < 0 ? 0 : y;
  x1 = x + w;
  y1 = y + h;
  if (x1 > FOREST_SCREEN_WIDTH)
    x1 = FOREST_SCREEN_WIDTH;
  if (y1 > FOREST_SCREEN_HEIGHT)
    y1 = FOREST_SCREEN_HEIGHT;
  for (py = y0; py < y1; py++)
    for (px = x0; px < x1; px++)
      forest_set_pixel_scaled(px, py, r, g, b, a);
}

void forest_draw_rect_outline(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  forest_draw_rect_fill(x, y, w, 1, r, g, b, a);
  forest_draw_rect_fill(x, y + h - 1, w, 1, r, g, b, a);
  forest_draw_rect_fill(x, y, 1, h, r, g, b, a);
  forest_draw_rect_fill(x + w - 1, y, 1, h, r, g, b, a);
}

void forest_clear_screen(void)
{
  if (!forest_screen)
    return;
  SDL_FillRect(forest_screen, NULL, SDL_MapRGB(forest_screen->format, 0, 0, 0));
}

void forest_blit_surface(SDL_Surface *src, int x, int y)
{
  SDL_Rect dest;
  if (!src || !forest_screen)
    return;
  dest.x = x * 2;
  dest.y = y * 2;
  dest.w = src->w * 2;
  dest.h = src->h * 2;
  SDL_BlitScaled(src, NULL, forest_screen, &dest);
}

void forest_draw_cgf_at(cgfinfo *info, Uint32 frame, int x, int y, Uint32 tint)
{
  int dx, dy;
  if (!info->fp || !forest_screen || frame >= info->head.num)
    return;
  dx = x - (int)info->data[frame].posx;
  dy = y - (int)info->data[frame].posy;
  MoveRightCGF(info, dx);
  MoveDownCGF(info, dy);
  DrawCGF(*info, frame, 1, forest_screen, tint, 0);
  MoveLeftCGF(info, dx);
  MoveUpCGF(info, dy);
}

void forest_draw_cgf_digit_white(cgfinfo *info, Uint32 frame, int x, int y)
{
  Uint32 i, j, k, sz, nmb, tmpx, tmpy, frame_w, frame_h;
  Uint8 *image;
  Uint8 *buf;

  if (!info->fp || !forest_screen || frame >= info->head.num)
    return;

  frame_w = info->data[frame].width;
  frame_h = info->data[frame].height;
  if (frame_w == 0 || frame_h == 0)
    return;

  i = 0;
  j = 0;
  sz = info->data[frame + 1].offset - info->data[frame].offset;
  buf = (Uint8 *)malloc(sizeof(Uint8) * sz);
  image = (Uint8 *)malloc(sizeof(Uint8) * frame_w * frame_h * 2);
  if (!buf || !image)
  {
    free(buf);
    free(image);
    return;
  }
  fseek(info->fp, 0x1C + info->head.sizedata + info->data[frame].offset, SEEK_SET);
  fread(buf, sizeof(Uint8), sz, info->fp);
  while (i < frame_w * frame_h)
  {
    sz = buf[j] + buf[j + 1] * 256 + buf[j + 2] * 256 * 256 + buf[j + 3] * 256 * 256 * 256;
    k = 4;
    while (k < sz)
    {
      if (buf[j + k] == 0)
      {
        k++;
        nmb = buf[j + k];
        k++;
        if ((nmb == 0) && (sz == 6))
          nmb = 1;
        while (nmb != 0)
        {
          image[i * 2] = 0;
          image[i * 2 + 1] = 0;
          ++i;
          --nmb;
        }
      }
      else if (buf[j + k] == 1)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = buf[j + k + 1];
          ++i;
          k = k + 2;
          --nmb;
        }
      }
      else if (buf[j + k] == 2)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = buf[j + k + 1];
          ++i;
          --nmb;
        }
        k = k + 2;
      }
      else if (buf[j + k] == 3)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = 255;
          ++i;
          ++k;
          --nmb;
        }
      }
      else if (buf[j + k] == 4)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = 255;
          ++i;
          --nmb;
        }
        ++k;
      }
    }
    j = j + k;
    while (i % frame_w != 0)
    {
      image[i * 2] = 0;
      image[i * 2 + 1] = 0;
      ++i;
    }
  }

  for (i = 0; i < frame_h; ++i)
    for (j = 0; j < frame_w; ++j)
    {
      Uint32 idx = i * frame_w + j;
      Uint8 alpha = image[idx * 2 + 1];
      Uint8 pi = image[idx * 2];
      Uint8 sr, sg, sb, lum;
      if (alpha == 0)
        continue;
      sr = info->pal[pi * 4 + 2];
      sg = info->pal[pi * 4 + 1];
      sb = info->pal[pi * 4];
      lum = (Uint8)((sr + sg + sb) / 3);
      if (lum == 0)
        continue;
      /* SCORE.cgf uses very dark grayscale indices; boost to visible white. */
      lum = 255;
      tmpx = x + (int)j;
      tmpy = y + (int)i;
      forest_set_pixel_scaled(tmpx, tmpy, 255, 255, 255, lum);
    }
  free(image);
  free(buf);
}

void forest_draw_cgf_sheet_digit(cgfinfo *info, int digit, int x, int y)
{
  Uint32 i, j, k, sz, nmb, tmpx, tmpy;
  Uint8 *image;
  Uint8 *buf;
  int cell_w = 32;
  int cell_h = 33;
  int sx = 1 + (digit % 5) * (cell_w + 1);
  int sy = 1 + (digit / 5) * (cell_h + 1);

  if (!info->fp || !forest_screen || info->head.num == 0)
    return;

  i = 0;
  j = 0;
  sz = info->data[1].offset - info->data[0].offset;
  buf = (Uint8 *)malloc(sizeof(Uint8) * sz);
  image = (Uint8 *)malloc(sizeof(Uint8) * info->data[0].width * info->data[0].height * 2);
  if (!buf || !image)
  {
    free(buf);
    free(image);
    return;
  }
  fseek(info->fp, 0x1C + info->head.sizedata + info->data[0].offset, SEEK_SET);
  fread(buf, sizeof(Uint8), sz, info->fp);
  while (i < info->data[0].width * info->data[0].height)
  {
    sz = buf[j] + buf[j + 1] * 256 + buf[j + 2] * 256 * 256 + buf[j + 3] * 256 * 256 * 256;
    k = 4;
    while (k < sz)
    {
      if (buf[j + k] == 0)
      {
        k++;
        nmb = buf[j + k];
        k++;
        if ((nmb == 0) && (sz == 6))
          nmb = 1;
        while (nmb != 0)
        {
          image[i * 2] = 0;
          image[i * 2 + 1] = 0;
          ++i;
          --nmb;
        }
      }
      else if (buf[j + k] == 1)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = buf[j + k + 1];
          ++i;
          k = k + 2;
          --nmb;
        }
      }
      else if (buf[j + k] == 2)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = buf[j + k + 1];
          ++i;
          --nmb;
        }
        k = k + 2;
      }
      else if (buf[j + k] == 3)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = 255;
          ++i;
          ++k;
          --nmb;
        }
      }
      else if (buf[j + k] == 4)
      {
        k++;
        nmb = buf[j + k];
        k++;
        while (nmb != 0)
        {
          image[i * 2] = buf[j + k];
          image[i * 2 + 1] = 255;
          ++i;
          --nmb;
        }
        ++k;
      }
    }
    j = j + k;
    while (i % info->data[0].width != 0)
    {
      image[i * 2] = 0;
      image[i * 2 + 1] = 0;
      ++i;
    }
  }

  for (i = 0; i < (Uint32)cell_h; ++i)
    for (j = 0; j < (Uint32)cell_w; ++j)
    {
      Uint32 src_x = sx + j;
      Uint32 src_y = sy + i;
      Uint32 idx = src_y * info->data[0].width + src_x;
      if (src_x >= info->data[0].width || src_y >= info->data[0].height)
        continue;
      tmpx = x + (int)j;
      tmpy = y + (int)i;
      forest_set_pixel_scaled(tmpx, tmpy,
        info->pal[image[idx * 2] * 4 + 2],
        info->pal[image[idx * 2] * 4 + 1],
        info->pal[image[idx * 2] * 4],
        image[idx * 2 + 1]);
    }
  free(image);
  free(buf);
}

void forest_draw_anim_frame(ForestAnim *anim, int local_frame, int x, int y)
{
  int abs_frame;
  if (local_frame < 0 || local_frame >= forest_anim_frame_count(anim))
    return;
  abs_frame = anim->start + local_frame;
  if (anim->is_til)
  {
    (void)x;
    (void)y;
    DrawFrameTIL(anim->til, abs_frame, forest_screen);
  }
  else
  {
    forest_draw_cgf_at(&anim->cgf, abs_frame, x, y, 1);
  }
}

void forest_draw_anim_full(ForestAnim *anim, int local_frame)
{
  forest_draw_anim_frame(anim, local_frame, 0, 0);
}

void forest_draw_sync_cgf(ForestAnim *anim, oosinfo *sync, int tick, int x, int y)
{
  int local;
  if (!sync->buf || tick < 0 || (Uint32)tick >= sync->sz)
    return;
  local = sync->buf[tick] - 1;
  if (local < 0)
    local = 0;
  if (local >= forest_anim_frame_count(anim))
    local = forest_anim_frame_count(anim) - 1;
  forest_draw_cgf_at(&anim->cgf, anim->start + local, x, y, 1);
}

void forest_draw_sync_til(ForestAnim *anim, oosinfo *sync, int tick)
{
  int local;
  if (!sync->buf || tick < 0 || (Uint32)tick >= sync->sz)
    return;
  local = sync->buf[tick] - 1;
  if (local < 0)
    local = 0;
  if (local >= forest_anim_frame_count(anim))
    local = forest_anim_frame_count(anim) - 1;
  DrawFrameTIL(anim->til, anim->start + local, forest_screen);
}
