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

struct image * image_load_from_surface(SDL_Surface * source, bool generate_alpha)
{
  int bpp;
  struct image * rv;

  assert(source != NULL);
  assert(source->format->BytesPerPixel == 1 ||
         source->format->BytesPerPixel == 3 ||
         source->format->BytesPerPixel == 4);

  if(generate_alpha)
    bpp = 4;
  else
    bpp = 3;

  rv = image_new(source->w, source->h, generate_alpha);
  assert(rv != NULL);
  if(rv != NULL)
    {
      SDL_LockSurface(source);

      rv->sdl_surface = source;

      for(int y = 0; y < source->h; y++)
        {
          uint8_t * src_p, * dst_p;

          src_p = source->pixels + y * source->pitch;
          dst_p = rv->data + (rv->height - y - 1) * rv->width * bpp;

          for(int x = 0; x < source->w; x++)
            {
              uint8_t r, g, b, a;

              a = 0xff;
              if(source->format->BytesPerPixel == 4)
                SDL_GetRGBA(*((uint32_t *) src_p), source->format, &r, &g, &b, &a);
              else if(source->format->BytesPerPixel == 1)
                {
                  uint8_t colour_index;

                  colour_index = *src_p;
                  assert((int) colour_index < source->format->palette->ncolors);
                  r = source->format->palette->colors[colour_index].r;
                  g = source->format->palette->colors[colour_index].g;
                  b = source->format->palette->colors[colour_index].b;
                  a = 0xff;
                }
              else if(source->format->BytesPerPixel == 3)
                {
                  uint32_t value;

                  memset(&value, 0xff, sizeof value);
                  memcpy(&value, src_p, source->format->BytesPerPixel);
                  SDL_GetRGB(value, source->format, &r, &g, &b);
                  a = 0xff;
                }
              else
                assert(false);

              if(source->format->BytesPerPixel < 4)
                if(source->flags & SDL_SRCCOLORKEY)
                  {
                    Uint32 rgb;
                    
                    rgb = SDL_MapRGB(source->format, r, g, b);
                    if(rgb == source->format->colorkey)
                      a = 0x00;
                  }

              dst_p[0] = r;
              dst_p[1] = g;
              dst_p[2] = b;
              if(generate_alpha)
                dst_p[3] = a;

              src_p += source->format->BytesPerPixel;
              dst_p += bpp;
            }
        }

      SDL_UnlockSurface(source);
    }

  return rv;
}
