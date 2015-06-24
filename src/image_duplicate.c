/*
  Diamond Girl - Game where player collects diamonds.
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

struct image * image_duplicate(struct image * image)
{
  struct image * new;
  bool alpha;

  alpha = (image->flags & IF_ALPHA) ? true : false;
  
  new = image_new(image->width, image->height, alpha);
  assert(new != NULL);
  if(new != NULL)
    {
      int bpp;

      bpp = alpha == true ? 4 : 3;
      memcpy(new->data, image->data, bpp * new->width * new->height * sizeof(uint8_t));
    }

  return new;
}

