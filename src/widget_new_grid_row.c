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

struct widget * widget_new_grid_row(struct widget * grid, int y)
{
  struct widget * row;

  assert(grid != NULL);
  assert(widget_get_ulong(grid, "type") == WT_GRID);
  row = NULL;
  if(grid != NULL)
    {
      /* Find the Y coordinate for this row. */
      int padding_y;

      padding_y = widget_get_ulong(grid, "padding-y");
      for(int i = 0; i < grid->children_count_; i++)
        if(grid->children_[i] != NULL)
          {
            assert(widget_get_ulong(grid->children_[i], "type") == WT_GRID_ROW);
            y += widget_height(grid->children_[i]) + padding_y;
          }

      row = widget_new_child(grid, 0, y, 0, 0);
      widget_set_ulong(row, "type", WT_GRID_ROW);
    }

  return row;
}
