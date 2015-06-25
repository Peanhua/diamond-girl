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

void image_to_greyscale(struct image * image)
{
  assert(image != NULL);

  for(int y = 0; y < image->height; y++)
    for(int x = 0; x < image->width; x++)
      {
        float grey;
        uint8_t r, g, b, a;
        
        r = image->data[(x + y * image->width) * 4 + 0];
        g = image->data[(x + y * image->width) * 4 + 1];
        b = image->data[(x + y * image->width) * 4 + 2];
        a = image->data[(x + y * image->width) * 4 + 3];

        grey = (float) r * 0.222f + (float) g * 0.707f + (float) b * 0.071f;

        image->data[(x + y * image->width) * 4 + 0] = grey;
        image->data[(x + y * image->width) * 4 + 1] = grey;
        image->data[(x + y * image->width) * 4 + 2] = grey;
        image->data[(x + y * image->width) * 4 + 3] = a;
      }

  if(image->sdl_surface != NULL)
    {
      SDL_FreeSurface(image->sdl_surface);
      image->sdl_surface = NULL;

      image_to_sdl(image);
    }
}
