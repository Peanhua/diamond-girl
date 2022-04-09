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
#include "font.h"

#include <assert.h>

struct widget * widget_new_text(struct widget * parent, int x, int y, char const * const text)
{
  struct widget * obj;
  int w, h;

  if(text != NULL)
    {
      w = 2 + font_width(text) + 2;
      h = 1 + font_height() + 1;
    }
  else
    {
      w = 4;
      h = 4;
    }
  obj = widget_new_child(parent, x, y, w, h);
  assert(obj != NULL);
  if(obj != NULL)
    {
      widget_set_on_draw(obj, widget_draw);
      widget_set_ulong(obj, "type", WT_TEXT);
      widget_set_flags(obj, 0);

      if(text != NULL)
        widget_set_string(obj, "text", "%s", text);
    }

  return obj;
}
