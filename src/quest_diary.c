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

#include "cave.h"
#include "font.h"
#include "quest.h"
#include "image.h"
#include "trader.h"
#include "treasure.h"
#include "widget.h"
#include <assert.h>
#include <libintl.h>

static void refresh(struct questline * questline, struct widget * window);
static int output_entry(struct widget * window, int x, int y, struct diary_entry * entry);

static void on_previous_page_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_next_page_clicked(struct widget * this, enum WIDGET_BUTTON button);


static unsigned int entries_per_page;
static unsigned int current_page;


void quest_diary(struct questline * questline)
{
  struct widget * window;

  assert(questline != NULL);
  window = widget_new_window(NULL, 720, 520, gettext("My Diary"));
  if(window != NULL)
    {
      widget_set_questline_pointer(window, "questline", questline);
      widget_set_modal(window);
      widget_set_y(window, 70);

      entries_per_page = 4;
      assert(questline->diary_entries->size > 0);
      current_page = (questline->diary_entries->size - 1) / entries_per_page;

      struct widget * w;
      struct widget * prev;
      int x, y;
      char buf[128];

      x = 10;
      y = widget_height(window) - font_height() * 2;
      w = widget_new_text(window, x, y, "");
      widget_set_string(w, "id", "%s", "current_page");
      x += 120 + 4;
      
      w = widget_new_button(window, x, y, " < ");
      widget_set_on_release(w, on_previous_page_clicked);
      x += widget_width(w) + 4;

      prev = w;
      w = widget_new_button(window, x, y, " > ");
      widget_set_on_release(w, on_next_page_clicked);
      widget_set_navigation_leftright(prev, w);
      //x += widget_width(w) + 4;

      struct widget * closebutton;

      snprintf(buf, sizeof buf, " %s ", gettext("Close"));
      closebutton = widget_new_button(window, x, y, buf);
      widget_center_horizontally(closebutton);
      widget_set_navigation_leftright(w, closebutton);

      refresh(questline, window);

      widget_set_widget_pointer(window, "previously_selected_object", widget_focus());
      widget_set_focus(closebutton);
      ui_wait_for_window_close(window, closebutton);
    }
}


      
static void refresh(struct questline * questline, struct widget * window)
{
  struct widget * container;
  int y;

  y = font_height() + 5;

  container = widget_find(window, "container");
  if(container != NULL)
    widget_delete(container);

  {
    struct widget * w;

    w = widget_find(window, "current_page");
    assert(w != NULL);
    assert(questline->diary_entries->size > 0);
    widget_set_string(w, "text", gettext("Page %u/%u"), current_page + 1, (questline->diary_entries->size - 1) / entries_per_page + 1);
  }
  
  container = widget_new_frame(window, 5, y, widget_width(window) - 5 * 2, widget_height(window) - y - font_height() * 3);
  assert(container != NULL);
  widget_set_string(container, "id", "%s", "container");
  widget_delete_flags(container, WF_CAST_SHADOW);
  widget_add_flags(container, WF_LIGHTER);
  
  y = 5;

  int start;

  start = current_page * entries_per_page;
  for(unsigned int i = 0; i < entries_per_page && (start + i) < questline->diary_entries->size; i++)
    y = output_entry(container, 5, y, questline->diary_entries->data[start + i]);
}



static int output_entry(struct widget * window, int x, int y, struct diary_entry * entry)
{
  struct widget * tobj;
  { /* Date: */
    struct tm * tm;
    char buf[128];

    assert(entry != NULL);
    tm = localtime(&entry->timestamp);
#ifdef WIN32
    strftime(buf, sizeof buf, gettext("%A, %d of %B %Y, at %H:%M"), tm);
#else
    strftime(buf, sizeof buf, gettext("%A, %e of %B %Y, at %H:%M"), tm);
#endif
    buf[0] = toupper(buf[0]);
    tobj = widget_new_text(window, x, y, buf);
    y = widget_y(tobj) + widget_height(tobj) + font_height() / 4;
  }

  /* Word-wrap the description into multiple lines: */
  int maxwidth;
  struct widget * w;
  
  maxwidth = widget_width(window) - x * 2;

  w = widget_new_text_area(window, x, y, maxwidth, 10, entry->entry, false);
  widget_delete_flags(w, WF_BACKGROUND | WF_DRAW_BORDERS);
  widget_resize_to_fit_children(w);
  y += widget_height(w);

  if(entry->image != NULL)
    {
      w = widget_new_image(window, widget_width(tobj) + 100, widget_y(tobj) + widget_height(tobj) - 40 - 2, entry->image);
      widget_delete_flags(w, WF_DRAW_BORDERS | WF_BACKGROUND);
      widget_add_flags(w, WF_SCALE_RAW_IMAGE);

      double ar;

      ar = (double) widget_width(w) / (double) widget_height(w);
      widget_set_width(w, 40.0 * ar);
      widget_set_height(w, 40.0);
    }

  y += font_height() * 2;
  
  return y;
}


static void on_previous_page_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct widget * window;
  struct questline * questline;

  window = widget_parent(this);
  questline = widget_get_questline_pointer(window, "questline");
  if(current_page > 0)
    {
      current_page--;
      refresh(questline, window);
    }
}

static void on_next_page_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct widget * window;
  struct questline * questline;

  window = widget_parent(this);
  questline = widget_get_questline_pointer(window, "questline");
  if((current_page + 1) * entries_per_page < questline->diary_entries->size)
    {
      current_page++;
      refresh(questline, window);
    }
}
