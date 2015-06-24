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

#ifdef WITH_OPENGL

#include "gfxbuf.h"
#include "gfx.h"
#include "gc.h"
#include "backtrace.h"
#include "globals.h"
#include <assert.h>
#include <stdlib.h>

struct gfxbuf * gfxbuf_new(enum GFXBUF_TYPE type, GLenum primitive_type, uint8_t options)
{
  struct gfxbuf * rv;

  rv = malloc(sizeof(struct gfxbuf));
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->vertex_vbo     = 0;
      rv->vbuf           = NULL;
      rv->texture_vbo    = 0;
      rv->tbuf           = NULL;
      rv->colour_vbo     = 0;
      rv->cbuf           = NULL;

      rv->type           = type;
      rv->primitive_type = primitive_type;
      rv->options        = options;
      rv->vertices       = 0;
      rv->max_vertices   = 0;

#ifndef NDEBUG
      rv->backtrace = get_backtrace();
#endif
      
      gfxbuf_load(rv);
    }

  gc_new(GCT_GFXBUF, rv);

  return rv;
}

struct gfxbuf * gfxbuf_free(struct gfxbuf * buffer)
{
  assert(buffer != NULL);
  if(buffer != NULL)
    {
      gc_free(GCT_GFXBUF, buffer);

      gfxbuf_unload(buffer);
      free(buffer->vbuf);
      free(buffer->tbuf);
      free(buffer->cbuf);
#ifndef NDEBUG
      free(buffer->backtrace);
#endif
      free(buffer);
    }

  return NULL;
}


void gfxbuf_unload(struct gfxbuf * buffer)
{
  if(globals.opengl_1_5)
    {
      if(glIsBuffer(buffer->vertex_vbo))
        glDeleteBuffers(1, &buffer->vertex_vbo);
      if(buffer->options & GFXBUF_TEXTURE)
        if(glIsBuffer(buffer->texture_vbo))
          glDeleteBuffers(1, &buffer->texture_vbo);
      if(buffer->options & GFXBUF_COLOUR)
        if(glIsBuffer(buffer->colour_vbo))
          glDeleteBuffers(1, &buffer->colour_vbo);
    }
  else if(GLEW_ARB_vertex_buffer_object)
    {
      if(glIsBufferARB(buffer->vertex_vbo))
        glDeleteBuffersARB(1, &buffer->vertex_vbo);
      if(buffer->options & GFXBUF_TEXTURE)
        if(glIsBufferARB(buffer->texture_vbo))
          glDeleteBuffersARB(1, &buffer->texture_vbo);
      if(buffer->options & GFXBUF_COLOUR)
        if(glIsBufferARB(buffer->colour_vbo))
          glDeleteBuffersARB(1, &buffer->colour_vbo);
    }
  buffer->vertex_vbo  = 0;
  buffer->texture_vbo = 0;
  buffer->colour_vbo  = 0;
}


