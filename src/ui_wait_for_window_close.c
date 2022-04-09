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

#include "diamond_girl.h"
#include "gc.h"
#include "stack.h"
#include "ui.h"
#include "widget.h"
#include <assert.h>

static struct stack * windows = NULL;

static void on_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_unload(struct widget * this);
static void on_window_exit(bool pressed, SDL_Event * event);


void ui_wait_for_window_close(struct widget * window, struct widget * closebutton)
{
  if(windows == NULL)
    windows = stack_new();

  assert(windows != NULL);
  stack_push(windows, window);
  
  widget_set_on_unload(window, on_window_unload);

  widget_set_widget_pointer(closebutton, "window", window);
  widget_set_on_release(closebutton, on_close_clicked);
  widget_add_flags(closebutton, WF_FOCUS_HINT);

  ui_push_handlers();
  ui_set_handler(UIC_EXIT,   on_window_exit);
  ui_set_handler(UIC_CANCEL, on_window_exit);
}


static void on_close_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct widget * window;

  window = widget_get_widget_pointer(this, "window");
  if(window != NULL)
    {  
      struct widget * previously_selected;

      previously_selected = widget_get_widget_pointer(window, "previously_selected_object");
  
      widget_delete(window);
  
      if(previously_selected != NULL)
        widget_set_focus(previously_selected);
    }
}

static void on_window_unload(struct widget * this)
{
  ui_pop_handlers();

  assert(windows != NULL);
  assert(windows->size > 0);

#ifndef NDEBUG
  bool success;
  
  success =
#endif
    stack_pull(windows, this);
  
  assert(success == true);
}

static void on_window_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      assert(windows != NULL);
      assert(windows->size > 0);
      struct widget * window;

      window = windows->data[windows->size - 1];
      assert(gc_check(GCT_WIDGET, window) == true);

      struct widget * w;
      
      w = widget_get_widget_pointer(window, "previously_selected_object");
      
      widget_delete(window);
      
      if(w != NULL)
        widget_set_focus(w);
    }
}
