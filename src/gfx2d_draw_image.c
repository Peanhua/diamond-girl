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

#include "gfx.h"
#include "gfxbuf.h"
#include "image.h"
#include "globals.h"

#include <assert.h>
#ifdef WITH_NONOPENGL
#include <SDL_rotozoom.h>
#endif

void gfx2d_draw_image(int x, int y, struct image * image)
{
  gfx2d_draw_image_scaled(x, y, image, image->content_width, image->content_height);
}


void gfx2d_draw_image_scaled(int x, int y, struct image * image, int scale_to_width, int scale_to_height)
{
  assert(image != NULL);
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      assert(image->texture_initialized == true);
      assert(image->buffer != NULL);

      if(image->bufferw != scale_to_width || image->bufferh != scale_to_height)
        {
          image->bufferw = scale_to_width;
          image->bufferh = scale_to_height;

          float toffx, toffy;
          float w, h;
          int vpos, tpos;

          vpos = 0;
          tpos = 0;

          toffx = 0.5f / (float) image->width;
          toffy = 0.5f / (float) image->height;

          w = (float) image->content_width  / (float) image->width  - toffx;
          h = (float) image->content_height / (float) image->height - toffy;

          image->buffer->tbuf[tpos++] = toffx;
          image->buffer->tbuf[tpos++] = h;
          image->buffer->vbuf[vpos++] = 0;
          image->buffer->vbuf[vpos++] = 0;

          image->buffer->tbuf[tpos++] = w;
          image->buffer->tbuf[tpos++] = h;
          image->buffer->vbuf[vpos++] = scale_to_width;
          image->buffer->vbuf[vpos++] = 0;

          image->buffer->tbuf[tpos++] = w;
          image->buffer->tbuf[tpos++] = toffy;
          image->buffer->vbuf[vpos++] = scale_to_width;
          image->buffer->vbuf[vpos++] = scale_to_height;

          image->buffer->tbuf[tpos++] = toffx;
          image->buffer->tbuf[tpos++] = toffy;
          image->buffer->vbuf[vpos++] = 0;
          image->buffer->vbuf[vpos++] = scale_to_height;

          gfxbuf_update(image->buffer, 0, 4);
        }

      gfx_colour_white();
      gfxgl_bind_texture(image->texture);
      gfxbuf_draw_init(image->buffer);
      gfxbuf_draw_at(image->buffer, x, y);
#endif
    }
  else
    {
#ifdef WITH_NONOPENGL
      if(image->sdl_surface == NULL)
        image_to_sdl(image);
      assert(image->sdl_surface != NULL);
      if(image->sdl_surface != NULL)
        {
          SDL_Rect dst;

          dst.x = x;
          dst.y = y;
          if(image->width == scale_to_width && image->height == scale_to_height)
            {
              SDL_BlitSurface(image->sdl_surface, NULL, gfx_game_screen(), &dst);
            }
          else
            {
              SDL_Surface * tmp;

              tmp = zoomSurface(image->sdl_surface, (float) scale_to_width / (float) image->width, (float) scale_to_height / (float) image->height, 0);
              SDL_BlitSurface(tmp, NULL, gfx_game_screen(), &dst);
              SDL_FreeSurface(tmp);
            }
        }
#endif
    }
}
