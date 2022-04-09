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

#ifdef WITH_OPENGL

#include "gfx.h"
#include "gfx_shaders.h"
#include "globals.h"


void gfxgl_fog(GLint mode, GLfloat density, GLint start, GLint end)
{
  glFogf(GL_FOG_DENSITY, density);
  glFogf(GL_FOG_START, start);
  glFogf(GL_FOG_END, end);
  GFX_GL_ERROR();

  if(globals.opengl_shaders == 0)
    {
      glFogf(GL_FOG_MODE, mode);
      gfxgl_state(GL_FOG, true);
    }
  else
    gfx_shaders_set_fog(mode, true);
}



void gfxgl_fog_off(void)
{
  if(globals.opengl_shaders == 0)
    gfxgl_state(GL_FOG, false);
  else
    gfx_shaders_set_fog(0, false);
}

#endif
