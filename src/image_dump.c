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

#ifndef NDEBUG

#include "image.h"

void image_dump(struct image const * image, unsigned int indentation)
{
  printf("%*sDumping image: %p\n", indentation, "", image);
  if(image != NULL)
    {
      printf("%*s size          = %d x %d\n", indentation, "", image->width, image->height);
      printf("%*s content size  = %d x %d\n", indentation, "", image->content_width, image->content_height);
      printf("%*s flags         =", indentation, "");
      if(image->flags & IF_ALPHA)
        printf(" alpha");
      printf("\n");
#ifdef WITH_OPENGL
      printf("%*s texture init  = %s\n", indentation, "", image->texture_initialized == true ? "true" : "false");
      printf("%*s texture want  = %s\n", indentation, "", image->texture_wanted      == true ? "true" : "false");
      printf("%*s texture atlas = %p\n", indentation, "", image->texture_atlas);
      if(image->texture_atlas != NULL)
        image_dump(image->texture_atlas, indentation + 2);
#endif
    }
}
#endif
