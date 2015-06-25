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
#include <libintl.h>

static void on_window_unload(struct widget * this);
static void on_window_exit(bool pressed, SDL_Event * event);
static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);

static struct widget * window;

struct widget * widget_new_message_window(const char * title, const char * message)
{
  struct widget * bclose;
  int y;
  char * message_copy;
  char * p;

  window = widget_new_window(widget_root(), 500, 60, title);

  message_copy = strdup(message);
  p = message_copy;
  y = 25;

  do
    {
      char * lf;
      struct widget * line;
      
      lf = strchr(p, '\n');
      if(lf != NULL)
        *lf = '\0';
      line = widget_new_text(window, 10, y, p);
      widget_add_flags(line, WF_ALIGN_CENTER);
      widget_set_width(line, widget_width(window) - 10 * 2);
      y += font_height();
      if(lf != NULL)
        p = lf + 1;
      else
        p = NULL;
    }
  while(p != NULL);
  
  y += 20;
  bclose = widget_new_button(window, 0, y, gettext("Close"));
  y += widget_height(bclose) + 10;
  widget_set_height(window, y);
  widget_set_width(bclose, 100);
  widget_set_x(bclose, (widget_width(window) - widget_width(bclose)) / 2);
  widget_set_on_release(bclose, on_window_close_clicked);
  
  widget_set_pointer(window, "previously_selected_object", widget_focus());
  widget_set_focus(bclose);
  
  
  widget_set_modal(window);
  /* Replace cancel and exit handlers. */
  ui_push_handlers();
  ui_set_handler(UIC_EXIT,   on_window_exit);
  ui_set_handler(UIC_CANCEL, on_window_exit);
  widget_set_on_unload(window, on_window_unload);

  free(message_copy);

  return window;
}



static void on_window_unload(struct widget * this DG_UNUSED)
{
  ui_pop_handlers();
}

static void on_window_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * w;

      w = widget_get_pointer(window, "previously_selected_object");

      window = widget_delete(window);

      if(w != NULL)
        widget_set_focus(w);
    }
}

static void on_window_close_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  on_window_exit(true, NULL);
}
