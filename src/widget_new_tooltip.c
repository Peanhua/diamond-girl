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
#include "font.h"
#include "gfx.h"

struct widget * widget_new_tooltip(struct widget * parent)
{ /* Create the tooltip widget. */
  struct widget * frame;
  int x, y, width, height;
  
  width  = font_width(parent->tooltip_.text) + 4;
  height = font_height() + 4;

  x = widget_absolute_x(parent) - width / 2 + widget_width(parent) / 2;
  if(x + width > SCREEN_WIDTH)
    x = SCREEN_WIDTH - width;
  if(x < 0)
    x = 0;
  
  y = widget_absolute_y(parent) - height - 4;
  if(y + height > SCREEN_HEIGHT)
    y = SCREEN_HEIGHT - height;
  if(y < 0)
    y = 0;

  frame = widget_new_frame(widget_root(), x, y, width, height);
  frame->z_ = parent->z_ + 1;
  widget_set_ulong(frame, "type", WT_TOOLTIP);
  widget_delete_flags(frame, WF_DRAW_BORDERS);
  widget_set_ulong(frame, "alpha", 0x80);
  
  widget_new_text(frame, 0, 0, parent->tooltip_.text);

  widget_set_no_focus(frame, true);

  return frame;
}
