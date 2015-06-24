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

#include "gfx.h"
#include "globals.h"

#include <string.h>
#include <errno.h>
#include <assert.h>

uint8_t * gfx_capture_screen(void)
{
  uint8_t * img;
  
  img = NULL;

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      glFlush();
      img = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 3);
      assert(img != NULL);
      if(img != NULL)
        glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, img);
      else
        fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
#endif
    }

  return img;
}

              
