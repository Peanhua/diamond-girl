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

#include "font.h"
#include "widget.h"
#include "gfx.h"
#include "themes.h"
#include "stack.h"

#include <assert.h>
#include <libintl.h>

void widget_new_theme_info_window(struct theme * theme)
{
  char linebuf[40 + 1];
  struct widget * window;

  assert(theme != NULL);
  snprintf(linebuf, sizeof linebuf, gettext("Special day: %s"), theme->name);

  window = widget_new_window(widget_root(), 400, 300, linebuf);
  assert(window != NULL);
  if(window != NULL)
    {
      int left_x, y;

      widget_set_modal(window);

      left_x = 10;
      y = 2 * font_height();

      if(theme->text != NULL)
        {
          widget_new_text(window,
                          (widget_width(window) - left_x * 2 - font_width(theme->text)) / 2,
                          y,
                          theme->text);
          y += font_height() * 2;
        }

      char ** standard_description;

      standard_description = read_localized_text_file(get_data_filename("data/theme"));
      if(standard_description != NULL)
        {
          for(int i = 0; standard_description[i] != NULL; i++)
            {
              widget_new_text(window,
                              (widget_width(window) - left_x * 2 - font_width(standard_description[i])) / 2,
                              y,
                              standard_description[i]);
              free(standard_description[i]);
              y += font_height();
            }
          free(standard_description);
        }

      
      struct widget * closebutton;

      y += 20;
      snprintf(linebuf, sizeof linebuf, " %s ", gettext("Close"));
      closebutton = widget_new_button(window, left_x, y, linebuf);
      widget_set_x(closebutton, (widget_width(window) - widget_width(closebutton)) / 2);
      widget_set_focus(closebutton);
      y += widget_height(closebutton);

      widget_set_height(window, y + 10);
      widget_set_y(window, (widget_height(widget_parent(window)) - widget_height(window)) / 2);

      ui_wait_for_window_close(window, closebutton);
    }
}
