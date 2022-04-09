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
#include "globals.h"
#include <assert.h>


#ifdef WITH_OPENGL
static struct gfxbuf * buffer     = NULL;
#endif

void gfx2d_draw_borders(SDL_Rect * r, uint8_t mode, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
  uint8_t colors[4 * 4];

  for(int i = 0; i < 4; i++)
    colors[i * 4 + 3] = alpha;

  switch(mode)
    {
    default:
    case 0:
      colors[0 * 4 + 0] = red;
      colors[0 * 4 + 1] = green;
      colors[0 * 4 + 2] = blue;
      colors[1 * 4 + 0] = 0xff;
      colors[1 * 4 + 1] = 0xff;
      colors[1 * 4 + 2] = 0xff;
      colors[2 * 4 + 0] = red;
      colors[2 * 4 + 1] = green;
      colors[2 * 4 + 2] = blue;
      colors[3 * 4 + 0] = 0x00;
      colors[3 * 4 + 1] = 0x00;
      colors[3 * 4 + 2] = 0x00;
      break;
    case 1:
      colors[0 * 4 + 0] = red;
      colors[0 * 4 + 1] = green;
      colors[0 * 4 + 2] = blue;
      colors[1 * 4 + 0] = 0xff;
      colors[1 * 4 + 1] = 0xff;
      colors[1 * 4 + 2] = 0xff;
      colors[2 * 4 + 0] = red;
      colors[2 * 4 + 1] = green;
      colors[2 * 4 + 2] = blue;
      colors[3 * 4 + 0] = red   / 2;
      colors[3 * 4 + 1] = green / 2;
      colors[3 * 4 + 2] = blue  / 2;
      break;
    case 2:
      colors[0 * 4 + 0] = red   / 2;
      colors[0 * 4 + 1] = green / 2;
      colors[0 * 4 + 2] = blue  / 2;
      colors[1 * 4 + 0] = 0x00;
      colors[1 * 4 + 1] = 0x00;
      colors[1 * 4 + 2] = 0x00;
      colors[2 * 4 + 0] = red   / 2;
      colors[2 * 4 + 1] = green / 2;
      colors[2 * 4 + 2] = blue  / 2;
      colors[3 * 4 + 0] = 0xff;
      colors[3 * 4 + 1] = 0xff;
      colors[3 * 4 + 2] = 0xff;
      break;
    case 3: /* like 0 but less shiny */
      colors[0 * 4 + 0] = red;
      colors[0 * 4 + 1] = green;
      colors[0 * 4 + 2] = blue;
      colors[1 * 4 + 0] = 0x40;
      colors[1 * 4 + 1] = 0x40;
      colors[1 * 4 + 2] = 0x40;
      colors[2 * 4 + 0] = red;
      colors[2 * 4 + 1] = green;
      colors[2 * 4 + 2] = blue;
      colors[3 * 4 + 0] = 0x00;
      colors[3 * 4 + 1] = 0x00;
      colors[3 * 4 + 2] = 0x00;
      break;
    case 4: /* testing mode, all white */
      for(int i = 0; i < 4; i++)
        for(int j = 0; j < 3; j++)
          colors[i * 4 + j] = 0xff;
      break;
    }

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      if(buffer == NULL)
        {
          buffer = gfxbuf_new(GFXBUF_DYNAMIC_2D, GL_LINE_LOOP, GFXBUF_COLOUR | GFXBUF_BLENDING);
          gfxbuf_resize(buffer, 4);
        }

      assert(buffer != NULL);
      if(buffer != NULL)
        {
          int vpos, cpos;

          vpos = 0;
          cpos = 0;

          buffer->vbuf[vpos++] = 0;
          buffer->vbuf[vpos++] = r->h - 1;
          for(int i = 0; i < 4; i++)
            buffer->cbuf[cpos++] = colors[0 * 4 + i];

          buffer->vbuf[vpos++] = 0;
          buffer->vbuf[vpos++] = 0;
          for(int i = 0; i < 4; i++)
            buffer->cbuf[cpos++] = colors[1 * 4 + i];

          buffer->vbuf[vpos++] = r->w - 1;
          buffer->vbuf[vpos++] = 0;
          for(int i = 0; i < 4; i++)
            buffer->cbuf[cpos++] = colors[2 * 4 + i];

          buffer->vbuf[vpos++] = r->w - 1;
          buffer->vbuf[vpos++] = r->h - 1;
          for(int i = 0; i < 4; i++)
            buffer->cbuf[cpos++] = colors[3 * 4 + i];

          gfxbuf_update(buffer, 0, vpos / 2);
          gfxbuf_draw_at(buffer, r->x, r->y);
          GFX_GL_ERROR();
        }
#endif
    }
  else
    {
#ifdef WITH_NONOPENGL
      gfx_draw_vline(r->x,            r->y, r->h - 1, colors[0 * 4 + 0], colors[0 * 4 + 1], colors[0 * 4 + 2], colors[0 * 4 + 3]); /* left */
      gfx_draw_vline(r->x + r->w - 1, r->y, r->h - 1, colors[3 * 4 + 0], colors[3 * 4 + 1], colors[3 * 4 + 2], colors[3 * 4 + 3]); /* right */

      gfx_draw_hline(r->x, r->y,            r->w - 1, colors[0 * 4 + 0], colors[0 * 4 + 1], colors[0 * 4 + 2], colors[0 * 4 + 3]); /* top */
      gfx_draw_hline(r->x, r->y + r->h - 1, r->w - 1, colors[3 * 4 + 0], colors[3 * 4 + 1], colors[3 * 4 + 2], colors[3 * 4 + 3]); /* bottom */
#endif
    }
}
