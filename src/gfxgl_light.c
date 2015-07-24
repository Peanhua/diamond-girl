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
#include "gfx_shaders.h"
#include "globals.h"
#include <assert.h>


static uint8_t lights = 0;


void gfxgl_light(GLfloat const * ambient, GLfloat const * diffuse, GLfloat const * specular, GLfloat const * position)
{
  assert(ambient != NULL);
  assert(diffuse != NULL);
  assert(specular != NULL);
  assert(position != NULL);

  GLenum light_id;

  if(position[3] > 0.1f)
    {
      lights   |= (1<<GFX_LIGHT_TYPE_POSITIONAL);
      light_id  = GL_LIGHT1;
    }
  else
    {
      lights   |= (1<<GFX_LIGHT_TYPE_DIRECTIONAL);
      light_id  = GL_LIGHT0;
    }
  
  GFX_GL_ERROR();
  glLightfv(light_id, GL_AMBIENT,  ambient);
  glLightfv(light_id, GL_DIFFUSE,  diffuse);
  glLightfv(light_id, GL_SPECULAR, specular);
  glLightfv(light_id, GL_POSITION, position);
  GFX_GL_ERROR();

  if(globals.opengl_shaders == 0)
    {
      gfxgl_state(light_id, true);
      gfxgl_state(GL_LIGHTING, true);
    }
  else
    gfx_shaders_set_lighting(true);
}

void gfxgl_light_off(void)
{
  if(globals.opengl_shaders == 0)
    {
      gfxgl_state(GL_LIGHT0, false);
      gfxgl_state(GL_LIGHT1, false);
      gfxgl_state(GL_LIGHTING, false);
    }
  else
    gfx_shaders_set_lighting(false);

  lights = 0;
}


uint8_t gfxgl_light_get(void)
{
  return lights;
}

#endif
