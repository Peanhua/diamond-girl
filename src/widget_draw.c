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

#include "widget.h"
#include "font.h"
#include "gfx.h"
#include "gfx_image.h"
#include "image.h"
#include "globals.h"
#include "td_object.h"
#include <assert.h>
#include <math.h>

void widget_draw(struct widget * this)
{
  SDL_Rect         r;
  uint32_t         flags;
  enum WIDGET_TYPE type;
  int              alpha;
  enum COLORS
  {
    COLOR_BACKGROUND,
    COLOR_FOREGROUND,
    COLOR_SIZEOF_
  };
  struct rgb
  {
    uint8_t r, g, b;
  };
  struct rgb c_window[COLOR_SIZEOF_]        = { { 0x00, 0x10, 0x10 }, { 0x00, 0x40, 0x40 } };
  struct rgb c_tooltip[COLOR_SIZEOF_]       = { { 0x11, 0x7a, 0x8d }, { 0xff, 0xff, 0xff } };
  struct rgb c_disabled[COLOR_SIZEOF_]      = { { 0x00, 0x10, 0x10 }, { 0x00, 0x20, 0x20 } };
  struct rgb c_normal[COLOR_SIZEOF_]        = { { 0x00, 0x20, 0x20 }, { 0x00, 0x80, 0x80 } };
  struct rgb c_selected[COLOR_SIZEOF_]      = { { 0x00, 0x40, 0x40 }, { 0x00, 0xff, 0xff } };
  struct rgb c_activated[COLOR_SIZEOF_]     = { { 0x00, 0x10, 0x10 }, { 0x00, 0x80, 0x80 } };
  struct rgb c_slider_handle[COLOR_SIZEOF_] = { { 0x00, 0x40, 0x40 }, { 0x00, 0x40, 0x40 } };
  struct rgb c_active[COLOR_SIZEOF_]        = { { 0x00, 0x00, 0xff }, { 0x00, 0x80, 0x80 } };
  struct rgb * colours;
  bool enabled;
  

  r.x = widget_absolute_x(this);
  r.y = widget_absolute_y(this);
  r.w = widget_width(this);
  r.h = widget_height(this);

  if(r.x + r.w < 0             ||
     r.y + r.h < 0             ||
     r.x       > SCREEN_WIDTH  ||
     r.y       > SCREEN_HEIGHT )
    return;

  if(globals.opengl)
    if(widget_modal() == this)
      {
        SDL_Rect tmpr;
        
        tmpr.x = 0;
        tmpr.y = 0;
        tmpr.w = SCREEN_WIDTH;
        tmpr.h = SCREEN_HEIGHT;
        gfx_draw_rectangle(&tmpr, 0x00, 0x00, 0x00, 0x40);
      }

  type = widget_get_ulong(this, "type");
  flags = widget_flags(this);
  alpha = widget_get_ulong(this, "alpha");
  if(alpha == 0)
    alpha = 0xff;
  enabled = widget_enabled(this);

  uint8_t bordermode;

  bordermode = 0;
  if(type == WT_BUTTON || type == WT_CHECKBOX || type == WT_SELECT || type == WT_SLIDER)
    {
      if(enabled == true)
        {
          if(flags & WF_ACTIVE)
            {
              colours = c_active;
            }
          else
            {
              switch(widget_state(this))
                {
                default:
                case WIDGET_STATE_NORMAL:
                  colours = c_normal;
                  bordermode = 1;
                  break;
                case WIDGET_STATE_FOCUSED:
                  colours = c_selected; 
                  bordermode = 1;
                  break;
                case WIDGET_STATE_ACTIVATED:
                  colours = c_activated;
                  bordermode = 2;
                  break;
                }
            }
        }
      else
        {
          colours = c_disabled;
          bordermode = 3;
        }
    }
  else if(type == WT_FRAME || type == WT_WINDOW || type == WT_LIST)
    {
      colours = c_window;
    }
  else if(type == WT_TOOLTIP)
    {
      colours = c_tooltip;
    }
  else if(type == WT_SLIDER_HANDLE)
    {
      colours = c_slider_handle;
    }
  else
    {
      colours = c_normal;
    }

  if(flags & WF_LIGHTER)
    {
      float multiplier;
      
      if(type == WT_FRAME || type == WT_WINDOW || type == WT_LIST)
        multiplier = 1.5f;
      else
        multiplier = 1.2f;

      for(unsigned int i = 0; i < COLOR_SIZEOF_; i++)
        {
          float v;
          v = colours[i].r;
          v *= multiplier;
          if(v > 255.0)
            v = 255.0;
          colours[i].r = v;

          v = colours[i].g;
          v *= multiplier;
          if(v > 255.0)
            v = 255.0;
          colours[i].g = v;

          v = colours[i].b;
          v *= multiplier;
          if(v > 255.0)
            v = 255.0;
          colours[i].b = v;
        }
    }

  if(flags & WF_CAST_SHADOW)
    {
      SDL_Rect tmpr;
      
      tmpr.x = r.x + 10;
      tmpr.y = r.y + 10;
      tmpr.w = r.w;
      tmpr.h = r.h;
      gfx_draw_rectangle(&tmpr, 0x00, 0x00, 0x00, 0x80);
    }

  if(flags & WF_BACKGROUND)
    gfx_draw_rectangle(&r, colours[COLOR_BACKGROUND].r, colours[COLOR_BACKGROUND].g, colours[COLOR_BACKGROUND].b, alpha);

  if(flags & WF_DRAW_BORDERS)
    {
      if(flags & WF_ONLY_FOCUS_BORDERS)
        {
          if(widget_state(this) == WIDGET_STATE_FOCUSED || widget_state(this) == WIDGET_STATE_ACTIVATED)
            gfx2d_draw_borders(&r, bordermode, colours[COLOR_FOREGROUND].r, colours[COLOR_FOREGROUND].g, colours[COLOR_FOREGROUND].b, 0xff);
        }
      else
        gfx2d_draw_borders(&r, bordermode, colours[COLOR_FOREGROUND].r, colours[COLOR_FOREGROUND].g, colours[COLOR_FOREGROUND].b, 0xff);
    }

  {
    struct image * image;
      
    image = widget_get_image_pointer(this, "raw_image");
    if(image == NULL)
      {
        enum GFX_IMAGE image_id;

        image_id = widget_get_image(this, "image");
        if(image_id != GFX_IMAGE_SIZEOF_)
          image = gfx_image(image_id);
      }

    if(image != NULL)
      {
        int imgx, imgy;
        int w, h;

        imgx = r.x;
        imgy = r.y;

        w = r.w;
        h = r.h;

        if(flags & WF_DRAW_BORDERS)
          {
            imgx++;
            imgy++;

            w -= 2;
            h -= 2;
          }

        if(flags & WF_ALIGN_IMAGE_RIGHT)
          imgx += r.w - image->width - 1;

        imgx += widget_get_long(this, "image_offset_x");
        imgx += widget_get_long(this, "image_offset_y");

        if(flags & WF_SCALE_RAW_IMAGE)
          gfx2d_draw_image_scaled(imgx, imgy, image, w, h);
        else
          gfx2d_draw_image(imgx, imgy, image);
      }

#ifdef WITH_OPENGL
    struct td_object * tdobj;

    tdobj = widget_get_pointer(this, "3dobject", 'P');
    if(tdobj != NULL)
      {
        double rotation;

        rotation = widget_get_double(this, "3d-rotation");
        rotation += 0.5;
        if(rotation >= 360.0)
          rotation -= 360.0;
        widget_set_double(this, "3d-rotation", rotation);

        double fov;
        double distance_to_center;

        fov = 35.0;
        {
          double radius;

          radius = tdobj->bounding_sphere_radius;
          
          double aspectratio;
          aspectratio = (double) r.w / (double) r.h;

          double half_min_fov_in_radians;
          half_min_fov_in_radians = 0.5 * (fov * M_PI / 180.0);
          if(aspectratio < 1.0)
            half_min_fov_in_radians = atan(aspectratio * tan(half_min_fov_in_radians));

          distance_to_center = radius / sin(half_min_fov_in_radians);
        }
        
        glViewport(r.x, SCREEN_HEIGHT - r.y - r.h, r.w, r.h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov, (double) r.w / (double) r.h, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        gfxgl_state(GL_DEPTH_TEST, true);
        glDepthFunc(GL_LESS);
        glDepthRange(0.0, 0.5);
        glCullFace(GL_BACK);
        glClear(GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        glLoadIdentity();
        gluLookAt(0, 0, 0,
                  0, 0, -distance_to_center,
                  0, 1, 0);

        glTranslatef(0, 0, -distance_to_center);

        {
          GLfloat light_a[] = { 0.0f, 0.0f, 0.0f, 1.0f };
          GLfloat light_d[] = { 1.0f, 1.0f, 1.0f, 1.0f };
          GLfloat light_s[] = { 0.2f, 0.2f, 0.2f, 1.0f };
          GLfloat pos[] = { -0.5, 0.5, 0.5, 0 };
          gfxgl_light(light_a, light_d, light_s, pos);
        }
        {
          GLfloat light_a[] = { 0.0f, 0.0f, 0.0f, 1.0f };
          GLfloat light_d[] = { 0.5f, 0.2f, 0.2f, 1.0f };
          GLfloat light_s[] = { 0.0f, 0.0f, 0.0f, 1.0f };
          GLfloat pos[] = { 0.5, 0.0, distance_to_center * 1.01, 1 };
          gfxgl_light(light_a, light_d, light_s, pos);
        }

        glRotatef(rotation, 0.2, 1.0, 0.0);
        td_object_draw(tdobj);

        gfxgl_light_off();
        
        glPopMatrix();
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        gfx_2d();
      }
#endif

    char * title;

    title = widget_get_string(this, "title");
    if(title != NULL)
      {
        SDL_Rect tmpr;

        tmpr.x = r.x + 1;
        tmpr.y = r.y + 1;
        tmpr.w = r.w - 1;
        tmpr.h = font_height() + 1;
        if(image != NULL)
          {
            tmpr.x += image->width;
            tmpr.w -= image->width;
          }
        gfx_draw_rectangle(&tmpr, colours[COLOR_FOREGROUND].r, colours[COLOR_FOREGROUND].g, colours[COLOR_FOREGROUND].b, 0x80);
        font_write(r.x + (r.w - font_width(title)) / 2, r.y + 1, title);
      }
  }

  char * text;

  text = widget_get_string(this, "text");
  if(text != NULL)
    {
      if(flags & WF_ALIGN_CENTER)
        font_write(r.x + 2 + (r.w - 4 - font_width(text)) / 2, r.y + 1, text);
      else
        font_write(r.x + 2, r.y + 1, text);
    }


  if(type == WT_CHECKBOX)
    {
      bool is_checked;

      is_checked = widget_get_long(this, "checked");
      if(is_checked)
        {
          struct image * on_image;
          
          on_image = gfx_image(GFX_IMAGE_CHECKBOX_ON);
          gfx2d_draw_image(r.x + 1, r.y, on_image);
        }
    }
}
