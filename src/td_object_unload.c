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
#include "gfxbuf.h"
#include "globals.h"
#include "image.h"
#include "stack.h"

#include <assert.h>
#include <stdlib.h>

struct td_object * td_object_unload(struct td_object * td_object)
{
  assert(td_object != NULL);

  for(unsigned int i = 0; i < td_object->meshes->size; i++)
    {
      struct td_object_mesh * mesh;
      
      mesh = td_object->meshes->data[i];
  
      if(mesh->gfxbuf != NULL)
        mesh->gfxbuf = gfxbuf_free(mesh->gfxbuf);
  
      if(mesh->texture_image != NULL)
        mesh->texture_image = image_free(mesh->texture_image);
    }

  td_object->meshes = stack_free(td_object->meshes);
  
#ifndef NDEBUG
  free(td_object->filename);
  td_object->filename = NULL;
#endif
  
  free(td_object);
  td_object = NULL;

  return td_object;
}

#endif
