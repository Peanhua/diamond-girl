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

#include "gfx.h"
#include <assert.h>

static bool states[5];

void gfxgl_client_state(GLenum cap, bool enabled)
{
  assert(cap == 0                      ||
         cap == GL_VERTEX_ARRAY        ||
         cap == GL_NORMAL_ARRAY        ||
         cap == GL_TEXTURE_COORD_ARRAY ||
         cap == GL_COLOR_ARRAY);

  unsigned int i;

  switch(cap)
    {
    case GL_COLOR_ARRAY:         i = 1; break;
    case GL_NORMAL_ARRAY:        i = 2; break;
    case GL_TEXTURE_COORD_ARRAY: i = 3; break;
    case GL_VERTEX_ARRAY:        i = 4; break;

    case 0: /* Initialization */
      for(i = 1; i <= 4; i++)
        states[i] = enabled;
      i = 0;
      break;

    default:
      assert(false);
      i = 0;
      break;
    }

  if(i > 0)
    if(states[i] != enabled)
      {
        states[i] = enabled;
        
        if(enabled)
          glEnableClientState(cap);
        else
          glDisableClientState(cap);
        
        GFX_GL_ERROR();
      }
}

#endif