void gfxbuf_load(struct gfxbuf * buffer)
{
  if(globals.opengl_1_5)
    {
      if(buffer->vertex_vbo == 0)
        glGenBuffers(1, &buffer->vertex_vbo);
      if(buffer->options & GFXBUF_TEXTURE)
        if(buffer->texture_vbo == 0)
          glGenBuffers(1, &buffer->texture_vbo);
      if(buffer->options & GFXBUF_COLOUR)
        if(buffer->colour_vbo == 0)
          glGenBuffers(1, &buffer->colour_vbo);
    }
  else if(GLEW_ARB_vertex_buffer_object)
    {
      if(buffer->vertex_vbo == 0)
        glGenBuffersARB(1, &buffer->vertex_vbo);
      if(buffer->options & GFXBUF_TEXTURE)
        if(buffer->texture_vbo == 0)
          glGenBuffersARB(1, &buffer->texture_vbo);
      if(buffer->options & GFXBUF_COLOUR)
        if(buffer->colour_vbo == 0)
          glGenBuffersARB(1, &buffer->colour_vbo);
    }

  if(buffer->max_vertices > 0)
    {
      int bufsize, ub_bufsize;

      bufsize = buffer->max_vertices * sizeof(GLfloat);
      ub_bufsize = buffer->max_vertices * sizeof(GLubyte);
      assert(bufsize > 0);

      if(globals.opengl_1_5)
        {
          GLenum dynsta;
              
          if(buffer->type == GFXBUF_STATIC)
            dynsta = GL_STATIC_DRAW;
          else /* if(buffer->type == GFXBUF_DYNAMIC) */
            dynsta = GL_DYNAMIC_DRAW;

          glBindBuffer(GL_ARRAY_BUFFER, buffer->vertex_vbo);
          glBufferData(GL_ARRAY_BUFFER, bufsize * 2, buffer->vbuf, dynsta);

          if(buffer->options & GFXBUF_TEXTURE)
            {
              glBindBuffer(GL_ARRAY_BUFFER, buffer->texture_vbo);
              glBufferData(GL_ARRAY_BUFFER, bufsize * 2, buffer->tbuf, dynsta);
            }

          if(buffer->options & GFXBUF_COLOUR)
            {
              glBindBuffer(GL_ARRAY_BUFFER, buffer->colour_vbo);
              glBufferData(GL_ARRAY_BUFFER, ub_bufsize * 4, buffer->cbuf, dynsta);
            }
        }
      else if(GLEW_ARB_vertex_buffer_object)
        {
          GLenum dynsta;
              
          if(buffer->type == GFXBUF_STATIC)
            dynsta = GL_STATIC_DRAW_ARB;
          else /* if(buffer->type == GFXBUF_DYNAMIC) */
            dynsta = GL_DYNAMIC_DRAW_ARB;

          glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->vertex_vbo);
          glBufferDataARB(GL_ARRAY_BUFFER_ARB, bufsize * 2, buffer->vbuf, dynsta);

          if(buffer->options & GFXBUF_TEXTURE)
            {
              glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->texture_vbo);
              glBufferDataARB(GL_ARRAY_BUFFER_ARB, bufsize * 2, buffer->tbuf, dynsta);
            }

          if(buffer->options & GFXBUF_COLOUR)
            {
              glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->colour_vbo);
              glBufferDataARB(GL_ARRAY_BUFFER_ARB, ub_bufsize * 4, buffer->cbuf, dynsta);
            }
        }
    }
}


bool gfxbuf_resize(struct gfxbuf * buffer, int max_vertices)
{
  bool rv;

  rv = true;
  assert(buffer != NULL);
  if(max_vertices > buffer->max_vertices)
    {
      void * tmp;

      tmp = realloc(buffer->vbuf, sizeof(GLfloat) * max_vertices * 2);
      assert(tmp != NULL);
      if(tmp != NULL)
        buffer->vbuf = tmp;
      else
        rv = false;

      if(buffer->options & GFXBUF_TEXTURE)
        {
          tmp = realloc(buffer->tbuf, sizeof(GLfloat) * max_vertices * 2);
          assert(tmp != NULL);
          if(tmp != NULL)
            buffer->tbuf = tmp;
          else
            rv = false;
        }

      if(buffer->options & GFXBUF_COLOUR)
        {
          tmp = realloc(buffer->cbuf, sizeof(GLubyte) * max_vertices * 4);
          assert(tmp != NULL);
          if(tmp != NULL)
            buffer->cbuf = tmp;
          else
            rv = false;
        }

      assert(rv == true);
      if(rv == true)
        {
          buffer->max_vertices = max_vertices;
          assert(buffer->vertices <= max_vertices);
          gfxbuf_load(buffer);
        }
    }

  return rv;
}


