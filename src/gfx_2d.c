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

#include "gfx.h"
#include "globals.h"

void gfx_2d(void)
{
#ifdef WITH_OPENGL
  if(globals.opengl)
    {
#ifdef GL_EXT_direct_state_access
      if(GLEW_EXT_direct_state_access)
        {
          glMatrixLoadIdentityEXT(GL_PROJECTION);
          glMatrixOrthoEXT(GL_PROJECTION, 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1, 1);
        }
      else
#endif
        {
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1, 1);

          glMatrixMode(GL_MODELVIEW);
          GFX_GL_ERROR();
        }
      
      glLoadIdentity();

      gfxgl_state(GL_DEPTH_TEST, false);
      glNormal3f(0.0f, 0.0f, 1.0f);
      GFX_GL_ERROR();
      glCullFace(GL_FRONT);
      GFX_GL_ERROR();

      glTranslatef(0.375f, 0.375f, 0.0f);
    }
#endif
}
