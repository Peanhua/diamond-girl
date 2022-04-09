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

#include "ui.h"
#include "widget.h"
#include <assert.h>

static int get_maxz(struct widget * obj, int maxz);
static struct widget * object_at_process(struct widget * obj, int x, int y, int z);


struct widget * ui_get_widget_at_coordinates(int x, int y)
{
  struct widget * start;
  struct widget * modal_object;

  modal_object = widget_modal();
  if(modal_object != NULL)
    start = modal_object;
  else
    start = widget_root();

  int maxz;

  maxz = get_maxz(start, -1);
  
  struct widget * rv;

  rv = NULL;
  for(int z = 0; z <= maxz; z++)
    {
      struct widget * tmp;

      tmp = object_at_process(start, x, y, z);
      if(tmp != NULL)
        rv = tmp;
    }

  return rv;
}

static int get_maxz(struct widget * obj, int maxz)
{
  if(obj->z_ > maxz)
    maxz = obj->z_;

  for(int i = 0; i < obj->children_count_; i++)
    if(obj->children_[i] != NULL)
      maxz = get_maxz(obj->children_[i], maxz);
  
  return maxz;
}

static struct widget * object_at_process(struct widget * obj, int x, int y, int z)
{
  assert(obj != NULL && obj->deleted_ == false);

  struct widget * rv;

  rv = NULL;

  if((widget_flags(obj) & WF_CLIPPING) == 0 ||
     ( x >= widget_absolute_x(obj) && x < widget_absolute_x(obj) + widget_width(obj) &&
       y >= widget_absolute_y(obj) && y < widget_absolute_y(obj) + widget_height(obj) ))
    for(int i = 0; rv == NULL && i < obj->children_count_; i++)
      if(obj->children_[i] != NULL)
        rv = object_at_process(obj->children_[i], x, y, z);
  
  if(rv == NULL)
    if(z == obj->z_)
      if(widget_flags(obj) & WF_FOCUSABLE)
        if( ! (widget_flags(obj) & WF_HIDDEN) )
          if(x >= widget_absolute_x(obj) && x < widget_absolute_x(obj) + widget_width(obj) &&
             y >= widget_absolute_y(obj) && y < widget_absolute_y(obj) + widget_height(obj))
            rv = obj;

  return rv;
}
