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
#include "stack.h"
#include "td_object.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

static double td_object_mesh_bounding_sphere_radius(struct td_object_mesh * mesh);



struct td_object * td_object_load(char const * const filename, float * material1, float * gemstone1)
{
  struct td_object * rv;

  rv = td_object_load_assimp(filename, material1, gemstone1);
  if(rv != NULL)
    {
      /* Calculate the bounding sphere */
      rv->bounding_sphere_radius = td_object_mesh_bounding_sphere_radius(rv->root);
    }

  return rv;
}



static double gfxbuf_bounding_sphere_radius(struct gfxbuf * buf)
{
  double rv;

  rv = 0.0;
  assert(buf->ncoords == 3);
  for(unsigned int i = 0; i < buf->vertices; i++)
    {
      double x, y, z, len;

      x = buf->vbuf[i * buf->ncoords + 0];
      y = buf->vbuf[i * buf->ncoords + 1];
      z = buf->vbuf[i * buf->ncoords + 2];

      len = sqrt(x * x + y * y + z * z);
      
      if(len > rv)
        rv = len;
    }

  return rv;
}


static double td_object_mesh_bounding_sphere_radius(struct td_object_mesh * mesh)
{ /* This assumes the center is at origin, and does not take into account mesh->matrix */
  double radius;

  radius = 0.0;

  for(unsigned int i = 0; i < mesh->gfxbufs->size; i++)
    {
      double r;
      
      r = gfxbuf_bounding_sphere_radius(mesh->gfxbufs->data[i]);
      if(r > radius)
        radius = r;
    }

  for(unsigned int i = 0; i < mesh->children->size; i++)
    {
      double r;

      r = td_object_mesh_bounding_sphere_radius(mesh->children->data[i]);
      if(r > radius)
        radius = r;
    }
  
  return radius;
}


#endif
