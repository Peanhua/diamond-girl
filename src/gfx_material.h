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

#ifndef GFX_MATERIAL_H_
#define GFX_MATERIAL_H_

#include <inttypes.h>
#include <stdbool.h>

enum GFX_MATERIAL
  {
    GFX_MATERIAL_COLOUR    = (1<<0),
    GFX_MATERIAL_AMBIENT   = (1<<1),
    GFX_MATERIAL_DIFFUSE   = (1<<2),
    GFX_MATERIAL_SPECULAR  = (1<<3),
    GFX_MATERIAL_EMISSION  = (1<<4),
    GFX_MATERIAL_SHININESS = (1<<5)
  };

struct gfx_material
{
  float   colour[4];
  
  float   ambient[4];
  float   diffuse[4];
  float   specular[4];
  float   emission[4];
  uint8_t shininess;
  
  uint8_t changed; /* Bitmask of changed attributes. */
  
  uint8_t use_count; /* For delete/free -stuff. */
};

extern struct gfx_material * gfx_material_new(void);
extern struct gfx_material * gfx_material_delete(struct gfx_material * material);
extern void                  gfx_material_free_(struct gfx_material * material);

extern struct gfx_material * gfx_material_default(void);
extern void                  gfx_material_copy(struct gfx_material * destination, struct gfx_material const * source);
extern void                  gfx_material_change4f(struct gfx_material * material, enum GFX_MATERIAL type, float red, float green, float blue, float alpha);
extern void                  gfx_material_change4fv(struct gfx_material * material, enum GFX_MATERIAL type, float * colours);

extern void gfx_material_use_initialize(void);
extern void gfx_material_use(struct gfx_material * material);
extern void gfx_material_use_invalidate_colour(void);

#endif
