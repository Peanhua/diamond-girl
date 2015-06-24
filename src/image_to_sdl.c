/*
  Diamond Girl - Game where player collects diamonds.
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
#include "gfx.h"
#include <assert.h>
#include <SDL/SDL_gfxPrimitives.h>


bool image_to_sdl(struct image * image)
{
  bool rv;
  SDL_Surface * tmp;

  assert(image->sdl_surface == NULL);
  rv = false;

  tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, image->width, image->height, 32, 0, 0, 0, 0);
  assert(tmp != NULL);
  if(tmp != NULL)
    {
      image->sdl_surface = SDL_ConvertSurface(tmp, gfx_game_screen()->format, 0);
      SDL_FreeSurface(tmp);
      if(image->sdl_surface != NULL)
        {
          rv = true;
          for(int y = 0; y < image->height; y++)
            for(int x = 0; x < image->width; x++)
              if(image->flags & IF_ALPHA)
                pixelRGBA(image->sdl_surface,
                          x, image->height - y - 1,
                          image->data[4 * (x + y * image->width) + 0],
                          image->data[4 * (x + y * image->width) + 1],
                          image->data[4 * (x + y * image->width) + 2],
                          image->data[4 * (x + y * image->width) + 3]);
              else
                pixelRGBA(image->sdl_surface,
                          x, image->height - y - 1,
                          image->data[3 * (x + y * image->width) + 0],
                          image->data[3 * (x + y * image->width) + 1],
                          image->data[3 * (x + y * image->width) + 2],
                          0xff);
        }
    }


  return rv;
}
