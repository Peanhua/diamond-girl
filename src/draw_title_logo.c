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

#include "diamond_girl.h"
#include "font.h"
#include "title.h"
#include "gfx.h"
#include "gfx_image.h"
#include "td_object.h"
#include "sfx.h"
#include "globals.h"
#include "random.h"
#include "image.h"
#include "map.h"

#include <assert.h>
#include <SDL/SDL_image.h>
#include <math.h>
#ifdef WITH_OPENGL
#include <GL/glu.h>
#endif


static struct map * map = NULL;

#ifdef WITH_OPENGL
static struct td_object * logo;

/* The stable destination location and stuff for logo. */
static const float logo_center_pos[3] = {  0.0f, 4.0f, 6.8f };
static const float logo_center_sca[3] = {  1.0f, 1.0f, 1.0f };
static const float logo_center_rot    = 40.0f;
static const float light_center_pos[3] = { 0, 6, -12 };
/* The current state of logo. */
static float logo_pos[3];
static float logo_sca[3];
static float logo_rot;
static float light_offset[3];
static float light_pos[3];
static float light_brightness = 1.0f;
/* The function to call for initialization (initialization=true), and for each step (initialization=false). */
static void (*logo_phase_function)(bool initialization) = NULL;
static int   logo_phase_delay = 60;


static void logo_phase_rotateX(bool initialization);
static void logo_phase_stretch(bool initialization);
static void logo_phase_rotateright(bool initialization);
static void logo_phase_rotaterightstretch(bool initialization);
static void logo_phase_vibrate(bool initialization);
static void logo_phase_minimizeHeight(bool initialization);

static void logo_phase_from_intro(bool initialization);
static void logo_phase_zoom(bool initialization);
static void logo_phase_rollzoom(bool initialization);
static void logo_phase_slowroll(bool initialization);
static void logo_phase_topdown(bool initialization);

static void logolight_phase_left2right(bool initialization);
static void logolight_phase_strobo(bool initialization);

static void add_diamond(int x, int y);

static bool very_first_time = true;
#endif


void draw_title_logo_initialize(struct map * the_map)
{
  map = the_map;

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      void (*funcs[4])(bool) =
        {
          logo_phase_zoom,
          logo_phase_rollzoom,
          logo_phase_slowroll,
          logo_phase_topdown
        };

      for(int i = 0; i < 3; i++)
        {
          logo_pos[i] = logo_center_pos[i];
          logo_sca[i] = logo_center_sca[i];
          light_offset[i] = 0.0f;
          light_pos[i] = light_center_pos[i];
        }
      light_offset[0] = (float) get_rand(13) - 6.0f;
      logo_rot = logo_center_rot;

      if(very_first_time == true)
        {
          logo_phase_function = logo_phase_from_intro;
          very_first_time = false;
        }
      else
        logo_phase_function = funcs[get_rand(4)];

      logo_phase_function(true);

      logo_phase_delay = 60;

      logo = td_object_load(get_data_filename("gfx/logo.obj"), NULL, NULL);
      assert(logo != NULL);
#endif
    }
}

void draw_title_logo_cleanup(void)
{
  map = NULL;
#ifdef WITH_OPENGL
  if(logo != NULL)
    logo = td_object_unload(logo);
#endif
}

