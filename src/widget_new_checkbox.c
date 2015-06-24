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

static void on_checkbox_clicked(struct widget * this, enum WIDGET_BUTTON button);

struct widget * widget_new_checkbox(struct widget * parent, int x, int y, int initial_state)
{
  struct widget * checkbox;

  checkbox = widget_new_child(parent, x, y, 16 + 2, 16 + 2);
  widget_set_on_activate(checkbox, on_checkbox_clicked);
  widget_set_ulong(checkbox, "type", WT_CHECKBOX);
  widget_set_flags(checkbox, WF_FOCUSABLE | WF_DRAW_BORDERS | WF_BACKGROUND);
  widget_set_enabled(checkbox, true);
  widget_set_long(checkbox, "checked", initial_state ? 1 : 0);
  widget_set_long(checkbox, "initial_state_checked", initial_state ? 1 : 0);
  widget_set_on_draw(checkbox, widget_draw);

  return checkbox;
}

static void on_checkbox_clicked(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      int is_checked;

      is_checked = widget_get_long(this, "checked") ? 0 : 1;
      widget_set_long(this, "checked", is_checked);
    }
}
