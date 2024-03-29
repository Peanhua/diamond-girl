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

#include <assert.h>

void widget_set_navigation_up(struct widget * widget, struct widget * up)
{
  assert(widget != NULL);
  assert(up != NULL);
  assert(widget != up);

  assert(widget->navigation_up_ == NULL);

  enum WIDGET_TYPE type_up;

  type_up = widget_get_ulong(up, "type");
  if(type_up == WT_TITLE_HIGHSCORES)
    {
      int amount;
      
      amount = widget_get_ulong(up, "amount");
      assert(amount > 0);
      assert(amount <= up->children_count_);
      assert(up->children_[amount - 1] != NULL);
      up = up->children_[amount - 1];
    }
  
  widget->navigation_up_ = up;
  stack_push(widget->widgets_linking_to_this, up);
  stack_push(up->widgets_linking_to_this, widget);
}
