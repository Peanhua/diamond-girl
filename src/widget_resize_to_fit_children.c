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

void widget_resize_to_fit_children(struct widget * grid_row)
{
  assert(grid_row != NULL);
  if(grid_row != NULL)
    {
      int height, width;
      bool height_dirty, width_dirty;

      width = widget_width(grid_row);
      width_dirty = false;

      height = widget_height(grid_row);
      height_dirty = false;

      for(int i = 0; i < grid_row->children_count_; i++)
        if(grid_row->children_[i] != NULL)
          {
            int item_x, item_width;
            int item_y, item_height;

            item_x      = widget_x(grid_row->children_[i]);
            item_y      = widget_y(grid_row->children_[i]);
            item_width  = widget_width(grid_row->children_[i]);
            item_height = widget_height(grid_row->children_[i]);

            if(item_x + item_width > width)
              {
                width = item_x + item_width;
                width_dirty = true;
              }

            if(item_y + item_height > height)
              {
                height = item_y + item_height;
                height_dirty = true;
              }
          }

      if(width_dirty)
        widget_set_width(grid_row, width);

      if(height_dirty)
        widget_set_height(grid_row, height);
    }
}
