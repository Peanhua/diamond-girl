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
#include <stdarg.h>
#ifdef WITH_OPENGL
#include <GL/glew.h>
#endif

#ifndef NDEBUG
void gfx_gl_log(char * fmt, ...)
{
  if(GLEW_GREMEDY_string_marker)
    {
      va_list ap;
      char buf[1024];
      
      va_start(ap, fmt);
      vsnprintf(buf, sizeof buf, fmt, ap);
      va_end(ap);
      glStringMarkerGREMEDY(0, buf);
    }
}
#endif

#ifndef NDEBUG
# ifdef WITH_OPENGL

#include "globals.h"
#include <assert.h>

void gfx_gl_error(char * fmt, ...)
{
  if(globals.opengl)
    {
      GLenum err;
  
      err = glGetError();
      if(err != GL_NO_ERROR)
        {
          va_list ap;
          char buf[1024];
      
          va_start(ap, fmt);
          vsnprintf(buf, sizeof buf, fmt, ap);
          va_end(ap);
          fprintf(stderr, "OpenGL error %d: %s\n%s\n", (int) err, gluErrorString(err), buf);
        }
      assert(err == GL_NO_ERROR);
    }
}
# endif
#endif
