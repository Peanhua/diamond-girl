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

#include "gfx.h"
#include "globals.h"
#include <stdbool.h>
#include <assert.h>

#ifdef WITH_OPENGL
static bool    projection_matrix_initialized = false;
static GLfloat projection_matrix[16];

void gfx_3d(bool reset_view)
{
  if(globals.opengl)
    {
      if(projection_matrix_initialized == false)
        {
          GFX_GL_ERROR();
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          gluPerspective(60.0, (double) SCREEN_WIDTH / (double) SCREEN_HEIGHT, 0.1, 100.0);
          glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
          glMatrixMode(GL_MODELVIEW);
          GFX_GL_ERROR();
          projection_matrix_initialized = true;
        }

#ifdef GL_EXT_direct_state_access
      if(GLEW_EXT_direct_state_access)
        {
          glMatrixLoadfEXT(GL_PROJECTION, projection_matrix);
        }
      else
#endif
        {
          glMatrixMode(GL_PROJECTION);
          glLoadMatrixf(projection_matrix);
          glMatrixMode(GL_MODELVIEW);
        }

      glLoadIdentity();
      GFX_GL_ERROR();

      gfx_3d_depthtest(reset_view);

      glCullFace(GL_BACK);
      GFX_GL_ERROR();
    }
}


void gfx_3d_depthtest(bool reset_view)
{
  static bool depthless = true;

  gfxgl_state(GL_DEPTH_TEST, true);
  GFX_GL_ERROR();
  if(reset_view == true)
    {
      if(depthless)
        {
          glDepthFunc(GL_LESS);
          glDepthRange(0.0, 0.5);
          //glClearColor(0.0, 0.0, 0.0, 0.0);
          depthless = false;
        }
      else
        {
          glDepthFunc(GL_GREATER);
          glDepthRange(1.0, 0.5);
          //glClearColor(0.0, 0.0, 0.0, 1.0);
          depthless = true;
        }
    }
  GFX_GL_ERROR();
}

#else

void gfx_3d(bool reset_view DG_UNUSED)
{
}

void gfx_3d_depthtest(bool reset_view DG_UNUSED)
{
}

#endif
