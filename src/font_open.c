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
#include "gfxbuf.h"
#include "gfx_material.h"
#include "globals.h"
#include "image.h"
#include "texture.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

struct configchar
{
  int  x, y;
  int  endx;
};


static struct configchar * font_open_configuration(struct font * font, const char * filename);


struct font * font_open(const char * name)
{
  struct font * font;

  font = malloc(sizeof(struct font));
  assert(font != NULL);
  if(font != NULL)
    {
      struct configchar * fontdata;

      font->image      = NULL;
      font->characters = NULL;
      font->height     = 0;

      fontdata = font_open_configuration(font, name);
      if(fontdata != NULL)
        {
          int ccount;
          char filename[strlen(name) + strlen(".png") + 1];

          ccount = 0;
          for(int i = '!'; i <= '~'; i++)
            ccount++;

          snprintf(filename, sizeof filename, "%s.png", name);
          font->image = image_load(get_data_filename(filename), true);
          assert(font->image != NULL);
          if(font->image != NULL)
            {
              font->characters = malloc(sizeof(struct fontcharacter) * ccount);
              assert(font->characters != NULL);
              if(font->characters != NULL)
                {
                  for(int i = '!'; i <= '~'; i++)
                    {
                      int c;
                      struct fontcharacter * fc;

                      c = i - '!';

                      fc = &font->characters[c];

                      fc->width = fontdata[c].endx - fontdata[c].x - 1;
#ifdef WITH_OPENGL
                      /* Also swap y coordinates for OpenGL */
                      fc->texture_coordinates[0] = ((float) fontdata[c].x + 0.5f) / (float) font->image->width;
                      fc->texture_coordinates[1] = ((float) (font->image->height - fontdata[c].y) - 0.5f) / (float) font->image->height;          
                      fc->texture_coordinates[2] = ((float) fontdata[c].endx - 0.5f) / (float) font->image->width;
                      fc->texture_coordinates[3] = ((float) (font->image->height - fontdata[c].y - font->height - 1) + 0.5f) / (float) font->image->height;
#endif
#ifdef WITH_NONOPENGL
                      fc->coordinates[0] = fontdata[c].x;
                      fc->coordinates[1] = fontdata[c].y;
#endif
                    }
#ifdef WITH_OPENGL
                  font->gfxbuf = NULL;
                  if(globals.opengl)
                    {
                      texture_setup_from_image(font->image, true, false, false);

                      font->gfxbuf = gfxbuf_new(GFXBUF_DYNAMIC_2D, GL_QUADS, GFXBUF_TEXTURE | GFXBUF_BLENDING);
                      assert(font->gfxbuf != NULL);
                      if(font->gfxbuf != NULL)
                        {
                          font->gfxbuf->texture_image = font->image;
                          gfxbuf_resize(font->gfxbuf, 256 * 4);
                          font->gfxbuf->material = gfx_material_default();
                        }
                      else
                        font = font_close(font);
                    }
#endif
#ifdef WITH_NONOPENGL
                  if(globals.opengl)
                    { /* Using OpenGL -> no need for the SDL image anymore. */
                      SDL_FreeSurface(font->image->sdl_surface);
                      font->image->sdl_surface = NULL;
                    }
                  else
                    { /* Not using OpenGL, remove all pixels with low alpha from the SDL image to make the font look prettier. */
                      Uint32 pixel;

                      pixel = SDL_MapRGBA(font->image->sdl_surface->format, 0x00, 0x00, 0x00, 0x00);
                      SDL_SetColorKey(font->image->sdl_surface, SDL_SRCCOLORKEY, pixel);

                      for(int y = 0; y < font->image->height; y++)
                        for(int x = 0; x < font->image->width; x++)
                          if(font->image->data[(x + y * font->image->width) * 4 + 3] < 0x80)
                            pixelRGBA(font->image->sdl_surface, x, font->image->height - y - 1, 0x00, 0x00, 0x00, 0xff);
                    }
#endif
                }
              else
                font = font_close(font);
            }
          else
            font = font_close(font);
        }
      else
        font = font_close(font);
    }

  font_set(font);

  return font;
}



static struct configchar * font_open_configuration(struct font * font, const char * filename)
{
  char fn[strlen(filename) + strlen(".txt") + 1];
  struct configchar * fontdata;
  char ** lines;

  fontdata = NULL;

  snprintf(fn, sizeof fn, "%s.txt", filename);
  lines = read_text_file(get_data_filename(fn));
  if(lines != NULL)
    {
      fontdata = malloc(sizeof(struct configchar) * 128);
      assert(fontdata != NULL);
      if(fontdata != NULL)
        {
          bool error;

          error = false;
          /* First line must have the height set: height=123 */
          if(lines[0] != NULL)
            {
              font->height = strtoul(lines[0] + strlen("height="), NULL, 0);

              /* Process the rest of the lines.
               * Format per line is:
               * C X Y XEND
               * Where C is the character. X, Y, and XEND are numbers.
               */
              for(int i = '!'; i <= '~'; i++)
                {
                  int lineind;

                  lineind = -1;
                  for(int j = 1; lineind == -1 && lines[j] != NULL; j++)
                    if(lines[j][0] == i)
                      lineind = j;

                  if(lineind > 0)
                    { /* Parse x, y, and xend from the lines[lineind]. */
                      char * endptr;

                      fontdata[i - '!'].x = strtoul(lines[lineind] + 1, &endptr, 0);
                      fontdata[i - '!'].y = strtoul(endptr, &endptr, 0);
                      fontdata[i - '!'].endx = strtoul(endptr, &endptr, 0);
                    }
                  else
                    {
                      char name[2];
                      
                      name[0] = i;
                      name[1] = '\0';
                      fprintf(stderr, "Error parsing '%s': character '%s' not found.\n", get_data_filename(fn), name);
                      error = true;
                    }
                }

              if(error == true)
                {
                  free(fontdata);
                  fontdata = NULL;
                }
            }
          else
            {
              fprintf(stderr, "Error parsing '%s': too few lines\n", get_data_filename(fn));
            }
        }
      else
        {
          fprintf(stderr, "%s: Error: failed to allocate memory: %s\n", get_data_filename(fn), strerror(errno));
        }
    }
  else
    {
      fprintf(stderr, "Failed to read '%s'.\n", get_data_filename(fn));
    }


  return fontdata;
}
