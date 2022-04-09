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

#include "gc.h"
#include "widget.h"
#include <assert.h>

struct widget * widget_new_gfx_image(struct widget * parent, int x, int y, enum GFX_IMAGE gfx_image_id)
{
  struct widget * widget;

  widget = widget_new_child(parent, x, y, 0, 0);
  if(widget != NULL)
    {
      widget_set_on_draw(widget, widget_draw);
      widget_set_flags(widget, WF_DRAW_BORDERS | WF_BACKGROUND);

      widget_set_image(widget, "image", gfx_image_id);
      widget_to_image_size(widget, gfx_image(gfx_image_id));
    }

  return widget;
}

struct widget * widget_new_image(struct widget * parent, int x, int y, struct image * image)
{
  struct widget * widget;

  assert(gc_check(GCT_IMAGE, image) == true);
  widget = widget_new_child(parent, x, y, 0, 0);
  if(widget != NULL)
    {
      widget_set_on_draw(widget, widget_draw);
      widget_set_flags(widget, WF_DRAW_BORDERS | WF_BACKGROUND);

      widget_set_image_pointer(widget, "raw_image", image);
      widget_to_image_size(widget, image);
    }

  return widget;
}
