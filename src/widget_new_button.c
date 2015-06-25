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

struct widget * widget_new_button(struct widget * parent, int x, int y, char const * const text)
{
  struct widget * button;

  button = widget_new_text(parent, x, y, text);
  assert(button != NULL);
  if(button != NULL)
    {
      widget_set_ulong(button, "type", WT_BUTTON);
      widget_set_flags(button, WF_FOCUSABLE | WF_ALIGN_CENTER | WF_DRAW_BORDERS | WF_BACKGROUND);
      widget_set_enabled(button, true);
    }

  return button;
}

