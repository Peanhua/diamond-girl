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
#include "font.h"
#include "widget.h"
#include "gfx.h"
#include "themes.h"
#include "stack.h"

#include <assert.h>
#include <libintl.h>

static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_exit(bool pressed, SDL_Event * event);
static void on_window_unload(struct widget * this);

static struct widget * window;

struct widget * widget_new_theme_info_window(struct theme * theme)
{
  char linebuf[40 + 1];

  assert(theme != NULL);
  snprintf(linebuf, sizeof linebuf, gettext("Special day: %s"), theme->name);

  window = widget_new_window(widget_root(), 400, 300, linebuf);
  assert(window != NULL);
  if(window != NULL)
    {
      int left_x, y;

      widget_set_modal(window);
      widget_set_on_unload(window,  on_window_unload);

      ui_push_handlers();
      ui_set_handler(UIC_EXIT,   on_window_exit);
      ui_set_handler(UIC_CANCEL, on_window_exit);

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

      
      struct widget * b;

      y += 20;
      b = widget_new_button(window, left_x, y, gettext(" Close "));
      widget_set_x(b, (widget_width(window) - widget_width(b)) / 2);
      widget_set_on_release(b, on_window_close_clicked);
      widget_set_focus(b);
      y += widget_height(b);

      widget_set_height(window, y + 10);
      widget_set_y(window, (widget_height(widget_parent(window)) - widget_height(window)) / 2);
    }

  return window;
}


static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  widget_delete(widget_parent(this));
}

static void on_window_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    widget_delete(window);
}

static void on_window_unload(struct widget * this DG_UNUSED)
{
  ui_pop_handlers();
}
