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

#ifndef TEXT_H_
#define TEXT_H_

struct image;
struct gfxbuf;

struct fontcharacter
{
  int   width;
#ifdef WITH_OPENGL
  float texture_coordinates[4];
#endif
#ifdef WITH_NONOPENGL
  int   coordinates[2];
#endif
};


struct font
{
  struct image *         image;
  int                    height;
  struct fontcharacter * characters;
#ifdef WITH_OPENGL
  struct gfxbuf *        gfxbuf;
  float                  texture_offset[2];
#endif
};

/* Initializing and cleanup */
extern struct font * font_open(const char * name);
extern struct font * font_close(struct font * font);

/* Current font get/set */
extern struct font * font_get(void);
extern struct font * font_set(struct font * font);

/* Everything below will use the current font: */

/* Querying */
extern int           font_height(void);                           /* Return the height of the font. */
extern int           font_width(const char * text);               /* Return the width of the text. */
extern int           font_nwidth(const char * text, int textlen); /* Return the width of the text with textlen. */

/* Drawing */
extern void          font_write(int x, int y, const char * text); /* Write to screen. */
#ifdef WITH_OPENGL
extern void          font_render(int x, int y, const char * text, struct gfxbuf * gfxbuf); /* Render to gfxbuf. */
#endif

#endif
