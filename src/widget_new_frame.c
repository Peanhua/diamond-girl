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
#include "image.h"

#include <assert.h>

struct widget * widget_new_frame(struct widget * parent, int x, int y, int width, int height)
{
  struct widget * frame;

  if(parent == NULL)
    parent = widget_root();

  if(parent == NULL)
    frame = widget_new_root(x, y, width, height);
  else
    {
      frame = widget_new_child(parent, x, y, width, height);
      frame->z_ += 1;
    }
  assert(frame != NULL);
  
  widget_set_on_draw(frame, widget_draw);
  widget_set_ulong(frame, "type", WT_FRAME);
  widget_set_flags(frame, WF_CAST_SHADOW | WF_DRAW_BORDERS | WF_BACKGROUND);

  return frame;
}
