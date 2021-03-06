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

#ifndef NDEBUG

#include "widget.h"

void widget_dump(struct widget * widget)
{
  printf("Dumping widget: %p\n", widget);
  if(widget != NULL)
    {
      printf(" parent: %p\n", widget->parent_);
      printf(" user data:\n");
      for(int i = 0; i < widget->user_data_size_; i++)
        if(widget->user_data_names_[i] != NULL)
          {
            printf("  %s", widget->user_data_names_[i]);
            if(!strcmp(widget->user_data_names_[i], "text"))
              printf(" '%s'", (char *) widget->user_data_[i]);
            printf("\n");
          }
    }
}

#endif
