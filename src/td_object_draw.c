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

#include "td_object.h"
#include "globals.h"
#include "gfx.h"
#include "image.h"

#ifdef LIB3DS_V1
# include <lib3ds/types.h>
# include <lib3ds/vector.h>
#else
# include <lib3ds.h>
#endif

void td_object_draw(struct td_object * td_object)
{
  if(globals.opengl_1_5)
    {
      glBindBuffer(GL_ARRAY_BUFFER, td_object->vbo_colour);
      glColorPointer(3, GL_FLOAT, 0, NULL);

      glBindBuffer(GL_ARRAY_BUFFER, td_object->vbo_normal);
      glNormalPointer(GL_FLOAT, 0, NULL);
        
      glBindBuffer(GL_ARRAY_BUFFER, td_object->vbo_vertex);
      glVertexPointer(3, GL_FLOAT, 0, NULL);

      if(td_object->texture_image != NULL)
        {
          glBindBuffer(GL_ARRAY_BUFFER, td_object->vbo_texture_coordinates);
          glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        }
    }
  else if(GLEW_ARB_vertex_buffer_object)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, td_object->vbo_colour);
      glColorPointer(3, GL_FLOAT, 0, NULL);

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, td_object->vbo_normal);
      glNormalPointer(GL_FLOAT, 0, NULL);
        
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, td_object->vbo_vertex);
      glVertexPointer(3, GL_FLOAT, 0, NULL);

      if(td_object->texture_image != NULL)
        {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, td_object->vbo_texture_coordinates);
          glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        }
    }
  else
    {
      glColorPointer(3, GL_FLOAT,  0, td_object->colours);
      glNormalPointer(GL_FLOAT,    0, td_object->normals);
      glVertexPointer(3, GL_FLOAT, 0, td_object->vertices);
      if(td_object->texture_image != NULL)
        glTexCoordPointer(2, GL_FLOAT, 0, td_object->texture_coordinates);
    }
        
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  if(td_object->texture_image != NULL)
    {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  
  glPushMatrix();
  glMultMatrixf(td_object->matrix);
  if(td_object->texture_image != NULL)
    gfx_bind_texture(td_object->texture_image->texture);
  glDrawArrays(GL_TRIANGLES, 0, td_object->face_count * 3);
  glPopMatrix();
  
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  if(td_object->texture_image != NULL)
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
    }
}

#endif
