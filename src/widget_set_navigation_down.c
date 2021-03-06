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

#include <assert.h>

void widget_set_navigation_down(struct widget * widget, struct widget * down)
{
  assert(widget != NULL);
  assert(down != NULL);
  assert(widget != down);

  if(widget->navigation_down_ == down)
    return;
  
  assert(widget->navigation_down_ == NULL);

  enum WIDGET_TYPE type_up;

  type_up = widget_get_ulong(widget, "type");
  if(type_up == WT_TITLE_HIGHSCORES)
    {
      int amount;
      
      amount = widget_get_ulong(widget, "amount");
      assert(amount > 0);
      assert(amount <= widget->children_count_);
      assert(widget->children_[amount - 1] != NULL);
      widget = widget->children_[amount - 1];
    }
  
  widget->navigation_down_ = down;
  stack_push(widget->widgets_linking_to_this, down);
  stack_push(down->widgets_linking_to_this, widget);
}
