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

#include "widget.h"
#include "gc.h"

#include <errno.h>

struct widget * widget_new(int x, int y, int width, int height)
{
  struct widget * obj;

  obj = malloc(sizeof(struct widget));
  if(obj != NULL)
    {
      obj->user_data_names_ = NULL;
      obj->user_data_       = NULL;
      obj->user_data_size_  = 0;
      obj->x_               = x;
      obj->y_               = y;
      obj->z_               = 0;
      obj->width_           = width;
      obj->height_          = height;
      obj->flags_           = 0;
      obj->on_draw_         = NULL;
      obj->on_focus_        = NULL;
      obj->on_activate_     = NULL;
      obj->on_activate_at_  = NULL;
      obj->on_release_      = NULL;
      obj->on_mouse_move_   = NULL;
      obj->on_unload_       = NULL;
      obj->state_           = WIDGET_STATE_NORMAL;
      
      obj->parent_         = NULL;
      obj->children_       = NULL;
      obj->children_count_ = 0;

      obj->navigation_up_    = NULL;
      obj->navigation_left_  = NULL;
      obj->navigation_right_ = NULL;
      obj->navigation_down_  = NULL;
      obj->widgets_linking_to_this = stack_new();

      obj->tooltip_.text   = NULL;
      obj->tooltip_.widget = NULL;

#if defined(UI_DEBUG)
      obj->deleted_ = 0;
#endif

#ifndef NDEBUG
      gc_new(GCT_WIDGET, obj);
#endif
    }
  else
    fprintf(stderr, "%s:%d: Failed to allocate memory for widget: %s\n", __FILE__, __LINE__, strerror(errno));

  return obj;
}
