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
#include "gfx.h"
#include "gfx_image.h"
#include "image.h"

#include <assert.h>

static void on_close_clicked(struct widget * this, enum WIDGET_BUTTON button);

struct widget * widget_new_window(struct widget * parent, int width, int height, char const * const title)
{
  struct widget * window;
  int x, y;

  x = (SCREEN_WIDTH - width ) / 2;
  y = (SCREEN_HEIGHT - height) / 2;

  if(parent != NULL)
    {
      x -= widget_absolute_x(parent);
      y -= widget_absolute_y(parent);
    }

  window = widget_new_frame(parent, x, y, width, height);
  assert(window != NULL);

  if(window != NULL)
    {
      window->z_ = widget_top_z(widget_root()) + 1;

      widget_set_ulong(window, "type", WT_WINDOW);
      if(title != NULL)
        widget_set_string(window, "title", "%s", title);

      struct image *  img;
      struct widget * closebutton;

      img = gfx_image(GFX_IMAGE_CLOSE_BUTTON);
      assert(img != NULL);
      if(img != NULL)
        {
          closebutton = widget_new_button(window,
                                          widget_width(window) - img->width - 2,
                                          0,
                                          NULL);
          widget_set_image(closebutton, "image", GFX_IMAGE_CLOSE_BUTTON);
          widget_set_width(closebutton, img->width + 2);
          widget_set_height(closebutton, img->height + 2);
      
          widget_set_on_release(closebutton, on_close_clicked);

          widget_set_widget_pointer(window, "closebutton", closebutton);
        }
      else
        window = widget_delete(window);
    }

  return window;
}

static void on_close_clicked(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct widget * window;

      window = widget_parent(this);
      widget_delete(window);
    }
}
