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
#include "image.h"

#include <stdlib.h>


struct td_object * td_object_unload(struct td_object * td_object)
{
  if(globals.opengl_1_5)
    {
      if(glIsBuffer(td_object->vbo_vertex))
        glDeleteBuffers(1, &td_object->vbo_vertex);
      
      if(glIsBuffer(td_object->vbo_normal))
        glDeleteBuffers(1, &td_object->vbo_normal);
      
      if(glIsBuffer(td_object->vbo_normal))
        glDeleteBuffers(1, &td_object->vbo_colour);

      if(glIsBuffer(td_object->vbo_texture_coordinates))
        glDeleteBuffers(1, &td_object->vbo_texture_coordinates);
    }
  else if(GLEW_ARB_vertex_buffer_object)
    {
      if(glIsBufferARB(td_object->vbo_vertex))
        glDeleteBuffersARB(1, &td_object->vbo_vertex);
      
      if(glIsBufferARB(td_object->vbo_normal))
        glDeleteBuffersARB(1, &td_object->vbo_normal);
      
      if(glIsBufferARB(td_object->vbo_normal))
        glDeleteBuffersARB(1, &td_object->vbo_colour);

      if(glIsBufferARB(td_object->vbo_texture_coordinates))
        glDeleteBuffersARB(1, &td_object->vbo_texture_coordinates);
    }

  free(td_object->vertices);
  free(td_object->normals);
  free(td_object->colours);
  free(td_object->texture_coordinates);
  
  if(td_object->texture_image != NULL)
    td_object->texture_image = image_free(td_object->texture_image);

  free(td_object);
  td_object = NULL;

  return td_object;
}

#endif
