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
#include "globals.h"

#include <SDL/SDL_image.h>

struct image * image_load(const char * filename, bool generate_alpha)
{
  SDL_Surface * bmp;
  struct image * rv;
  char fn[256];

  rv = NULL;

  snprintf(fn, sizeof fn, "%s", filename);
  bmp = IMG_Load(fn);

  if(bmp != NULL)
    {
      rv = image_load_from_surface(bmp, generate_alpha);
      if(rv != NULL)
        {
          rv->sdl_surface = bmp;
          if(!globals.opengl)
            if(generate_alpha == true)
              SDL_SetAlpha(rv->sdl_surface, 0, SDL_ALPHA_OPAQUE);
        }
      else
        SDL_FreeSurface(bmp);
    }
  else
    {
      printf("Failed to IMG_Load(%s): %s\n", fn, SDL_GetError());
    }
  
  return rv;
}
