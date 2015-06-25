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

#include "ui.h"
#include "widget.h"

int ui_process_mouse_button(int mouse_x, int mouse_y, enum WIDGET_BUTTON button, unsigned char release)
{
  int processed;

  processed = 0;
  if(widget_root() != NULL)
    {
      struct widget * obj;

      obj = ui_get_widget_at_coordinates(mouse_x, mouse_y);
      if(obj != NULL)
        {
          processed = 1;
          if(!release)
            { /* Activate */
              if(widget_enabled(obj) == true)
                {
                  obj->state_ = WIDGET_STATE_ACTIVATED;
                  if(obj->on_activate_ != NULL)
                    obj->on_activate_(obj, button);
                  if(obj->on_activate_at_ != NULL)
                    if(mouse_x >= obj->x_ && mouse_y >= obj->y_)
                      obj->on_activate_at_(obj, button, mouse_x - widget_absolute_x(obj), mouse_y - widget_absolute_y(obj));
                }
            }
          else
            { /* Release */
              if(obj->state_ == WIDGET_STATE_ACTIVATED)
                {
                  obj->state_ = WIDGET_STATE_FOCUSED;
                  if(obj->on_release_ != NULL)
                    obj->on_release_(obj, button);
                }
            }
        }

      if(release)
        { /* In case the currently focused widget is activated, but the mouse was released somewhere else, we unactivate it here. */
          obj = widget_focus();
          if(obj != NULL)
            if(obj->state_ == WIDGET_STATE_ACTIVATED)
              obj->state_ = WIDGET_STATE_FOCUSED;
        }
    }

  return processed;
}
