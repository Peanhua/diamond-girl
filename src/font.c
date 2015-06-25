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

#include "diamond_girl.h"
#include "font.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "globals.h"
#include "image.h"
#include "texture.h"
#include <assert.h>

static struct font * current_font = NULL;


struct font * font_get(void)
{
  assert(current_font != NULL);

  return current_font;
}

struct font * font_set(struct font * font)
{
  current_font = font;

  return current_font;
}




struct font * font_close(struct font * font)
{
  if(font == NULL)
    font = current_font;

  if(font != NULL)
    {
      if(current_font == font)
        current_font = NULL;

      font->image = image_free(font->image);
#ifdef WITH_OPENGL
      free(font->characters);
#endif
      free(font);
    }

  return NULL;
}


int font_height(void)
{
  assert(current_font != NULL);

  return current_font->height;
}


int font_width(const char * text)
{
  assert(current_font != NULL);

  return font_nwidth(text, 0);
}

int font_nwidth(const char * text, int textlen)
{
  int len;

  assert(current_font != NULL);

  len = 0;
  for(int i = 0; (textlen == 0 || i < textlen) && text[i] != '\0'; i++)
    if(text[i] == ' ')
      len += current_font->height / 3;
    else if(text[i] >= '!' && text[i] <= '~')
      len += current_font->characters[text[i] - '!'].width;

  return len;
}


void font_write(int x, int y, const char * text)
{
  assert(current_font != NULL);
  assert(strlen(text) < 256);

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      current_font->gfxbuf->vertices = 0;
      font_render(x, y, text, current_font->gfxbuf);
      
      gfxbuf_update(current_font->gfxbuf, 0, current_font->gfxbuf->vertices);
      
      gfx_colour_white();
      gfxgl_bind_texture(current_font->image->texture);
      gfxbuf_draw(current_font->gfxbuf);
#endif
    }
  else
    {
#ifdef WITH_NONOPENGL
      while(*text != '\0')
        {
          int ch;

          ch = *text;
          if(ch == ' ')
            x += current_font->height / 3;
          else if(ch >= '!' && ch <= '~')
            {
              SDL_Rect src, dst;

              ch -= '!';

              src.x = current_font->characters[ch].coordinates[0];
              src.y = current_font->characters[ch].coordinates[1];
              src.w = current_font->characters[ch].width;
              src.h = current_font->height;

              dst.x = x;
              dst.y = y;

              SDL_BlitSurface(current_font->image->sdl_surface, &src, gfx_game_screen(), &dst);

              x += src.w;
            }
          text++;
        }
#endif
    }
}


#ifdef WITH_OPENGL
void font_render(int x, int y, const char * text, struct gfxbuf * gfxbuf)
{
  int vpos, tpos;

  assert(current_font != NULL);
  assert(text != NULL);
  assert(globals.opengl);
  assert(strlen(text) < 256);
  assert(gfxbuf->primitive_type == GL_QUADS);

  vpos = tpos = gfxbuf->vertices * 2;

  while(*text != '\0')
    {
      int ch;

      ch = *text;
      if(ch == ' ')
        x += current_font->height / 3;
      else if(ch >= '!' && ch <= '~')
        {
          int width;

          ch -= '!';

          width = current_font->characters[ch].width;

          gfxbuf->vbuf[vpos++] = x;
          gfxbuf->vbuf[vpos++] = y;
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[0] + current_font->texture_offset[0];
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[1] - current_font->texture_offset[1];
          
          gfxbuf->vbuf[vpos++] = x + width - 1;
          gfxbuf->vbuf[vpos++] = y;
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[2] - current_font->texture_offset[0];
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[1] - current_font->texture_offset[1];
          
          gfxbuf->vbuf[vpos++] = x + width - 1;
          gfxbuf->vbuf[vpos++] = y + current_font->height;
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[2] - current_font->texture_offset[0];
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[3] + current_font->texture_offset[1];
      
          gfxbuf->vbuf[vpos++] = x;
          gfxbuf->vbuf[vpos++] = y + current_font->height;
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[0] + current_font->texture_offset[0];
          gfxbuf->tbuf[tpos++] = current_font->characters[ch].texture_coordinates[3] + current_font->texture_offset[1];

          x += width;
        }

      text++;
    }

  gfxbuf->vertices = vpos / 2;
}
#endif
