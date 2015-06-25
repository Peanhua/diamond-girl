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

void widget_center(struct widget * widget)
{
  int x, y;

  x = (SCREEN_WIDTH  - widget_width(widget) ) / 2;
  y = (SCREEN_HEIGHT - widget_height(widget)) / 2;

  if(widget_parent(widget) != NULL)
    {
      x -= widget_absolute_x(widget_parent(widget));
      y -= widget_absolute_y(widget_parent(widget));
    }
  widget_set_x(widget, x);
  widget_set_y(widget, y);
}
