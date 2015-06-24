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

#ifndef GFXBUF_H_
#define GFXBUF_H_

#ifdef WITH_OPENGL

#include <GL/glew.h>
#include <stdbool.h>
#include <inttypes.h>

enum GFXBUF_TYPE
  {
    GFXBUF_DYNAMIC,
    GFXBUF_STATIC
  };

#define GFXBUF_TEXTURE  (1<<0)
#define GFXBUF_COLOUR   (1<<1)
#define GFXBUF_BLENDING (1<<2)

struct gfxbuf
{
  GLuint    vertex_vbo;
  GLfloat * vbuf;

  GLuint    texture_vbo;
  GLfloat * tbuf;

  GLuint    colour_vbo;
  GLubyte * cbuf;

  int vertices;
  int max_vertices;

  enum GFXBUF_TYPE type; /* dynamic or static */
  GLenum           primitive_type;
  uint8_t          options;

#ifndef NDEBUG
  char * backtrace;
#endif
};

extern struct gfxbuf * gfxbuf_new(enum GFXBUF_TYPE type, GLenum primitive_type, uint8_t options);
extern struct gfxbuf * gfxbuf_free(struct gfxbuf * buffer);

extern bool gfxbuf_resize(struct gfxbuf * buffer, int max_vertices);
extern void gfxbuf_update(struct gfxbuf * buffer, int start, int count); /* Does also gfxbuf_draw_init(). */
extern void gfxbuf_draw_init(struct gfxbuf * buffer);                    /* Set vertex etc. pointers. */
extern void gfxbuf_draw(struct gfxbuf * buffer);                         /* Does not set: colour, texture */
extern void gfxbuf_draw_at(struct gfxbuf * buffer, float x, float y);    /* Does not set: colour, texture */

extern void gfxbuf_load(struct gfxbuf * buffer);
extern void gfxbuf_unload(struct gfxbuf * buffer);


#endif

#endif