void draw_title_logo(void)
{
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      {
        GLfloat light_a[] = { light_brightness * 0.1f,  light_brightness * 0.1f,  light_brightness * 0.1f,  1.0f };
        GLfloat light_d[] = { light_brightness * 0.85f, light_brightness * 0.85f, light_brightness * 0.85f, 1.0f };
        GLfloat light_s[] = { light_brightness * 1.0f,  light_brightness * 1.0f,  light_brightness * 1.0f,  1.0f };
        GLfloat pos[] =
          {
            logo_center_pos[0] + light_pos[0] + light_offset[0],
            logo_center_pos[1] + light_pos[1] + light_offset[1],
            logo_center_pos[2] + light_pos[2] + light_offset[2],
            1.0f
          };

        gfxgl_light(light_a, light_d, light_s, pos);

        double camerac[3] = { 0, 0, 0 };
        double camerae[3] = { 0, -10, 0 };

        gluLookAt(camerae[0], camerae[1], camerae[2], camerac[0], camerac[1], camerac[2], 0, 0, 1);
      }

      {
        glPushMatrix();

        glTranslatef(logo_pos[0], logo_pos[1], logo_pos[2]);
        glRotatef(logo_rot + 70, 1.0, 0.0, 0.0);
        glScalef(logo_sca[0], logo_sca[1], logo_sca[2]);

        td_object_draw(logo);
            
        glPopMatrix();
            
        if(logo_phase_function == NULL)
          { /* Not active, run an animation while the player is idling. */
            logo_phase_delay--;
            if(logo_phase_delay == 0)
              {
                void (*funcs[8])(bool) =
                  {
                    logo_phase_rotateX,
                    logo_phase_stretch,
                    logo_phase_rotateright,
                    logo_phase_rotaterightstretch,
                    logo_phase_vibrate,
                    logo_phase_minimizeHeight,
                    logolight_phase_left2right,
                    logolight_phase_strobo
                  };
                    
                logo_phase_function = funcs[get_rand(8)];
                logo_phase_function(true);
              }
          }
        else
          { /* Active, do a logo step. */
            logo_phase_function(false);

            if(logo_phase_function == NULL)
              { /* The steps are done, cleanup. */
                for(int i = 0; i < 3; i++)
                  logo_pos[i] = logo_center_pos[i];
                logo_rot = logo_center_rot;
                for(int i = 0; i < 3; i++)
                  logo_sca[i] = logo_center_sca[i];

                /* Activate animation after delay frames */
                logo_phase_delay = 60 + get_rand(60 * 5);
              }
          }
      }

      gfxgl_light_off();
#endif
    }
  else
    {
#ifdef WITH_NONOPENGL
      struct image * logo_image;

      logo_image = gfx_image(GFX_IMAGE_LOGO);
      assert(logo_image != NULL);
      if(logo_image != NULL)
        {
          SDL_Rect r;

          r.x = (SCREEN_WIDTH - logo_image->width) / 2;
          r.y = (60 - logo_image->height) / 2;
          r.w = logo_image->width;
          r.h = logo_image->height;
          SDL_BlitSurface(logo_image->sdl_surface, NULL, gfx_game_screen(), &r);
        }
      else
        {
          char tmp[128];
          int y;

          y = 30 + font_height() / 2;
          snprintf(tmp, sizeof tmp, "DIAMOND GIRL v%s", VERSION);
          clear_area(0, y, SCREEN_WIDTH, font_height());
          font_write((SCREEN_WIDTH - font_width(tmp)) / 2, y, tmp);
        }
#endif
    }
}


#ifdef WITH_OPENGL

static void logo_phase_rotateX(bool initialization)
{
  static float progress;

  if(initialization == true)
    { /* Initialization */
      progress = 0.0;
    }
  else
    { /* Step - rotate slowly around X axis */
      progress += 0.05f;
      logo_rot = logo_center_rot + 360.0f * progress;
                          
      /* Done */
      if(progress >= 1.0f)
        logo_phase_function = NULL;
    }
}

static void logo_phase_stretch(bool initialization)
{
  static float progress;

  if(initialization == true)
    { /* Initialization */
      progress = 0.0;
    }
  else
    { /* Step - stretch in X axis */
      progress += 0.05f;
      logo_sca[0] = logo_center_sca[0] + 50.0f * (1.0f - progress);
                          
      /* Done */
      if(progress >= 1.0f)
        {
          int r;

          r = 3 + get_rand(7);
          for(int i = 0; i < r; i++)
            if(map != NULL)
              add_diamond(5 + get_rand(map->width - 10), 2);
          logo_phase_function = NULL;
        }
    }
}

static void logo_phase_rotateright(bool initialization)
{
  static float progress;

  if(initialization == true)
    { /* Initialization */
      progress = 0.0;
    }
  else
    { /* Step - rotate out to the right and then back in from the left */
      progress += 0.01f;
      if(progress <= 0.5f)
        logo_pos[0] = logo_center_pos[0] + progress * 40.0f;
      else
        logo_pos[0] = logo_center_pos[0] - (1.0f - progress) * 40.0f;
                          
      /* Done */
      if(progress >= 1.0f)
        {
          if(map != NULL)
            add_diamond(map->width - 5, 2);
          logo_phase_function = NULL;
        }
    }
}

static void logo_phase_rotaterightstretch(bool initialization)
{
  static float progress;

  if(initialization == true)
    { /* Initialization */
      progress = 0.0;
    }
  else
    { /* Step - rotate out to the right and then back in from the left and stretch for fluffy effect */
      progress += 0.005f;
      if(progress <= 0.5f)
        {
          logo_pos[0] = logo_center_pos[0] + progress * 80.0f;
          logo_sca[0] = logo_center_sca[0] + progress * 2.0f;
        }
      else
        {
          logo_pos[0] = logo_center_pos[0] - (1.0f - progress) * 120.0f;
          logo_sca[0] = logo_center_sca[0] + (1.0f - progress) * 10.0f;
        }
                          
      /* Done */
      if(progress >= 1.0f)
        {
          int r;

          r = 2 + get_rand(3);
          for(int i = 0; i < r; i++)
            if(map != NULL)
              add_diamond(map->width - 16 + get_rand(5), 2);

          logo_phase_function = NULL;
        }
    }
}

