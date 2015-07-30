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
#include "image.h"
#include "stack.h"

#include <assert.h>
#include <stdlib.h>

static struct td_object_mesh * td_object_mesh_unload(struct td_object_mesh * mesh);

struct td_object * td_object_unload(struct td_object * td_object)
{
  assert(td_object != NULL);

  if(td_object->root != NULL)
    td_object->root = td_object_mesh_unload(td_object->root);

#ifndef NDEBUG
  free(td_object->filename);
  td_object->filename = NULL;
#endif
  
  free(td_object);

  return NULL;
}


static struct td_object_mesh * td_object_mesh_unload(struct td_object_mesh * mesh)
{
  for(unsigned int i = 0; i < mesh->gfxbufs->size; i++)
    mesh->gfxbufs->data[i] = gfxbuf_free(mesh->gfxbufs->data[i]);
  
  mesh->gfxbufs = stack_free(mesh->gfxbufs);

  
  for(unsigned int i = 0; i < mesh->children->size; i++)
    mesh->children->data[i] = td_object_mesh_unload(mesh->children->data[i]);

  mesh->children = stack_free(mesh->children);

  free(mesh);

  return NULL;
}

#endif
