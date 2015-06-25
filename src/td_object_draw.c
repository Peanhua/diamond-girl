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

#include "td_object.h"
#include "globals.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "image.h"
#include "stack.h"


void td_object_draw(struct td_object * td_object)
{
  //glPushMatrix();
  for(unsigned int i = 0; i < td_object->meshes->size; i++)
    {
      struct td_object_mesh * mesh;
      
      mesh = td_object->meshes->data[i];
      //glMultMatrixf(mesh->matrix); // Commented out because it looks like exporting from Blender automatically applies location&rotation (perhaps scale as well, did not test it).
      if(mesh->texture_image != NULL)
        gfxgl_bind_texture(mesh->texture_image->texture);
      gfxbuf_draw_init(mesh->gfxbuf);
      gfxbuf_draw(mesh->gfxbuf);
    }
  //glPopMatrix();
}

#endif
