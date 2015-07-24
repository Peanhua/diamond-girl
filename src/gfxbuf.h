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

#ifndef GFXBUF_H_
#define GFXBUF_H_

#ifdef WITH_OPENGL

#include <GL/glew.h>
#include <stdbool.h>
#include <inttypes.h>

struct image;
struct gfx_material;

enum GFXBUF_TYPE
  {
    GFXBUF_STREAM_2D,
    GFXBUF_STREAM_3D,
    GFXBUF_DYNAMIC_2D,
    GFXBUF_DYNAMIC_3D,
    GFXBUF_STATIC_2D,
    GFXBUF_STATIC_3D
  };

#define GFXBUF_BLENDING (1<<0)
#define GFXBUF_COLOUR   (1<<1)
#define GFXBUF_NORMALS  (1<<2)
#define GFXBUF_TEXTURE  (1<<3)

struct gfxbuf
{
  struct gfx_material * material;
  struct image *        texture_image;

  GLuint    vertex_vbo;
  GLfloat * vbuf;

  GLuint    normal_vbo;
  GLfloat * nbuf;

  GLuint    texture_vbo;
  GLfloat * tbuf;

  GLuint    colour_vbo;
  GLubyte * cbuf;

  unsigned int vertices;
  unsigned int max_vertices;

  enum GFXBUF_TYPE type; /* dynamic or static */
  unsigned int     ncoords; /* The number of vertex coordinates (2 or 3). */
  GLenum           primitive_type;
  uint8_t          options;

#ifndef NDEBUG
  char * backtrace;
#endif
};

extern struct gfxbuf * gfxbuf_new(enum GFXBUF_TYPE type, GLenum primitive_type, uint8_t options);
extern struct gfxbuf * gfxbuf_free(struct gfxbuf * buffer);

#ifdef NDEBUG
#define gfxbuf_dump(X)
#else
extern void gfxbuf_dump(struct gfxbuf const * buffer);
#endif

extern bool gfxbuf_resize(struct gfxbuf * buffer, unsigned int max_vertices);
extern void gfxbuf_update(struct gfxbuf * buffer, unsigned int start, unsigned int count); /* Does also gfxbuf_draw_init(). */
extern void gfxbuf_draw_init(struct gfxbuf * buffer);                    /* Set vertex etc. pointers. */
extern void gfxbuf_draw(struct gfxbuf * buffer);                         /* Does not set: colour, texture */
extern void gfxbuf_draw_at(struct gfxbuf * buffer, float x, float y);    /* Does not set: colour, texture */

extern void gfxbuf_load(struct gfxbuf * buffer);
extern void gfxbuf_unload(struct gfxbuf * buffer);

extern bool gfxbuf_merge(struct gfxbuf * destination, struct gfxbuf * source); /* Return true if successful. */

#endif

#endif
