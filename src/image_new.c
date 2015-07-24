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

#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

struct image * image_new(int width, int height, bool alpha)
{
  struct image * rv;

  rv = malloc(sizeof(struct image));
  if(rv != NULL)
    {
      int bpp;

      rv->width               = width;
      rv->height              = height;
      rv->content_width       = width;
      rv->content_height      = height;
      rv->sdl_surface         = NULL;
      rv->flags               = 0;
#ifdef WITH_OPENGL
      rv->texture             = 0;
      rv->texture_initialized = false;
      rv->texture_atlas       = NULL;
      rv->texture_offset_x    = 0.0f;
      rv->texture_offset_y    = 0.0f;
      rv->buffer              = NULL;
      rv->bufferw             = 0;
      rv->bufferh             = 0;
#endif

      if(alpha)
        {
          bpp = 4;
          rv->flags |= IF_ALPHA;
        }
      else
	bpp = 3;

      rv->data = malloc(rv->width * rv->height * bpp);
      if(rv->data != NULL)
	{
#ifndef NDEBUG
          /* Valgrind complains. Possibly false alarm, ignored now.
           * But could be OpenGL texture drawing over it's limits: could cause gfx artifacts near image borders.
           */
          memset(rv->data, 0, rv->width * rv->height * bpp);
#endif
	}
      else
	{
	  fprintf(stderr, "Failed to allocate memory for image: %s\n", strerror(errno));
	  free(rv);
	  rv = NULL;
	}
    }
  else
    {
      fprintf(stderr, "Failed to allocate memory for image: %s\n", strerror(errno));
    }

  assert(rv != NULL);

  return rv;
}
