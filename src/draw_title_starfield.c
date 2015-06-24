/*
  Diamond Girl - Game where player collects diamonds.
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

#include "title.h"
#include "gfx.h"
#include "globals.h"
#include "random.h"

#include <stdbool.h>


#define STAR_INITIAL_Y 15
#define STARS_PER_PIXEL 20
#define STARS (STAR_INITIAL_Y + 1) * STARS_PER_PIXEL

#ifdef WITH_OPENGL
static float   fade_in;
static GLfloat stars[STARS * 2*3];
static GLuint  vbo;


static void new_star(int ind, float y)
{
  stars[ind * 2*3 + 0] =    stars[ind * 2*3 + 3] = ((float) get_rand(1000) / 1000.0f - 0.5f) * 3.0f;
  stars[ind * 2*3 + 1] = y; stars[ind * 2*3 + 4] = y - 0.1f;
  stars[ind * 2*3 + 2] =    stars[ind * 2*3 + 5] = ((float) get_rand(1000) / 1000.0f - 0.5f) * 3.0f;
}
#endif

void draw_title_starfield_initialize(void)
{
#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      float y;

      y = STAR_INITIAL_Y;
      for(int i = 0; i < STARS; i++, y += 1.0f / (float) STARS_PER_PIXEL)
        new_star(i, y);

      fade_in = 0.0f;

      vbo = 0;
      if(globals.opengl_1_5)
        {
          glGenBuffers(1, &vbo);
          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferData(GL_ARRAY_BUFFER, sizeof stars, stars, GL_STREAM_DRAW);
        }
      else if(GLEW_ARB_vertex_buffer_object)
        {
          glGenBuffersARB(1, &vbo);
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
          glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof stars, stars, GL_STREAM_DRAW_ARB);
        }
    }
#endif
}

void draw_title_starfield(void)
{
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      fade_in += 0.0005f;
      if(fade_in > 0.5f)
        fade_in = 0.5f;

      glPushMatrix();
      glLoadIdentity();

      float camera_eye[3]    = { 0, 0, 0 };
      float camera_center[3] = { 0, 1, 0 };
      float camera_up[3]     = { 0, 0, 1 };

      gluLookAt(camera_eye[0],    camera_eye[1],    camera_eye[2],
                camera_center[0], camera_center[1], camera_center[2],
                camera_up[0],     camera_up[1],     camera_up[2]);

      gfx_colour(255.0f * 0.25 * fade_in, 255.0f * 0.5f * fade_in, 255.0f * 0.5f * fade_in, 0xff);


      for(int i = 0; i < STARS; i++)
        {
          stars[i * 2*3 + 1] -= 0.1f;
          stars[i * 2*3 + 4] -= 0.1f;
          if(stars[i * 2*3 + 1] < -1.0f)
            new_star(i, STAR_INITIAL_Y);
        }

      if(globals.opengl_1_5)
        {
          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof stars, stars);
          glVertexPointer(3, GL_FLOAT, 0, NULL);
        }
      else if(GLEW_ARB_vertex_buffer_object)
        {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
          glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof stars, stars);
          glVertexPointer(3, GL_FLOAT, 0, NULL);
        }
      else
        {
          glVertexPointer(3, GL_FLOAT, 0, stars);
        }

      glDrawArrays(GL_LINES, 0, STARS * 2);

      glPopMatrix();
#endif
    }
}
