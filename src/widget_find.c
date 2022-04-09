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

struct widget * widget_find(struct widget * root, const char * id)
{
  struct widget * rv;
  char * tmp;

  rv = NULL;

  if(root == NULL)
    root = widget_root();

  if(root != NULL)
    {
      tmp = widget_get_string(root, "id");
      if(tmp != NULL)
        if(!strcmp(tmp, id))
          rv = root;

      for(int i = 0; rv == NULL && i < root->children_count_; i++)
        if(root->children_[i] != NULL)
          rv = widget_find(root->children_[i], id);
    }

  return rv;
}

