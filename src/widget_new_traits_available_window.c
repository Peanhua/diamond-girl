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
#include <assert.h>
#include <libintl.h>

static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_exit(bool pressed, SDL_Event * event);
static void on_window_unload(struct widget * this);

static struct widget * window;


struct widget * widget_new_traits_available_window(trait_t new_traits)
{
  int count;
  int width;
  const int spacing = 50;

  assert(new_traits != 0);
  if(new_traits == 0)
    return NULL;

  count = 0;
  for(int i = 0; i < TRAIT_SIZEOF_; i++)
    if(new_traits & traits_sorted[i])
      count++;
  assert(count > 0);

  width = count * spacing;
  if(width < 220)
    width = 220;

  window = widget_new_window(widget_root(), 10 + width + 10, 25 + 50 + 10, gettext("New traits available!"));
  if(window != NULL)
    {
      struct widget * frame;
      struct widget * buttons[count];
                

      widget_set_modal(window);
      widget_set_on_unload(window, on_window_unload);

      ui_push_handlers();
      ui_set_navigation_handlers();
      ui_set_common_handlers();
      ui_set_handler(UIC_EXIT,   on_window_exit);
      ui_set_handler(UIC_CANCEL, on_window_exit);

      frame = widget_new_frame(window, 10, 25, 0, 0);
      if(frame != NULL)
        {
          struct widget * prev;
          int current;

          widget_delete_flags(frame, WF_DRAW_BORDERS | WF_CAST_SHADOW);

          prev = NULL;
          current = 0;
          for(int i = 0; i < TRAIT_SIZEOF_; i++)
            if(new_traits & traits_sorted[i])
              {
                buttons[current] = widget_new_trait_button(frame, current * spacing, 0, spacing - 10 + 2, spacing - 10 + 2, traits_sorted[i], true, false);
                if(buttons[current] != NULL)
                  {
                    if(prev != NULL)
                      widget_set_navigation_leftright(prev, buttons[current]);
                    prev = buttons[current];
                  }
                current++;
              }
          assert(current == count);
          widget_resize_to_fit_children(frame);
          widget_center_horizontally(frame);

          struct widget * b;
          char buf[64];
          
          snprintf(buf, sizeof buf, " %s ", gettext("Close"));
          b = widget_new_button(window, 0, widget_y(frame) + widget_height(frame) + 20, buf);
          widget_center_horizontally(b);
          widget_set_on_release(b, on_window_close_clicked);
          widget_set_widget_pointer(window, "previously_selected_object", widget_focus());
          widget_set_focus(b);
          if(prev != NULL)
            widget_set_navigation_up(b, prev);
          for(int i = 0; i < current; i++)
            widget_set_navigation_down(buttons[i], b);

          widget_set_height(window, widget_y(b) + widget_height(b) + 10);
          widget_center_vertically(window);
        }
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

static void on_window_unload(struct widget * this)
{
  ui_pop_handlers();

  struct widget * w;

  w = widget_get_widget_pointer(this, "previously_selected_object");
  if(w != NULL)
    widget_set_focus(w);
}
