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

#include "gfx.h"
#include "widget.h"
#include <assert.h>

void widget_center_on_parent(struct widget * widget)
{
  struct widget * parent;
  int pw, ph;

  assert(widget != NULL);
  parent = widget_parent(widget);
  if(parent != NULL)
    {
      pw = widget_width(parent);
      ph = widget_height(parent);
    }
  else
    {
      pw = SCREEN_WIDTH;
      ph = SCREEN_HEIGHT;
    }

  widget_set_x(widget, (pw - widget_width(widget))  / 2);
  widget_set_y(widget, (ph - widget_height(widget)) / 2);
}
