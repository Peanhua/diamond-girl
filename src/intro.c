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

#include "diamond_girl.h"
#include "td_object.h"
#include "globals.h"
#include "gfx.h"
#include "random.h"
#include "sfx.h"

#include <stdbool.h>
#include <SDL/SDL_framerate.h>
#include <assert.h>
#include <SDL/SDL.h>

#ifdef WITH_OPENGL
static struct td_object * logo;

struct anim
{
  struct anim * next;
  float camera_center_start[3];
  float camera_center_end[3];
  float camera_eye_start[3];
  float camera_eye_end[3];
  float camera_up_start[3];
  float camera_up_end[3];
  float fog_density_start, fog_density_end;
  enum SFX sfx;
  unsigned int duration;
  unsigned int current_frame;
};

static struct anim anim_end_vanish =
  {
    NULL,

    { 0, 0, 0 },
    { 0, 0, 0 },

    { 0, 0, 6 },
    { 0, 0, 0 },
    
    { 0, 1, 0 },
    { 0, 1, 0 },

    0.0f, 0.0f,

    SFX_SIZEOF_,
    40,
    0
  };

static struct anim anim_end3 =
  {
    &anim_end_vanish,

    { 0, 0, 0 },
    { 0, 0, 0 },

    { 0, 0, 8 },
    { 0, 0, 6 },
    
    { 0, 1, 0 },
    { 0, 1, 0 },

    0.0f, 0.0f,

    SFX_SIZEOF_,
    30,
    0
  };

static struct anim anim_end2 =
  {
    &anim_end3,

    { 0, 0, 0 },
    { 0, 0, 0 },

    { 0, 0, 9 },
    { 0, 0, 8 },
    
    { 0, 1, 0 },
    { 0, 1, 0 },

    0.0f, 0.0f,

    SFX_INTRO,
    30,
    0
  };

static struct anim anim_end =
  {
    &anim_end2,

    { 0, 0, 0 },
    { 0, 0, 0 },

    { 0, 0, 11 },
    { 0, 0,  9 },
    
    { 0, 1, 0 },
    { 0, 1, 0 },

    0.0f, 0.0f,

    SFX_SIZEOF_,
    60 * 3,
    0
  };

static struct anim anim_2 =
  {
    &anim_end,

    { 10, 0, 0 },
    {  0, 0, 0 },

    {  3, 0, 0.5 },
    {  0, 0, 11 },

    {  0,  1, 0 },
    {  0,  1, 0 },

    0.0f, 0.0f,

    SFX_SIZEOF_,
    30,
    0
  };

static struct anim anim_3 = 
  {
    &anim_2,
    {   6,    0, 0 },
    {  10,    0, 0 },

    { -12,   0, 0.5 },
    {   3,   0, 0.5 },

    {  0,  0, 1 },
    {  0,  1, 0 },

    0.0f, 0.0f,

    SFX_SIZEOF_,
    60 * 5 / 2 / 2,
    0
  };

static struct anim anim_4 = 
  {
    &anim_3,
    {   2,    0, 0 },
    {   6,    0, 0 },

    { -22,   0, 0.5 },
    { -12,   0, 0.5 },

    {  0, 0, 1 },
    {  0, 0, 1 },

    0.1f, 0.0f,

    SFX_SIZEOF_,
    60 * 5 / 2 / 2,
    0
  };

static struct anim anim_start = 
  {
    &anim_4,
    {  -6,    0, 0 },
    {   2,    0, 0 },

    { -47,   0, 0.5 },
    { -22,   0, 0.5 },

    {  0, 0, 1 },
    {  0, 0, 1 },

    0.2f, 0.1f,

    SFX_SIZEOF_,
    60 * 5 / 2,
    0
  };

static void draw_frame(struct anim * anim)
{
  if(anim != NULL)
    {
      glClear(GL_COLOR_BUFFER_BIT);
      gfx_3d_depthtest(true);

      glLoadIdentity();

      float perc, dif;
      float camera_eye[3], camera_center[3], camera_up[3];

      perc = (float) anim->current_frame / (float) anim->duration;
      for(int i = 0; i < 3; i++)
        {
          dif = anim->camera_eye_end[i] - anim->camera_eye_start[i];
          camera_eye[i] = anim->camera_eye_start[i] + dif * perc;

          dif = anim->camera_center_end[i] - anim->camera_center_start[i];
          camera_center[i] = anim->camera_center_start[i] + dif * perc;

          dif = anim->camera_up_end[i] - anim->camera_up_start[i];
          camera_up[i] = anim->camera_up_start[i] + dif * perc;
        }

      if(anim->fog_density_start > 0.0f || anim->fog_density_end > 0.0f)
        {
          float fog_density;
          
          dif = anim->fog_density_end - anim->fog_density_start;
          fog_density = anim->fog_density_start + dif * perc;
          gfxgl_fog(GL_LINEAR, fog_density, 0.0f, 50.0f);
        }

      gluLookAt(camera_eye[0],    camera_eye[1],    camera_eye[2],
                camera_center[0], camera_center[1], camera_center[2],
                camera_up[0],     camera_up[1],     camera_up[2]);

      td_object_draw(logo);

      if(anim->fog_density_start > 0.0f || anim->fog_density_end > 0.0f)
        gfxgl_fog_off();
      
      gfx_flip();
    }
}

#endif


void intro(void)
{
  sfx_volume(0.3f);
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      bool done;
      FPSmanager framerate_manager;

      sfx_set_listener_position(0, 0);
      
      GFX_GL_ERROR();
      gfx_3d(true);
      GFX_GL_ERROR();

      SDL_initFramerate(&framerate_manager);
      SDL_setFramerate(&framerate_manager, 60);

      logo = td_object_load(get_data_filename("gfx/logo.obj"), NULL, NULL);
      assert(logo != NULL);
      GFX_GL_ERROR();

      {
        GLfloat light_a[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light_d[] = { 0.75f, 0.75f, 0.75f, 1.0f };
        GLfloat light_s[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat pos[] = { -0.15f, 1.0f, 1.0f, 0.0f };
        gfxgl_light(light_a, light_d, light_s, pos);
      }

      struct anim * anim;

      done = false;
      if(get_rand(100) < 50)
        anim = &anim_start;
      else
        anim = &anim_end;

      if(anim->sfx != SFX_SIZEOF_)
        sfx_emit(anim->sfx, 0, 0);
      
      while(done == false)
        {
          draw_frame(anim);
          SDL_framerateDelay(&framerate_manager);

          {
            SDL_Event event;

            while(SDL_PollEvent(&event))
              {
                switch(event.type)
                  {
                  case SDL_QUIT:
                  case SDL_KEYDOWN:
                  case SDL_MOUSEBUTTONDOWN:
                  case SDL_JOYBUTTONDOWN:
                    done = true;
                    break;
                  }
              }
          }

          if(anim != NULL)
            {
              anim->current_frame++;
              if(anim->current_frame >= anim->duration)
                {
                  anim = anim->next;
                  if(anim != NULL)
                    {
                      if(anim->sfx != SFX_SIZEOF_)
                        sfx_emit(anim->sfx, 0, 0);
                      anim->current_frame = 0;
                    }
                  else
                    done = true;
                }
            }
        }

      gfx_2d();

      logo = td_object_unload(logo);

      gfxgl_light_off();
#endif
    }
}
