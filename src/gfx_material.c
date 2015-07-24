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

#include "gfx.h"
#include "gfx_material.h"
#include <assert.h>
#include <stdlib.h>
#include <GL/glew.h>


struct gfx_material * gfx_material_new(void)
{
  struct gfx_material * rv;

  rv = malloc(sizeof *rv);
  assert(rv != NULL);
  if(rv != NULL)
    {
      gfx_material_copy(rv, gfx_material_default());
      rv->changed = 0;
      rv->use_count = 1;
    }
  
  return rv;
}


struct gfx_material * gfx_material_delete(struct gfx_material * material)
{
  assert(material != gfx_material_default());
  assert(material->use_count > 0);
  material->use_count--;
  if(material->use_count == 0)
    gfx_material_free_(material);

  return NULL;
}



void gfx_material_free_(struct gfx_material * material)
{
  assert(material != gfx_material_default());
  free(material);
}

struct gfx_material * gfx_material_default(void)
{
  static struct gfx_material defaultmaterial =
    {
      .colour   = { 1.0, 1.0, 1.0, 1.0 },
      
      .ambient  = { 0.2, 0.2, 0.2, 1.0 },
      .diffuse  = { 1.0, 1.0, 1.0, 1.0 },
      .specular = { 1.0, 1.0, 1.0, 1.0 },
      .emission = { 0.0, 0.0, 0.0, 1.0 },
      .shininess = 128,

      .changed   = 0,
      .use_count = 1
    };

  return &defaultmaterial;
}


void gfx_material_copy(struct gfx_material * destination, struct gfx_material const * source)
{
  for(int i = 0; i < 4; i++)
    {
      destination->colour[i]   = source->colour[i];
      destination->ambient[i]  = source->ambient[i];
      destination->diffuse[i]  = source->diffuse[i];
      destination->specular[i] = source->specular[i];
      destination->emission[i] = source->emission[i];
    }

  destination->shininess = source->shininess;
  
  destination->changed = 0xff;
}


static struct gfx_material * current_material = NULL;
#ifndef NDEBUG
static struct gfx_material * default_copy = NULL;
#endif
static bool current_material_invalid_colour;

void gfx_material_use_initialize(void)
{
  if(current_material == NULL)
    {
      current_material = gfx_material_default();
      current_material->use_count++;
      current_material_invalid_colour = true;
#ifndef NDEBUG
      default_copy = gfx_material_new();
#endif
    }
  else
    gfx_material_use(gfx_material_default());
}

void gfx_material_use_invalidate_colour(void)
{
  current_material_invalid_colour = true;
}


void gfx_material_use(struct gfx_material * material)
{
  assert(current_material != NULL);
  assert(material != NULL);

#ifndef NDEBUG
  if(material == gfx_material_default())
    { /* Check that the default colour has not been modified. */
      for(int i = 0; i < 4; i++)
        {
          assert(default_copy->colour[i] == material->colour[i]);
          assert(default_copy->ambient[i] == material->ambient[i]);
          assert(default_copy->diffuse[i] == material->diffuse[i]);
          assert(default_copy->specular[i] == material->specular[i]);
          assert(default_copy->emission[i] == material->emission[i]);
        }
      assert(default_copy->shininess == material->shininess);
    }
#endif
  
  if(current_material_invalid_colour == true || material != current_material || material->changed != 0)
    {
      GFX_GL_ERROR();
      if(current_material_invalid_colour == true || (material->changed & GFX_MATERIAL_COLOUR) || memcmp(current_material->colour, material->colour, sizeof material->colour))
        glColor4fv(material->colour);

      if(gfxgl_light_get() != GFX_LIGHT_TYPE_NONE)
        {
          if((material->changed & GFX_MATERIAL_AMBIENT) || memcmp(current_material->ambient, material->ambient, sizeof material->ambient))
            glMaterialfv(GL_FRONT, GL_AMBIENT,   material->ambient);

          if((material->changed & GFX_MATERIAL_DIFFUSE) || memcmp(current_material->diffuse, material->diffuse, sizeof material->diffuse))
            glMaterialfv(GL_FRONT, GL_DIFFUSE,   material->diffuse);

          if((material->changed & GFX_MATERIAL_SPECULAR) || memcmp(current_material->specular, material->specular, sizeof material->specular))
            glMaterialfv(GL_FRONT, GL_SPECULAR,  material->specular);
      
          if((material->changed & GFX_MATERIAL_EMISSION) || memcmp(current_material->emission, material->emission, sizeof material->emission))
            glMaterialfv(GL_FRONT, GL_EMISSION,  material->emission);
      
          if((material->changed & GFX_MATERIAL_SHININESS) || current_material->shininess != material->shininess)
            glMateriali(GL_FRONT,  GL_SHININESS, material->shininess);
        }
      GFX_GL_ERROR();
      
      assert(current_material->use_count > 0);
      current_material->use_count--;
      
      material->use_count++;
      material->changed = 0;
      current_material = material;
      current_material_invalid_colour = false;
    }
}


void gfx_material_change4f(struct gfx_material * material, enum GFX_MATERIAL type, float red, float green, float blue, float alpha)
{
  if(type & GFX_MATERIAL_COLOUR)
    {
      material->colour[0] = red;
      material->colour[1] = green;
      material->colour[2] = blue;
      material->colour[3] = alpha;
    }
  material->changed |= type;
}


void gfx_material_change4fv(struct gfx_material * material, enum GFX_MATERIAL type, float * colours)
{
  if(type & GFX_MATERIAL_COLOUR)
    for(int i = 0; i < 4; i++)
      material->colour[i] = colours[i];
  
  material->changed |= type;
}

#endif
