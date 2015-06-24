/*
  Diamond Girl - Game where player collects diamonds.
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

void widget_set_select_options(struct widget * widget, struct stack * options)
{
  assert(widget != NULL);
  if(widget != NULL)
    {
      struct stack * old;

      /* Free old */
      old = widget_get_pointer(widget, "options");
      if(old != NULL)
        stack_free(old);

      /* Update options */
      widget_set_pointer(widget, "options", options);

      /* Update currently selected label */
      unsigned int selected_index;

      selected_index = widget_get_ulong(widget, "selected_index");
      if(options != NULL && selected_index < options->size)
        widget_set_string(widget, "text", "%s", (char *) options->data[selected_index]);
    }
}
