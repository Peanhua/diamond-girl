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

#include "widget.h"
#include "font.h"
#include "gfx.h"
#include "gfx_image.h"
#include "image.h"
#include "globals.h"
#include <assert.h>

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
  struct rgb c_window[COLOR_SIZEOF_]    = { { 0x00, 0x10, 0x10 }, { 0x00, 0x40, 0x40 } };
  struct rgb c_tooltip[COLOR_SIZEOF_]   = { { 0x46, 0x49, 0x08 }, { 0xff, 0xff, 0xff } };
  struct rgb c_disabled[COLOR_SIZEOF_]  = { { 0x00, 0x10, 0x10 }, { 0x00, 0x20, 0x20 } };
  struct rgb c_normal[COLOR_SIZEOF_]    = { { 0x00, 0x20, 0x20 }, { 0x00, 0x80, 0x80 } };
  struct rgb c_selected[COLOR_SIZEOF_]  = { { 0x00, 0x40, 0x40 }, { 0x00, 0xff, 0xff } };
  struct rgb c_activated[COLOR_SIZEOF_] = { { 0x00, 0x10, 0x10 }, { 0x00, 0x80, 0x80 } };
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
  else
    {
      colours = c_normal;
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
    gfx2d_draw_borders(&r, bordermode, colours[COLOR_FOREGROUND].r, colours[COLOR_FOREGROUND].g, colours[COLOR_FOREGROUND].b, 0xff);

  {
    struct image * image;
      
    image = widget_get_pointer(this, "raw_image");
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
