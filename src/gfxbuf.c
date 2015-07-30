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

#ifdef WITH_OPENGL

#include "gfxbuf.h"
#include "gfx.h"
#include "gc.h"
#include "backtrace.h"
#include "gfx_material.h"
#include "gfx_shaders.h"
#include "globals.h"
#include "image.h"
#include <assert.h>
#include <stdlib.h>

struct gfxbuf * gfxbuf_new(enum GFXBUF_TYPE type, GLenum primitive_type, uint8_t options)
{
  struct gfxbuf * rv;

  rv = malloc(sizeof(struct gfxbuf));
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->material      = NULL;
      rv->texture_image = NULL;
      
      rv->vertex_vbo     = 0;
      rv->vbuf           = NULL;
      rv->normal_vbo     = 0;
      rv->nbuf           = NULL;
      rv->texture_vbo    = 0;
      rv->tbuf           = NULL;
      rv->colour_vbo     = 0;
      rv->cbuf           = NULL;

      rv->type           = type;
      if(type == GFXBUF_STREAM_2D || type == GFXBUF_DYNAMIC_2D || type == GFXBUF_STATIC_2D)
        rv->ncoords      = 2;
      else /* if(type == GFXBUF_STREAM_3D ||type == GFXBUF_DYNAMIC_3D || type == GFXBUF_STATIC_3D) */
        rv->ncoords      = 3;
      rv->primitive_type = primitive_type;
      rv->options        = options;
      rv->vertices       = 0;
      rv->max_vertices   = 0;

#ifndef NDEBUG
      rv->backtrace = get_backtrace();
#endif
      
      gfxbuf_load(rv);

      gc_new(GCT_GFXBUF, rv);
    }

  return rv;
}

struct gfxbuf * gfxbuf_free(struct gfxbuf * buffer)
{
  assert(buffer != NULL);
  if(buffer != NULL)
    {
      gc_free(GCT_GFXBUF, buffer);

      gfxbuf_unload(buffer);
      if(buffer->texture_image != NULL)
        buffer->texture_image = image_free(buffer->texture_image);
      free(buffer->vbuf);
      free(buffer->nbuf);
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
      if(buffer->options & GFXBUF_NORMALS)
        if(glIsBuffer(buffer->normal_vbo))
          glDeleteBuffers(1, &buffer->normal_vbo);
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
      if(buffer->options & GFXBUF_NORMALS)
        if(glIsBufferARB(buffer->normal_vbo))
          glDeleteBuffersARB(1, &buffer->normal_vbo);
      if(buffer->options & GFXBUF_TEXTURE)
        if(glIsBufferARB(buffer->texture_vbo))
          glDeleteBuffersARB(1, &buffer->texture_vbo);
      if(buffer->options & GFXBUF_COLOUR)
        if(glIsBufferARB(buffer->colour_vbo))
          glDeleteBuffersARB(1, &buffer->colour_vbo);
    }
  buffer->vertex_vbo  = 0;
  buffer->normal_vbo  = 0;
  buffer->texture_vbo = 0;
  buffer->colour_vbo  = 0;
}


void gfxbuf_load(struct gfxbuf * buffer)
{
  if(globals.opengl_1_5)
    {
      if(buffer->vertex_vbo == 0)
        glGenBuffers(1, &buffer->vertex_vbo);
      if(buffer->options & GFXBUF_NORMALS)
        if(buffer->normal_vbo == 0)
          glGenBuffers(1, &buffer->normal_vbo);
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
      if(buffer->options & GFXBUF_NORMALS)
        if(buffer->normal_vbo == 0)
          glGenBuffersARB(1, &buffer->normal_vbo);
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
              
          if(buffer->type == GFXBUF_STATIC_2D || buffer->type == GFXBUF_STATIC_3D)
            dynsta = GL_STATIC_DRAW;
          else if(buffer->type == GFXBUF_STREAM_2D || buffer->type == GFXBUF_STREAM_3D)
            dynsta = GL_STREAM_DRAW;
          else /* if(buffer->type == GFXBUF_DYNAMIC_2D || buffer->type == GFXBUF_DYNAMIC_3D) */
            dynsta = GL_DYNAMIC_DRAW;

          glBindBuffer(GL_ARRAY_BUFFER, buffer->vertex_vbo);
          glBufferData(GL_ARRAY_BUFFER, bufsize * buffer->ncoords, buffer->vbuf, dynsta);

          if(buffer->options & GFXBUF_NORMALS)
            {
              glBindBuffer(GL_ARRAY_BUFFER, buffer->normal_vbo);
              glBufferData(GL_ARRAY_BUFFER, bufsize * 3, buffer->nbuf, dynsta);
            }

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
              
          if(buffer->type == GFXBUF_STATIC_2D || buffer->type == GFXBUF_STATIC_3D)
            dynsta = GL_STATIC_DRAW_ARB;
          else if(buffer->type == GFXBUF_STREAM_2D || buffer->type == GFXBUF_STREAM_3D)
            dynsta = GL_STREAM_DRAW_ARB;
          else /* if(buffer->type == GFXBUF_DYNAMIC_2D || buffer->type == GFXBUF_DYNAMIC_3D) */
            dynsta = GL_DYNAMIC_DRAW_ARB;

          glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->vertex_vbo);
          glBufferDataARB(GL_ARRAY_BUFFER_ARB, bufsize * buffer->ncoords, buffer->vbuf, dynsta);

          if(buffer->options & GFXBUF_NORMALS)
            {
              glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->normal_vbo);
              glBufferDataARB(GL_ARRAY_BUFFER_ARB, bufsize * 3, buffer->nbuf, dynsta);
            }

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


