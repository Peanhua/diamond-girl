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

#ifndef GFX_SHADERS_H_
#define GFX_SHADERS_H_

# ifdef WITH_OPENGL
#  include <GL/glew.h>

extern void gfx_shaders_initialize(void);
extern void gfx_shaders_cleanup(void);

extern void gfx_shaders_set_fog(GLint mode, bool enabled);
extern void gfx_shaders_set_lighting(bool enabled);
extern void gfx_shaders_set_texture(bool enabled);
extern void gfx_shaders_apply(void);

# endif
#endif
