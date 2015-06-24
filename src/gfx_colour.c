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

#ifdef WITH_OPENGL

#include "gfx.h"
#include <GL/glew.h>

static GLubyte current_colour[4];

void gfx_colour_clear(void)
{
  for(int i = 0; i < 4; i++)
    current_colour[i] = 0x00;
  gfx_colour_white();
}


void gfx_colour_white(void)
{
  gfx_colour(0xff, 0xff, 0xff, 0xff);
}


void gfx_colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
  if(current_colour[0] != red   ||
     current_colour[1] != green ||
     current_colour[2] != blue  ||
     current_colour[3] != alpha    )
    {
      current_colour[0] = red;
      current_colour[1] = green;
      current_colour[2] = blue;
      current_colour[3] = alpha;
      
      glColor4ubv(current_colour);
    }
}

#endif
