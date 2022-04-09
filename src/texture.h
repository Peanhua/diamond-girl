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

#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef WITH_OPENGL

#include <GL/glew.h>
#include <stdbool.h>

struct image;

extern struct image * texture_load(const char * filename, bool generate_alpha, bool use_mipmapping, bool linear);
extern void           texture_setup_from_image(struct image * image, bool generate_alpha, bool use_mipmapping, bool linear);

#endif

#endif
