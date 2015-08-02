/*
  Lucy the Diamond Girl - Game where player collects diamonds.
  Copyright (C) 2005-2015  Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


  Complete license can be found in the LICENSE file.
*/

#include "image.h"
#include <assert.h>

void image_blit(struct image const * src, struct image * dst, int dst_x, int dst_y)
{
  image_blit_partial(src, 0, 0, src->width, src->height, dst, dst_x, dst_y);
}

void image_blit_partial(struct image const * src, int src_x, int src_y, int width, int height, struct image * dst, int dst_x, int dst_y)
{
  int srcbpp, dstbpp;

  assert(src != NULL);
  assert(dst != NULL);
  assert(src_x >= 0);
  assert(src_y >= 0);
  assert(dst_x >= 0);
  assert(dst_y >= 0);
  assert(src->width  >= src_x + width );
  assert(src->height >= src_y + height);
  assert(dst->width  >= dst_x + width );
  assert(dst->height >= dst_y + height);

  if(src->flags & IF_ALPHA)
    srcbpp = 4;
  else
    srcbpp = 3;

  if(dst->flags & IF_ALPHA)
    dstbpp = 4;
  else
    dstbpp = 3;

  assert(srcbpp == 4);
  assert(srcbpp == dstbpp);

  for(int y = 0; y < height; y++)
    memcpy(dst->data + (dst_x + (y + dst_y) * dst->width) * dstbpp,
           src->data + (src_x + (y + src_y) * src->width) * srcbpp,
           width * srcbpp);

}

void image_blit_rotated(struct image const * src, int degrees, struct image * dst, int dst_x, int dst_y)
{
  assert(src != NULL);
  assert(dst != NULL);
  assert(degrees == 90);

  if(degrees == 90)
    {
      int neww, newh;
      struct image * tmp;
      int bpp;

      if(src->flags & IF_ALPHA)
        bpp = 4;
      else
        bpp = 3;

      neww = src->height;
      newh = src->width;
      tmp = image_new(neww, newh, (src->flags & IF_ALPHA) ? true : false);
      assert(tmp != NULL);
      
      for(int y = 0; y < src->height; y++)
        for(int x = 0; x < src->width; x++)
          memcpy(tmp->data + (y + x * tmp->width) * bpp,
                 src->data + (x + y * src->width) * bpp,
                 bpp);

      image_blit(tmp, dst, dst_x, dst_y);
      tmp = image_free(tmp);
    }
}
