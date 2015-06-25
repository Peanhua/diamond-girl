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
#include "image.h"

#include <assert.h>
#include <sys/time.h>

static bool           fps_initialized = false;
static int            fps_counter;
static struct timeval fps_time;


void gfx_flip()
{
  if(globals.opengl)
    {
#ifdef WITH_OPENGL

#ifndef NDEBUG
      GLenum err;

      err = glGetError();
      if(err != GL_NO_ERROR)
        fprintf(stderr, "%s(): OpenGL error %d: %s\n", __FUNCTION__, (int) err, gluErrorString(err));
      assert(err == GL_NO_ERROR);
#endif

      if(globals.screen_capture_path != NULL)
        {
          static long int image_index = 0;
          static int skipcounter = 0;

          skipcounter++;
          if(skipcounter == globals.screen_capture_frameskip)
            {
              struct image image;

              skipcounter = 0;

              image.data   = gfx_capture_screen();
              assert(image.data != NULL);
              image.width  = SCREEN_WIDTH;
              image.height = SCREEN_HEIGHT;
              image.content_width  = image.width;
              image.content_height = image.height;
              image.sdl_surface = NULL;
              image.flags = 0;
#ifdef WITH_OPENGL
              image.texture = 0;
              image.texture_initialized = false;
              image.buffer = NULL;
              image.bufferw = 0;
              image.bufferh = 0;
#endif

              char fn[1024];

              snprintf(fn, sizeof fn, "%s/diamond-girl-capture-%ld.png", globals.screen_capture_path, image_index++);
              image_save(&image, fn);
              
              free(image.data);
            }
        }

      SDL_GL_SwapBuffers();
#endif
    }
#ifdef WITH_NONOPENGL
  else
    SDL_Flip(gfx_game_screen());
#endif


  if(globals.fps_counter_active == true)
    {
      if(fps_initialized == false)
        {
          fps_initialized = true;
          fps_counter = 0;
          gettimeofday(&fps_time, NULL);
        }
      fps_counter++;

      if(fps_counter >= 1000)
        {
          struct timeval t;
          double x;

          gettimeofday(&t, NULL);
          x = t.tv_sec - fps_time.tv_sec + (t.tv_usec - fps_time.tv_usec) / 1000000.0;
          printf("fps: 1000 frames in %.3f seconds => %.3f frames/second\n", x, 1000.0 / x);

          fps_time.tv_sec  = t.tv_sec;
          fps_time.tv_usec = t.tv_usec;
          fps_counter = 0;
        }
    }
}
