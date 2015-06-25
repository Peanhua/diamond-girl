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

#include "game.h"
#include "widget.h"
#include <assert.h>

static struct widget * current_message = NULL; /* The currently displayed message (a window in the middle of the screen). */

void game_show_text(char * text)
{
  if(current_message != NULL)
    current_message = widget_delete(current_message);

  if(text != NULL)
    {
      char * lines[10];
      int linecount;
      char * buf;
      char * p, * tmp;

      buf = strdup(text);
      p = buf;
      linecount = 0;
      do {
        assert(linecount < 10);
        lines[linecount] = p;
        tmp = strchr(p, '\n');
        if(tmp != NULL)
          {
            *tmp = '\0';
            p = tmp + 1;
          }
        linecount++;
      } while(tmp != NULL);

      current_message = widget_new_game_window(lines, linecount);

      free(buf);
    }
}
