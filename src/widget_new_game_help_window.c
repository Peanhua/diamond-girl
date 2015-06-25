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

#include "diamond_girl.h"
#include "widget.h"
#include "font.h"
#include "gfx.h"
#include <assert.h>

/* Display window with help text in game.
 * The help text is divided into two columns, separated in the source file with a tabulator.
 * The right column is aligned by the maximum width of the left column.
 */
struct widget * widget_new_game_help_window(void)
{
  struct widget * help_window;

  help_window = NULL;
  
  char ** help;

  help = read_localized_text_file(get_data_filename("data/game-help"));
  if(help != NULL)
    {
      help_window = widget_new_frame(widget_root(), 0, 0, 0, 0);
      if(help_window != NULL)
        {
          int leftw;
          const int maxlines = 128;
          char * help_right[maxlines];

          leftw = 0;
          for(int i = 0; help[i] != NULL && i < maxlines; i++)
            { /* Determine the maximum width of the left side, and remember right side texts. */
              char * tab;

              tab = strchr(help[i], '\t');
              if(tab != NULL)
                {
                  *tab = '\0';
                  help_right[i] = tab + 1;

                  int tmpw;
                  
                  tmpw = font_width(help[i]);
                  if(tmpw > leftw)
                    leftw = tmpw;
                }
              else
                help_right[i] = NULL;
            }

          int w, y;

          w = 0;
          y = font_height();
          for(int i = 0; help[i] != NULL; i++)
            {
              int tmpw;

              widget_new_text(help_window, 15, y, help[i]);
              tmpw = font_width(help[i]);
              
              if(help_right[i] != NULL)
                {
                  widget_new_text(help_window, 15 + leftw + 15, y, help_right[i]);
                  tmpw += font_width(help_right[i]);
                }
              
              free(help[i]);
              
              if(tmpw > w)
                w = tmpw;

              y += font_height();
            }
          free(help);

          widget_set_height(help_window, y + font_height());
          widget_set_width(help_window, 15 + w + 15 + 15); // 15 left margin, 15 center padding, 15 right margin
          widget_center(help_window);
        }
    }

  return help_window;
}
