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

static bool states[11];

void gfxgl_state(GLenum cap, bool enabled)
{
  assert(cap == 0                 ||
         cap == GL_BLEND          ||
         cap == GL_COLOR_MATERIAL ||
         cap == GL_CULL_FACE      ||
         cap == GL_DEPTH_TEST     ||
         cap == GL_DITHER         ||
         cap == GL_FOG            ||
         cap == GL_LIGHT0         ||
         cap == GL_LIGHTING       ||
         cap == GL_SCISSOR_TEST   ||
         cap == GL_TEXTURE_2D);

  unsigned int i;

  switch(cap)
    {
    case GL_BLEND:          i =  1; break;
    case GL_COLOR_MATERIAL: i =  2; break;
    case GL_CULL_FACE:      i =  3; break;
    case GL_DEPTH_TEST:     i =  4; break;
    case GL_DITHER:         i =  5; break;
    case GL_FOG:            i =  6; break;
    case GL_LIGHT0:         i =  7; break;
    case GL_LIGHTING:       i =  8; break;
    case GL_SCISSOR_TEST:   i =  9; break;
    case GL_TEXTURE_2D:     i = 10; break;

    case 0: /* Initialization */
      gfxgl_state(GL_BLEND,          enabled);
      gfxgl_state(GL_COLOR_MATERIAL, enabled);
      gfxgl_state(GL_CULL_FACE,      enabled);
      gfxgl_state(GL_DEPTH_TEST,     enabled);
      gfxgl_state(GL_DITHER,         enabled);
      gfxgl_state(GL_FOG,            enabled);
      gfxgl_state(GL_LIGHT0,         enabled);
      gfxgl_state(GL_LIGHTING,       enabled);
      gfxgl_state(GL_SCISSOR_TEST,   enabled);
      gfxgl_state(GL_TEXTURE_2D,     enabled);
      i = 0;
      break;

    default:
      assert(false);
      i = 0;
      break;
    }

  if(i > 0)
    {
      if(states[i] != enabled)
        {
          states[i] = enabled;

          if(enabled)
            glEnable(cap);
          else
            glDisable(cap);

          GFX_GL_ERROR();
        }
    }
}

#endif
