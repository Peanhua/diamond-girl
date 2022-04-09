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
#include "font.h"
#include "gfx.h"
#include <assert.h>
#include <libintl.h>

struct widget * widget_new_text_area(struct widget * parent, int x, int y, int width, int height, char const * text, bool center_horizontally)
{
  struct widget * window;

  window = widget_new_frame(parent, x, y, width, height);
  assert(window != NULL);
  if(window != NULL)
    {
      char * message_copy;

      widget_delete_flags(window, WF_CAST_SHADOW | WF_DRAW_BORDERS);

      message_copy = strdup(text);
      assert(message_copy != NULL);
      if(message_copy != NULL)
        {
          char * remaining;

          remaining = message_copy;
          y = 0;
      
          while(strlen(remaining) > 0)
            {
              char buf[1024];
              bool fits;
      
              fits = true;
              buf[0] = '\0';
              for(int i = 0; fits == true && remaining[i] != '\n' && remaining[i] != '\0' && i < (int) (sizeof buf) - 1; i++)
                {
                  buf[i] = remaining[i];
                  buf[i + 1] = '\0';
                  if(font_width(buf) >= width)
                    fits = false;
                }
              for(int i = strlen(buf) - 1; fits == false && i >= 0; i--)
                {
                  if(buf[i] == ' ' || buf[i] == '\n')
                    {
                      buf[i] = '\0';
                      if(font_width(buf) < width)
                        fits = true;
                    }
                }
              assert(fits == true);

              remaining += strlen(buf);
              if(*remaining == '\n')
                remaining++;
              while(*remaining == ' ')
                remaining++;
              
              struct widget * line;
      
              line = widget_new_text(window, 0, y, buf);
              if(center_horizontally == true)
                widget_add_flags(line, WF_ALIGN_CENTER);
              widget_set_width(line, width);
              y += font_height();
            }

          free(message_copy);
        }
    }
  
  return window;
}
