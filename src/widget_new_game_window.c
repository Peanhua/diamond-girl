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
#include "font.h"
#include "gfx.h"

#include <assert.h>

/* Show some lines of text (during the game). */
struct widget * widget_new_game_window(char ** lines, int lines_count)
{
  int width, height;
  int x, y;

  width = 10;
  for(int i = 0; i < lines_count; i++)
    {
      int w;

      w = font_width(lines[i]);
      if(w > width)
        width = w;
    }
  width += 20;

  height = (2 + lines_count) * font_height();

  x = (SCREEN_WIDTH - width) / 2;
  y = (SCREEN_HEIGHT - height) / 2;

  struct widget * window;

  window = widget_new_frame(widget_root(), x, y, width, height);
  assert(window != NULL);

  widget_delete_flags(window, WF_DRAW_BORDERS | WF_CAST_SHADOW);
  widget_set_ulong(window, "alpha", 0xc0);

  for(int i = 0; i < lines_count; i++)
    widget_new_text(window, 10, (1 + i) * font_height(), lines[i]);

  return window;
}
