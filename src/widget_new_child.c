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
#include <errno.h>

struct widget * widget_new_child(struct widget * parent, int x, int y, int width, int height)
{
#if defined(UI_DEBUG)
  assert(parent != NULL && parent->deleted_ == 0);
#endif
  struct widget * obj;

  assert(parent != NULL);

  obj = widget_new(x, y, width, height);
  if(obj != NULL)
    {
      int ind;

      ind = -1;
      for(int i = 0; ind == -1 && i < parent->children_count_; i++)
        if(parent->children_[i] == NULL)
          ind = i;
      if(ind == -1)
        {
          struct widget ** tmp;

          tmp = realloc(parent->children_, sizeof(struct widget *) * (parent->children_count_ + 1));
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              ind = parent->children_count_;

              parent->children_        = tmp;
              parent->children_count_ += 1;
            }
          else
            fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
        }
      assert(ind >= 0);
      if(ind != -1)
        {
          parent->children_[ind] = obj;
          obj->parent_ = parent;
          obj->z_ = parent->z_;
        }
      else
        obj = widget_delete(obj);
    }

  return obj;
}
