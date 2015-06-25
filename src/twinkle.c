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

#include "twinkle.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "random.h"
#include "globals.h"
#include <assert.h>

#define FULLSCREEN_TWINKLES 20
#define AREA_TWINKLES 300
#define MAX_TWINKLES (FULLSCREEN_TWINKLES + AREA_TWINKLES)

struct twinkle
{
  int   x, y;
  float status; /* the lifetime left, when reaches zero, it is over, usually starts from 1.0 */
  float speed;  /* the speed at which the lifetime decreases */
  float size;   /* the maximum size at lifetime = 0.50 */
  int   type;   /* the type, 0..1 */
  uint8_t r, g, b;
};


static struct twinkle twinkles[MAX_TWINKLES];

static int twinkle_timer;

#ifdef WITH_OPENGL
static struct gfxbuf * buffer;

static void twinkle_add_to_draw(GLfloat * vbuf, GLubyte * cbuf, struct twinkle * twinkle);
#endif


void twinkle_initialize(void)
{
  for(int i = 0; i < MAX_TWINKLES; i++)
    twinkles[i].status = 0.0f;
  twinkle_timer = 0;

#ifdef WITH_OPENGL
  buffer = NULL;
  if(globals.opengl)
    {
      buffer = gfxbuf_new(GFXBUF_DYNAMIC_2D, GL_LINES, GFXBUF_COLOUR | GFXBUF_BLENDING);
      gfxbuf_resize(buffer, MAX_TWINKLES * 8);
    }
#endif
}

void twinkle_cleanup(void)
{
#ifdef WITH_OPENGL
  if(buffer != NULL)
    buffer = gfxbuf_free(buffer);
#endif
}


void twinkle_draw(void)
{
#ifdef WITH_OPENGL
  if(globals.opengl && buffer != NULL)
    {
      int vbi, cbi;

      vbi = 0;
      cbi = 0;

      buffer->vertices = 0;

      for(int i = 0; i < MAX_TWINKLES; i++)
        if(twinkles[i].status > 0.0f)
          {
            if(twinkles[i].status <= 1.0f)
              {
                float size;
                float brightness;

                if(twinkles[i].status > 0.5f)
                  brightness = (1.0f - twinkles[i].status) * 2.0f;
                else
                  brightness = twinkles[i].status * 2.0f;
        
                if(twinkles[i].type == 0)
                  size = twinkles[i].size * brightness;
                else //if(twinkles[i].type == 1)
                  size = twinkles[i].size * (1.0f - twinkles[i].status);

                if(size >= 1.0f)
                  {
                    twinkle_add_to_draw(buffer->vbuf + vbi, buffer->cbuf + cbi, &twinkles[i]);
                    vbi += 2 * 8;
                    cbi += 4 * 8;
                  }
              }

            twinkles[i].status -= twinkles[i].speed;
          }

      if(vbi > 0)
        {
          gfxbuf_update(buffer, 0, vbi / 2);
          gfxgl_state(GL_BLEND, true);
          gfxbuf_draw(buffer);
          gfxgl_state(GL_BLEND, false);
        }
    }
#endif
}


#ifdef WITH_OPENGL
static void twinkle_add_to_draw(GLfloat * vbuf, GLubyte * cbuf, struct twinkle * twinkle)
{
  if(twinkle->status > 0.0f)
    {
      float size;
      float brightness;
            
      if(twinkle->status > 0.5f)
        brightness = (1.0f - twinkle->status) * 2.0f;
      else
        brightness = twinkle->status * 2.0f;
        
      if(twinkle->type == 0)
        size = twinkle->size * brightness;
      else //if(twinkle->type == 1)
        size = twinkle->size * (1.0f - twinkle->status);
      
      if(size >= 1.0f)
        {
          float tx, ty;
          int cbi, vbi;

          tx = twinkle->x + 5;
          ty = twinkle->y + 5;
          cbi = 0;
          vbi = 0;

          /* left-right */
          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = 0x00;
          vbuf[vbi++] = tx - size;
          vbuf[vbi++] = ty;

          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = brightness * 255.0f;
          vbuf[vbi++] = tx;
          vbuf[vbi++] = ty;

          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = brightness * 255.0f;
          vbuf[vbi++] = tx;
          vbuf[vbi++] = ty;

          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = 0x00;
          vbuf[vbi++] = tx + size;
          vbuf[vbi++] = ty;
              
          /* up-down */
          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = 0x00;
          vbuf[vbi++] = tx;
          vbuf[vbi++] = ty - size;
                
          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = brightness * 255.0f;
          vbuf[vbi++] = tx;
          vbuf[vbi++] = ty;
              
          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = brightness * 255.0f;
          vbuf[vbi++] = tx;
          vbuf[vbi++] = ty;

          cbuf[cbi++] = twinkle->r;
          cbuf[cbi++] = twinkle->g;
          cbuf[cbi++] = twinkle->b;
          cbuf[cbi++] = 0x00;
          vbuf[vbi++] = tx;
          vbuf[vbi++] = ty + size;
        }
    }
}
#endif

void twinkle_fullscreen(void)
{
  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      if(twinkle_timer == 0)
        {
          twinkle_timer = 10 + get_rand(5);

          bool done;

          done = false;
          for(int i = 0; done == false && i < FULLSCREEN_TWINKLES; i++)
            if(twinkles[i].status <= 0.0f)
              {
                done = true;
                twinkles[i].status = 1.0f;
                twinkles[i].speed = 0.03f;
                twinkles[i].size = 15.0f;
                twinkles[i].type = get_rand(2);
                twinkles[i].x = get_rand(SCREEN_WIDTH);
                twinkles[i].y = get_rand(SCREEN_HEIGHT);
                twinkles[i].r = 0xff;
                twinkles[i].g = 0xff;
                twinkles[i].b = 0xff;
              }
        }
      else
        twinkle_timer--;
#endif
    }
}

void twinkle_area(int x, int y, int width, int height, int amount)
{
#ifdef WITH_OPENGL
  if(globals.opengl)
    for(int i = 0; amount > 0 && i < AREA_TWINKLES; i++)
      {
        int pos;

        pos = FULLSCREEN_TWINKLES + i;
        if(twinkles[pos].status <= 0.0f)
          {
            twinkles[pos].status = 1.0f + (float) get_rand(200) / 100.0f;
            twinkles[pos].speed = 0.01f;
            twinkles[pos].size = 15.0f;
            twinkles[pos].type = get_rand(2);
            twinkles[pos].x = x + get_rand(width);
            twinkles[pos].y = y + get_rand(height);
            twinkles[pos].r = 128 + get_rand(127);
            twinkles[pos].g = 128 + get_rand(127);
            twinkles[pos].b = 128 + get_rand(127);
            amount--;
          }
      }
#endif
}
