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

#include "gfx.h"
#include "gfxbuf.h"
#include "gfx_material.h"
#include "globals.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <assert.h>

#ifdef WITH_OPENGL
static struct gfxbuf * buffer = NULL;
#endif

void gfx_draw_hline(int x, int y, int width, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      if(buffer == NULL)
        {
          buffer = gfxbuf_new(GFXBUF_DYNAMIC_2D, GL_LINES, 0);
          if(buffer != NULL)
            {
              gfxbuf_resize(buffer, 2);
              buffer->material = gfx_material_new();
            }
        }

      if(buffer != NULL && buffer->material != NULL)
        {
          int pos;

          pos = 0;
          buffer->vbuf[pos++] = 0;
          buffer->vbuf[pos++] = 0;
          buffer->vbuf[pos++] = width;
          buffer->vbuf[pos++] = 0;

          gfx_material_change4f(buffer->material, GFX_MATERIAL_COLOUR,
                                (float) red / 255.0,
                                (float) green / 255.0,
                                (float) blue / 255.0,
                                (float) alpha / 255.0);

          gfxbuf_update(buffer, 0, 2);
          gfxbuf_draw_at(buffer, x, y);
        }
#endif
    }
#ifdef WITH_NONOPENGL
  else
    hlineRGBA(gfx_game_screen(), x, x + width, y, red, green, blue, alpha);
#endif
}
