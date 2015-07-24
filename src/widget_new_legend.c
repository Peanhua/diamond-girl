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
#include "font.h"
#include "gfx.h"
#include <assert.h>

static void on_draw(struct widget * this);

struct widget * widget_new_legend(struct widget * parent, int x, int y)
{
  struct widget * obj;
  unsigned int width, height;
  
  draw_legend(0, 0, &width, &height);

  obj = widget_new_child(parent, x, y, width, height);
  assert(obj != NULL);
  if(obj != NULL)
    widget_set_on_draw(obj, on_draw);

  return obj;
}

static void on_draw(struct widget * this)
{
  int x, y;

  x = widget_absolute_x(this);
  y = widget_absolute_y(this);
  draw_legend(x, y, NULL, NULL);
}
