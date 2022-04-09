/*
  Lucy the Diamond Girl - Game where player collects diamonds.
  Copyright (C) 2005-2022  Joni Yrjänä <joniyrjana@gmail.com>
  
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

struct image * image_trim(struct image const * image)
{
  assert(image != NULL);
  int x1, y1;
  int x2, y2;
  bool empty;

  x1 = 0;
  empty = true;
  for(int x = 0; empty == true && x < image->width; x++)
    {
      for(int y = 0; empty == true && y < image->height; y++)
        for(int i = 0; empty == true && i < 4; i++)
          if(image->data[(x + y * image->width) * 4 + i] != 0)
            empty = false;

      if(empty == true)
        x1 = x;
    }
  y1 = 0;
  empty = true;
  for(int y = 0; empty == true && y < image->height; y++)
    {
      for(int x = 0; empty == true && x < image->width; x++)
        for(int i = 0; empty == true && i < 4; i++)
          if(image->data[(x + y * image->width) * 4 + i] != 0)
            empty = false;

      if(empty == true)
        y1 = y;
    }
  x2 = image->width - 1;
  empty = true;
  for(int x = image->width - 1; empty == true && x >= 0; x--)
    {
      for(int y = 0; empty == true && y < image->height; y++)
        for(int i = 0; empty == true && i < 4; i++)
          if(image->data[(x + y * image->width) * 4 + i] != 0)
            empty = false;

      if(empty == true)
        x2 = x;
    }
  y2 = 0;
  empty = true;
  for(int y = image->height - 1; empty == true && y >= 0; y--)
    {
      for(int x = 0; empty == true && x < image->width; x++)
        for(int i = 0; empty == true && i < 4; i++)
          if(image->data[(x + y * image->width) * 4 + i] != 0)
            empty = false;

      if(empty == true)
        y2 = y;
    }

  struct image * rv;
  
  rv = image_new(x2 - x1, y2 - y1, image->flags & IF_ALPHA ? true : false);
  if(rv != NULL)
    image_blit_partial(image, x1, y1, rv->width, rv->height, rv, 0, 0);

  return rv;
}
