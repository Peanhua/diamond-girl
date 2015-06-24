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

#include "ui.h"
#include "widget.h"

void ui_process_mouse_move(int mouse_x, int mouse_y)
{
  if(widget_root() != NULL)
    {
      struct widget * focused;

      focused = widget_focus();

      /* Check for focus change. */
      struct widget * obj;

      obj = ui_get_widget_at_coordinates(mouse_x, mouse_y);
      if(focused != obj)
        {
          if(focused != NULL)
            { /* Mouse moved out of the previously focused widget, hide tooltip. */
              if(focused->tooltip_.widget != NULL && (widget_flags(focused->tooltip_.widget) & WF_HIDDEN) == 0)
                widget_add_flags(focused->tooltip_.widget, WF_HIDDEN);
            }

          if(obj != NULL && widget_flags(obj) & WF_FOCUSABLE)
            { /* Focus is going to change. */
              focused = obj;
              widget_set_focus(focused);
            }
        }

      if(focused != NULL && focused == obj && focused->tooltip_.text != NULL)
        { /* Show the tooltip. */
          if(focused->tooltip_.widget == NULL)
            { /* Create new tooltip widget. */
              struct widget * t;
                  
              t = widget_new_tooltip(focused);
              focused->tooltip_.widget = t; 
            }
          else 
            if(focused->tooltip_.widget != NULL && (widget_flags(focused->tooltip_.widget) & WF_HIDDEN))
              { /* Re-show the hidden tooltip widget. */
                widget_delete_flags(focused->tooltip_.widget, WF_HIDDEN);
              }
        }
      
      /* Call the callback of the currently focused widget. */
      if(focused != NULL)
        if(mouse_x >= focused->x_ && mouse_y >= focused->y_)
          if(focused->on_mouse_move_ != NULL)
            focused->on_mouse_move_(focused, mouse_x - widget_absolute_x(focused), mouse_y - widget_absolute_y(focused));
    }
}
