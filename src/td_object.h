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

#ifndef TD_OBJECT_H_
#define TD_OBJECT_H_

#ifdef WITH_OPENGL

#include <GL/glew.h>

struct gfxbuf;
struct stack;

/* There is support only for a single texture per mesh */
struct td_object_mesh
{
  GLfloat         matrix[4 * 4];
  struct gfxbuf * gfxbuf;
  struct image *  texture_image;
};

struct td_object
{
#ifndef NDEBUG
  char *         filename;
#endif
  struct stack * meshes;
};


extern struct td_object * td_object_load(char const * const filename);
extern struct td_object * td_object_unload(struct td_object * td_object);
extern void               td_object_draw(struct td_object * td_object);

#endif

#endif