void gfxbuf_update(struct gfxbuf * buffer, int start, int count)
{
  int startpos, ub_startpos;
  int size, ub_size;

  startpos = start * sizeof(GLfloat);
  ub_startpos = start * sizeof(GLubyte);
  size = count * sizeof(GLfloat);
  ub_size = count * sizeof(GLubyte);

  assert(buffer != NULL);
  assert(start + count <= buffer->max_vertices);

  if(count > 0)
    {
      if(globals.opengl_1_5)
        {
          glBindBuffer(GL_ARRAY_BUFFER, buffer->vertex_vbo);
          glBufferSubData(GL_ARRAY_BUFFER, startpos * 2, size * 2, buffer->vbuf);
          glVertexPointer(2, GL_FLOAT, 0, NULL);

          if(buffer->options & GFXBUF_TEXTURE)
            {
              glBindBuffer(GL_ARRAY_BUFFER, buffer->texture_vbo);
              glBufferSubData(GL_ARRAY_BUFFER, startpos * 2, size * 2, buffer->tbuf);
              glTexCoordPointer(2, GL_FLOAT, 0, NULL);
            }

          if(buffer->options & GFXBUF_COLOUR)
            {
              glBindBuffer(GL_ARRAY_BUFFER, buffer->colour_vbo);
              glBufferSubData(GL_ARRAY_BUFFER, ub_startpos * 4, ub_size * 4, buffer->cbuf);
              glColorPointer(4, GL_UNSIGNED_BYTE, 0, NULL);
            }
        }
      else if(GLEW_ARB_vertex_buffer_object)
        {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->vertex_vbo);
          glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, startpos * 2, size * 2, buffer->vbuf);
          glVertexPointer(2, GL_FLOAT, 0, NULL);

          if(buffer->options & GFXBUF_TEXTURE)
            {
              glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->texture_vbo);
              glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, startpos * 2, size * 2, buffer->tbuf);
              glTexCoordPointer(2, GL_FLOAT, 0, NULL);
            }

          if(buffer->options & GFXBUF_COLOUR)
            {
              glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->colour_vbo);
              glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ub_startpos * 4, ub_size * 4, buffer->cbuf);
              glColorPointer(4, GL_UNSIGNED_BYTE, 0, NULL);
            }
        }
      else
        {
          glVertexPointer(2, GL_FLOAT, 0, buffer->vbuf);
          if(buffer->options & GFXBUF_TEXTURE)
            glTexCoordPointer(2, GL_FLOAT, 0, buffer->tbuf);
          if(buffer->options & GFXBUF_COLOUR)
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, buffer->cbuf);
        }
    }

  if(buffer->vertices < start + count)
    buffer->vertices = start + count;
}

void gfxbuf_draw_init(struct gfxbuf * buffer)
{
  assert(buffer != NULL);

  if(globals.opengl_1_5)
    {
      glBindBuffer(GL_ARRAY_BUFFER, buffer->vertex_vbo);
      glVertexPointer(2, GL_FLOAT, 0, NULL);

      if(buffer->options & GFXBUF_TEXTURE)
        {
          glBindBuffer(GL_ARRAY_BUFFER, buffer->texture_vbo);
          glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        }

      if(buffer->options & GFXBUF_COLOUR)
        {
          glBindBuffer(GL_ARRAY_BUFFER, buffer->colour_vbo);
          glColorPointer(4, GL_UNSIGNED_BYTE, 0, NULL);
        }
    }
  else if(GLEW_ARB_vertex_buffer_object)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->vertex_vbo);
      glVertexPointer(2, GL_FLOAT, 0, NULL);

      if(buffer->options & GFXBUF_TEXTURE)
        {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->texture_vbo);
          glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        }

      if(buffer->options & GFXBUF_COLOUR)
        {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->colour_vbo);
          glColorPointer(4, GL_UNSIGNED_BYTE, 0, NULL);
        }
    }
  else
    {
      glVertexPointer(2, GL_FLOAT, 0, buffer->vbuf);
      if(buffer->options & GFXBUF_TEXTURE)
        glTexCoordPointer(2, GL_FLOAT, 0, buffer->tbuf);
      if(buffer->options & GFXBUF_COLOUR)
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, buffer->cbuf);
    }
}


void gfxbuf_draw(struct gfxbuf * buffer)
{
  if(buffer->options & GFXBUF_BLENDING)
    glEnable(GL_BLEND);
  if(buffer->options & GFXBUF_TEXTURE)
    {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if(buffer->options & GFXBUF_COLOUR)
    glEnableClientState(GL_COLOR_ARRAY);
  GFX_GL_ERROR();

  glDrawArrays(buffer->primitive_type, 0, buffer->vertices);
  GFX_GL_ERROR();

  if(buffer->options & GFXBUF_BLENDING)
    glDisable(GL_BLEND);
  if(buffer->options & GFXBUF_TEXTURE)
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
    }
  if(buffer->options & GFXBUF_COLOUR)
    glDisableClientState(GL_COLOR_ARRAY);
}


void gfxbuf_draw_at(struct gfxbuf * buffer, float x, float y)
{
  glPushMatrix();
  glTranslatef(x, y, 0.0f);

  gfxbuf_draw(buffer);

  glPopMatrix();
}


#endif
