/*
  Lucy the Diamond Girl - Game where player collects diamonds.
  Copyright (C) 2005-2022  Joni Yrjänä <joniyrjana@gmail.com>
  
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

#include "gfx_material.h"
#include "gfxbuf.h"
#include "globals.h"
#include "image.h"
#include "texture.h"
#include <assert.h>

void image_to_texture(struct image * image, bool use_mipmapping, bool linear)
{
  assert(globals.opengl);
  assert(image->texture_initialized == false);

  bool generate_alpha;

  if(image->flags & IF_ALPHA)
    generate_alpha = true;
  else
    generate_alpha = false;
  
  if(use_mipmapping == true)
    image->flags |= IF_MIPMAPPING;
  else
    image->flags &= ~IF_MIPMAPPING;

  if(linear == true)
    image->flags |= IF_LINEAR;
  else
    image->flags &= ~IF_LINEAR;
  
  texture_setup_from_image(image, generate_alpha, use_mipmapping, linear);

  image_setup_buffer(image, generate_alpha);
}

bool image_setup_buffer(struct image * image, bool alpha)
{
  bool success;

  success = false;
  if(image->buffer == NULL)
    {
      uint8_t bufopts;

      bufopts = GFXBUF_TEXTURE;
      if(alpha == true)
        bufopts |= GFXBUF_BLENDING;
      image->buffer = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, bufopts);
      assert(image->buffer != NULL);
      if(image->buffer != NULL)
        {
          gfxbuf_resize(image->buffer, 4);
          image->buffer->material = gfx_material_default();
          success = true;
        }
    }

  return success;
}

#endif
