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

#include "gfx.h"
#include "gfxbuf.h"
#include "globals.h"
#include <assert.h>

#ifdef WITH_OPENGL
static struct gfxbuf * buffer = NULL;
#endif

void gfx_draw_rectangle(SDL_Rect * r, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      int pos;

      if(buffer == NULL)
        {
          buffer = gfxbuf_new(GFXBUF_DYNAMIC, GL_QUADS, GFXBUF_BLENDING);
          assert(buffer != NULL);
          gfxbuf_resize(buffer, 4);
        }

      pos = 0;
      buffer->vbuf[pos++] = 0;
      buffer->vbuf[pos++] = 0;
      buffer->vbuf[pos++] = r->w - 1;
      buffer->vbuf[pos++] = 0;
      buffer->vbuf[pos++] = r->w - 1;
      buffer->vbuf[pos++] = r->h - 1;
      buffer->vbuf[pos++] = 0;
      buffer->vbuf[pos++] = r->h - 1;
      assert(pos / 2 == 4);
      gfxbuf_update(buffer, 0, 4);

      gfx_colour(red, green, blue, alpha);

      gfxbuf_draw_at(buffer, r->x, r->y);
#endif
    }
  else
    {
#ifdef WITH_NONOPENGL
      Uint32 c;

      c = SDL_MapRGB(gfx_game_screen()->format, red, green, blue);
      SDL_FillRect(gfx_game_screen(), r, c);
      if(alpha)
        { } /* Just to silence compiler when OpenGL is disabled. */
#endif
    }
}
