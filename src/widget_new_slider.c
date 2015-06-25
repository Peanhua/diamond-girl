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
#include "widget.h"
#include "font.h"
#include "gfx.h"
#include <assert.h>

static int calc_position(struct widget * widget);
static void on_activate_at(struct widget * this, enum WIDGET_BUTTON button, int x, int y);
static void on_mouse_move(struct widget * this, int x, int y);


struct widget * widget_new_slider(struct widget * parent, int x, int y, int width, int initial_value, int min_value, int max_value)
{
  struct widget * rv;

  rv = widget_new_frame(parent, x, y, width, font_height());
  assert(rv != NULL);
  if(rv != NULL)
    {
      if(parent != NULL)
        rv->z_ = parent->z_;
      widget_set_ulong(rv, "type", WT_SLIDER);
      widget_set_flags(rv, WF_FOCUSABLE | WF_DRAW_BORDERS);
      widget_set_enabled(rv, true);
      widget_set_on_activate_at(rv, on_activate_at);
      widget_set_on_mouse_move(rv, on_mouse_move);
      widget_set_long(rv, "value", initial_value);
      widget_set_long(rv, "value_min", min_value);
      widget_set_long(rv, "value_max", max_value);

      struct widget * slider;

      slider = widget_new_frame(rv, calc_position(rv), 2, 10, font_height() - 2 * 2);
      assert(slider != NULL);
      if(slider != NULL)
        {
          slider->z_ = rv->z_;
          widget_set_flags(slider, WF_DRAW_BORDERS | WF_BACKGROUND);
          widget_set_ulong(slider, "type", WT_SLIDER_HANDLE);
        }
      widget_set_pointer(rv, "slider_obj", slider);
    }

  return rv;
}

void widget_slider_set(struct widget * widget, int value)
{
  widget_set_long(widget, "value", value);

  struct widget * slider;

  slider = widget_get_pointer(widget, "slider_obj");
  assert(slider != NULL);
  if(slider != NULL)
    widget_set_x(slider, calc_position(widget));
}


static int calc_position(struct widget * widget)
{
  int rv;
  float value, mi, ma, width;

  mi    = widget_get_long(widget, "value_min");
  ma    = widget_get_long(widget, "value_max");
  value = widget_get_long(widget, "value");
  width = widget_width(widget) - 2 * 2 - 10;
  
  rv = 2.0f + (value - mi) / ma * width;

  return rv;
}


static void on_activate_at(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED, int x, int y DG_UNUSED)
{
  float value, mi, ma, width;

  mi    = widget_get_long(this, "value_min");
  ma    = widget_get_long(this, "value_max");
  width = widget_width(this) - 2 * 2 - 10;

  x -= 2;
  if(x < 0)
    x = 0;
  else if(x > width)
    x = width;

  value = mi + (ma - mi) * (float) x / width;

  widget_slider_set(this, value);
}


static void on_mouse_move(struct widget * this, int x, int y)
{
  if(this->state_ == WIDGET_STATE_ACTIVATED)
    on_activate_at(this, WIDGET_BUTTON_LEFT, x, y);
}
