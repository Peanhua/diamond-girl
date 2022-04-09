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
#include "gfx.h"
#include <assert.h>

void widget_center(struct widget * widget)
{
  widget_center_horizontally(widget);
  widget_center_vertically(widget);
}


void widget_center_horizontally(struct widget * widget)
{
  struct widget * parent;
  int pw;

  assert(widget != NULL);
  parent = widget_parent(widget);
  if(parent != NULL)
    pw = widget_width(parent);
  else
    pw = SCREEN_WIDTH;

  widget_set_x(widget, (pw - widget_width(widget))  / 2);
}


void widget_center_vertically(struct widget * widget)
{
  struct widget * parent;
  int ph;

  assert(widget != NULL);
  parent = widget_parent(widget);
  if(parent != NULL)
    ph = widget_height(parent);
  else
    ph = SCREEN_HEIGHT;

  widget_set_y(widget, (ph - widget_height(widget))  / 2);
}
