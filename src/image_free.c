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

#include "gc.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "image.h"

struct image * image_free(struct image * image)
{
  if(image != NULL)
    {
      gc_free(GCT_IMAGE, image);
      
      if(image->sdl_surface != NULL)
        SDL_FreeSurface(image->sdl_surface);
      if(image->data != NULL)
	free(image->data);
#ifdef WITH_OPENGL
      image_free_texture(image);
      if(image->buffer != NULL)
        image->buffer = gfxbuf_free(image->buffer);
#endif
#ifndef NDEBUG
      free(image->backtrace);
#endif
      free(image);
    }
  return NULL;
}

#ifdef WITH_OPENGL
void image_free_texture(struct image * image)
{
  if(image != NULL)
    if(image->texture_initialized == true)
      {
        gfxgl_unbind_texture_if_in_use(image->texture);
        glDeleteTextures(1, &image->texture);
        GFX_GL_ERROR();
        image->texture             = 0;
        image->texture_initialized = false;
      }
}
#endif
