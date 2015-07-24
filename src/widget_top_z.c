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

int widget_top_z(struct widget * widget)
{
  int z;

  if(widget == NULL)
    widget = widget_root();
  
  assert(widget != NULL);
  if(widget != NULL)
    {
      z = widget->z_;
      
      for(int i = 0; i < widget->children_count_; i++)
        if(widget->children_[i] != NULL)
          {
            int wz;
            
            wz = widget_top_z(widget->children_[i]);
            if(wz > z)
              z = wz;
          }
    }
  else
    z = 0;
            
  return z;
}
