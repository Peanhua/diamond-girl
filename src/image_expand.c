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
#include <assert.h>
#include <string.h>
#include <errno.h>

bool image_expand(struct image * image, int new_width, int new_height)
{
  bool rv;
  uint8_t * new;
  int bpp;

  assert(image != NULL);
  assert(image->width <= new_width);
  assert(image->height <= new_height);

  rv = false;
  bpp = (image->flags & IF_ALPHA) ? 4 : 3;

  new = malloc(new_width * new_height * bpp);
  assert(new != NULL);
  if(new != NULL)
    {
      size_t slinelen, dlinelen;
      
      slinelen = image->width * bpp;
      dlinelen = new_width    * bpp;

      for(int y = 0; y < image->height; y++)
        memcpy(new + y * dlinelen, image->data + y * slinelen, slinelen);
      
      image->content_width  = image->width;
      image->content_height = image->height;
      image->width  = new_width;
      image->height = new_height;
      
      /* Replace the image with the new one. */
      free(image->data);
      image->data = new;

      rv = true;
    }
  else
    fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));

  return rv;
}