bool gfxbuf_resize(struct gfxbuf * buffer, unsigned int max_vertices)
{
  bool rv;

  rv = true;
  assert(buffer != NULL);
  if(max_vertices > buffer->max_vertices)
    {
      void * tmp;

      tmp = realloc(buffer->vbuf, sizeof(GLfloat) * max_vertices * buffer->ncoords);
      assert(tmp != NULL);
      if(tmp != NULL)
        buffer->vbuf = tmp;
      else
        rv = false;

      if(buffer->options & GFXBUF_NORMALS)
        {
          tmp = realloc(buffer->nbuf, sizeof(GLfloat) * max_vertices * 3);
          assert(tmp != NULL);
          if(tmp != NULL)
            buffer->nbuf = tmp;
          else
            rv = false;
        }
      
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


void gfxbuf_update(struct gfxbuf * buffer, unsigned int start, unsigned int count)
{
  unsigned int startpos, ub_startpos;
  unsigned int size, ub_size;

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
          glBufferSubData(GL_ARRAY_BUFFER, startpos * buffer->ncoords, size * buffer->ncoords, buffer->vbuf);
          glVertexPointer(buffer->ncoords, GL_FLOAT, 0, NULL);

          if(buffer->options & GFXBUF_NORMALS)
            {
              glBindBuffer(GL_ARRAY_BUFFER, buffer->normal_vbo);
              glBufferSubData(GL_ARRAY_BUFFER, startpos * 3, size * 3, buffer->nbuf);
              glNormalPointer(GL_FLOAT, 0, NULL);
            }

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
          glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, startpos * buffer->ncoords, size * buffer->ncoords, buffer->vbuf);
          glVertexPointer(buffer->ncoords, GL_FLOAT, 0, NULL);

          if(buffer->options & GFXBUF_NORMALS)
            {
              glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->normal_vbo);
              glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, startpos * 3, size * 3, buffer->nbuf);
              glNormalPointer(GL_FLOAT, 0, NULL);
            }

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
          glVertexPointer(buffer->ncoords, GL_FLOAT, 0, buffer->vbuf);
          if(buffer->options & GFXBUF_NORMALS)
            glNormalPointer(GL_FLOAT, 0, buffer->nbuf);
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
      glVertexPointer(buffer->ncoords, GL_FLOAT, 0, NULL);

      if(buffer->options & GFXBUF_NORMALS)
        {
          glBindBuffer(GL_ARRAY_BUFFER, buffer->normal_vbo);
          glNormalPointer(GL_FLOAT, 0, NULL);
        }

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
      glVertexPointer(buffer->ncoords, GL_FLOAT, 0, NULL);

      if(buffer->options & GFXBUF_NORMALS)
        {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer->normal_vbo);
          glNormalPointer(GL_FLOAT, 0, NULL);
        }

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
      glVertexPointer(buffer->ncoords, GL_FLOAT, 0, buffer->vbuf);
      if(buffer->options & GFXBUF_NORMALS)
        glNormalPointer(GL_FLOAT, 0, buffer->nbuf);
      if(buffer->options & GFXBUF_TEXTURE)
        glTexCoordPointer(2, GL_FLOAT, 0, buffer->tbuf);
      if(buffer->options & GFXBUF_COLOUR)
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, buffer->cbuf);
    }
}


void gfxbuf_draw(struct gfxbuf * buffer)
{
  GFX_GL_ERROR();

  gfxgl_state(GL_BLEND, (buffer->options & GFXBUF_BLENDING) ? true : false);

  if(buffer->material != NULL)
    gfx_material_use(buffer->material);
  
  
  gfxgl_client_state(GL_NORMAL_ARRAY, (buffer->options & GFXBUF_NORMALS) ? true : false);

  if(buffer->options & GFXBUF_TEXTURE)
    {
      gfxgl_state(GL_TEXTURE_2D, true);

      if(buffer->texture_image != NULL)
        gfxgl_bind_texture(buffer->texture_image->texture);

      gfxgl_client_state(GL_TEXTURE_COORD_ARRAY, true);
    }
  else
    {
      gfxgl_state(GL_TEXTURE_2D, false);
      gfxgl_client_state(GL_TEXTURE_COORD_ARRAY, false);
    }


  if(buffer->options & GFXBUF_COLOUR)
    {
      gfxgl_client_state(GL_COLOR_ARRAY, true);
      gfx_material_use_invalidate_colour();
    }
  else
    gfxgl_client_state(GL_COLOR_ARRAY, false);

  gfx_shaders_apply();
  
  glDrawArrays(buffer->primitive_type, 0, buffer->vertices);
  GFX_GL_ERROR();
}


void gfxbuf_draw_at(struct gfxbuf * buffer, float x, float y)
{
  glPushMatrix();
  glTranslatef(x, y, 0.0f);

  gfxbuf_draw(buffer);

  glPopMatrix();
}

#ifndef NDEBUG
void gfxbuf_dump(struct gfxbuf const * buffer)
{
  printf("gfxbuf-%p:\n", buffer);
  if(buffer != NULL)
    {
      printf(" vertices = %u / %u\n", buffer->vertices, buffer->max_vertices);
      printf(" ncoords = %u\n", buffer->ncoords);
    }
}
#endif


#endif
