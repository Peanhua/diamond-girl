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

#include "texture.h"
#include "image.h"
#include "globals.h"
#include "gfx.h"
#include <GL/glu.h>
#include <assert.h>

void texture_setup_from_image(struct image * image, bool generate_alpha, bool use_mipmapping, bool linear)
{
  GLint internalformat;
  GLenum format;

  image->texture_wanted = true;
  
  glGenTextures(1, &image->texture);
  GFX_GL_ERROR();
  image->texture_initialized = true;

  if(image->flags & IF_ALPHA)
    assert((image->width * 4) % 4 == 0);
  else
    assert((image->width * 3) % 4 == 0);
  assert(image->width <= globals.opengl_max_texture_size);
  assert(image->height <= globals.opengl_max_texture_size);

  gfxgl_bind_texture(image->texture);
  if(generate_alpha)
    {
      if(globals.opengl_compressed_textures == true && GLEW_ARB_texture_compression)
        internalformat = GL_COMPRESSED_RGBA_ARB;
      else
        internalformat = GL_RGBA8;
      format = GL_RGBA;
    }
  else
    {
      if(globals.opengl_compressed_textures == true && GLEW_ARB_texture_compression)
        internalformat = GL_COMPRESSED_RGB_ARB;
      else
        internalformat = GL_RGB8;
      format = GL_RGB;
    }
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  GFX_GL_ERROR();
  
  if(linear == true)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if(use_mipmapping)
    {
      if(linear == true)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      GFX_GL_ERROR();
#ifdef GL_VERSION_3_0
      if(GLEW_VERSION_3_0)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->data);
          glGenerateMipmap(GL_TEXTURE_2D);
        }
      else
#endif
        if(globals.opengl_1_4)
          {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->data);
            GFX_GL_ERROR();
          }
        else
          gluBuild2DMipmaps(GL_TEXTURE_2D, internalformat, image->width, image->height, format, GL_UNSIGNED_BYTE, image->data);
      GFX_GL_ERROR();
    }
  else
    {
      if(linear == true)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      GFX_GL_ERROR();
      glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->data);
      GFX_GL_ERROR();
    }
}

#endif
