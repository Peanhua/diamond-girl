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

#include "image.h"
#include "texture.h"
#include "globals.h"
#include "gfxbuf.h"

#include <assert.h>

void image_to_texture(struct image * image, bool generate_alpha, bool use_mipmapping, bool linear)
{
  assert(globals.opengl);
  assert(image->texture_initialized == false);
  assert(((image->flags & IF_ALPHA) == 0 && generate_alpha == false) || ((image->flags & IF_ALPHA) != 0 && generate_alpha == true));

  texture_setup_from_image(image, generate_alpha, use_mipmapping, linear);

  if(image->buffer == NULL)
    {
      uint8_t bufopts;

      bufopts = GFXBUF_TEXTURE;
      if(generate_alpha == true)
        bufopts |= GFXBUF_BLENDING;
      image->buffer = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, bufopts);
      assert(image->buffer != NULL);
      if(image->buffer != NULL)
        gfxbuf_resize(image->buffer, 4);
    }
}

#endif