static void logo_phase_vibrate(bool initialization)
{
  static float countdown;

  if(initialization == true)
    { /* Initialization */
      countdown = 20 + get_rand(40);
    }
  else
    { /* Step - vibrate around */
      countdown--;

      for(int i = 0; i < 3; i++)
        logo_pos[i] = logo_center_pos[i] + ((double) get_rand(100) - 50.0f) / 1000.0f;
      if(map != NULL)
        add_diamond(5 + get_rand(map->width - 10), 2);

      /* Done */
      if(countdown == 0)
        logo_phase_function = NULL;
    }
}

static void logo_phase_minimizeHeight(bool initialization)
{
  static float progress;

  if(initialization == true)
    { /* Initialization */
      progress = 0.0f;
    }
  else
    { /* Step - scale in Y axis to almost disappear */
      progress += 0.01f;

      logo_sca[1] = logo_center_sca[1] * (1.0f - (progress * 4));
      if(logo_sca[1] < 0.3f)
        {
          logo_sca[1] = 0.3f;
          if(get_rand(100) < 30)
            if(map != NULL)
              add_diamond(5 + get_rand(map->width - 10), 2);
        }

      /* Done */
      if(progress >= 1.0f)
        logo_phase_function = NULL;
    }
}



static void logo_phase_from_intro(bool initialization)
{
  static float progress;

  if(initialization == true)
    { /* Initialization */
      progress = 0.0f;
    }
  else
    { /* Step - zoom in from distance */
      logo_pos[1] = logo_center_pos[1] + (1.0f - progress) * -10.0f;
      logo_pos[2] = logo_center_pos[2] + (1.0f - progress) * -5.0f;

      /* Done */
      progress += 0.06f;
      if(progress >= 1.0f)
        {
          logo_phase_function = NULL;
          sfx_emit(SFX_EXPLOSION, 0, 0);
        }
    }
}


static void logo_phase_zoom(bool initialization)
{
  if(initialization == true)
    { /* Initialization */
      logo_pos[1] = 100;
    }
  else
    { /* Step - zoom in from distance */
      logo_pos[1] -= 2.0f;

      /* Done */
      if(logo_pos[1] < logo_center_pos[1])
        {
          logo_phase_function = NULL;
          sfx_emit(SFX_EXPLOSION, 0, 0);
        }
    }
}

static void logo_phase_rollzoom(bool initialization)
{
  static float startpos;

  if(initialization == true)
    { /* Initialization */
      logo_phase_zoom(initialization);

      startpos = logo_pos[1];
      logo_rot = logo_center_rot;
    }
  else
    { /* Step - rotate N times around X axis, and zoom */
      float current;

      logo_phase_zoom(initialization); /* Includes End */

      current = fabs(logo_center_pos[1] - logo_pos[1]) / fabs(logo_center_pos[1] - startpos);
      current = current * 360.0f * 3.0f; /* 3.0f = N */
      logo_rot = logo_center_rot - current;
    }
}


static void logo_phase_slowroll(bool initialization)
{
  if(initialization == true)
    { /* Initialization */
      logo_rot = 0.0f;
    }
  else
    { /* Step - rotate slowly around X axis */
      logo_rot += 1.0f;

      /* Done */
      if(logo_rot > logo_center_rot)
        logo_phase_function = NULL;
    }
}


static void logo_phase_topdown(bool initialization)
{
  if(initialization == true)
    { /* Initialization */
      logo_pos[2] = 5.95f;
    }
  else
    { /* Step - rotate slowly around X axis */
      logo_pos[2] -= 0.05f;

      /* Done */
      if(logo_pos[2] < logo_center_pos[2])
        logo_phase_function = NULL;
    }
}


static void logolight_phase_left2right(bool initialization)
{
  static bool warpdone;

  if(initialization == true)
    {
      warpdone = false;
    }
  else
    { /* Step - move the light from left to right. */
      light_pos[0] += 1.0f;
      if(light_pos[0] > 10.0f)
        {
          light_pos[0] = -10.0f;
          warpdone = true;
        }

      /* Done */
      if(warpdone == true)
        {
          float start_x;

          start_x = light_center_pos[0] + light_offset[0];
          if(light_pos[0] > start_x)
            {
              light_pos[0] = start_x;
              logo_phase_function = NULL;
            }
        }
    }
}


static void logolight_phase_strobo(bool initialization)
{
  static float phase;
  
  if(initialization == true)
    {
      phase = 0.0f;
    }
  else
    {
      /* Step - vary light brightness */
      phase += 0.25f;
      light_brightness = 1.0f - sinf(phase) * 0.75f;
      assert(light_brightness >= 0.0f);
      
      /* Done */
      if(phase >= M_PI)
        {
          if(get_rand(100) < 90)
            { /* Restart this strobo effect. */
              logo_phase_function(true);
            }
          else
            { /* The end. */
              light_brightness = 1.0f; // Make sure no errors creep in.
              logo_phase_function = NULL;
            }
        }
    }
}



static void add_diamond(int x, int y)
{
  int pos;
  
  pos = x + y * map->width;
  map->data[pos] = MAP_DIAMOND;
}




#endif
