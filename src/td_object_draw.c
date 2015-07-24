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
#include "stack.h"

static void draw_mesh(struct td_object_mesh * mesh);


void td_object_draw(struct td_object * td_object)
{
  draw_mesh(td_object->root);
}


static void draw_mesh(struct td_object_mesh * mesh)
{
  //glPushMatrix();
  //glMultMatrixf(mesh->matrix); // Commented out because it looks like exporting from Blender automatically applies location&rotation (perhaps scale as well, did not test it).
  for(unsigned int i = 0; i < mesh->gfxbufs->size; i++)
    {
      struct gfxbuf * b;

      b = mesh->gfxbufs->data[i];
      gfxbuf_draw_init(b);
      gfxbuf_draw(b);
    }

  for(unsigned int i = 0; i < mesh->children->size; i++)
    draw_mesh(mesh->children->data[i]);
  
  //glPopMatrix();
}



#endif
