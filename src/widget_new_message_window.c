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
#include <libintl.h>


static void on_button_clicked(struct widget * this, enum WIDGET_BUTTON button);


void widget_new_message_window(const char * title, struct widget * header, const char * message)
{
  widget_new_message_dialog(title, header, message, NULL);
}


void widget_new_message_dialog(const char * title, struct widget * header, const char * message, struct widget * button)
{
  struct widget * window;
  struct widget * textarea;
  struct widget * bclose;
  int y;

  window = widget_new_window(widget_root(), 500, 60, title);

  y = font_height() + 10;

  if(header != NULL)
    {
      struct widget * tmp;
      
      tmp = widget_reparent(header, window);
      assert(tmp == window);
      if(tmp == window)
        {
          widget_center_horizontally(header);
          widget_set_y(header, y);
          y += widget_height(header) + 20;
        }
    }
  
  textarea = widget_new_text_area(window, 5, y, 500 - 10 * 2, 50, message, true);
  widget_resize_to_fit_children(textarea);
  y += widget_height(textarea) + font_height();

  bclose = widget_new_button(window, 0, y, gettext("Close"));
  widget_set_width(bclose, 100);
  widget_center_horizontally(bclose);

  if(button != NULL)
    {
      widget_reparent(button, window);
      widget_set_x(button, widget_x(bclose) + widget_width(bclose) + 20);
      widget_set_y(button, y);
      widget_set_navigation_leftright(bclose, button);

      ui_func_on_activate_t func;
      func = widget_on_release(button);
      widget_set_pointer(button, "saved_on_release", 'P', func);
      widget_set_on_release(button, on_button_clicked);
    }
  
  y += widget_height(bclose) + 10;
  
  widget_set_widget_pointer(window, "previously_selected_object", widget_focus());
  widget_set_focus(bclose);
  
  
  widget_set_height(window, y);
  widget_center_vertically(window);
  widget_set_modal(window);

  ui_wait_for_window_close(window, bclose);
}


static void on_button_clicked(struct widget * this, enum WIDGET_BUTTON button)
{
  ui_func_on_activate_t func;
  void * ptr;

  ptr = widget_get_pointer(this, "saved_on_release", 'P');
  func = ptr;

  if(func != NULL)
    func(this, button);

  widget_delete(widget_parent(this));
}
