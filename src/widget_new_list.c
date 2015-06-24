/*
  Diamond Girl - Game where player collects diamonds.
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
#include "stack.h"
#include "gfx.h"

static void on_item_release(struct widget * this, enum WIDGET_BUTTON button);
static void on_item_focus(struct widget * this);
static void on_scrollbar_clicked(struct widget * this, enum WIDGET_BUTTON button, int x, int y);
static void on_scrollbar_button_clicked(struct widget * this, enum WIDGET_BUTTON button, int x, int y);
static void on_scrollbar_button_release(struct widget * this, enum WIDGET_BUTTON button);
static void on_scrollbar_button_move(struct widget * this, int x, int y);
static void scroll_to(struct widget * scrollbar_button, int y);

struct widget * widget_new_list(struct widget * parent, int x, int y, int width, int height, struct stack * items)
{
  struct widget * rv;
  int scrollbar_width;
  int visible_items;

  {
    struct widget * tmp;

    tmp = widget_new_button(widget_root(), 0, 0, "x");
    visible_items = (height - 2) / widget_height(tmp);
    widget_delete(tmp);

    int item_count;

    item_count = 0;
    for(unsigned int i = 0; i < items->size; i++)
      if(items->data[i] != NULL)
        item_count++;

    if(item_count <= visible_items)
      scrollbar_width = 0;
    else
      scrollbar_width = 20;
  }

  rv = widget_new_child(parent, x, y, width, height);
  widget_set_ulong(rv, "type", WT_LIST);

  struct widget * container, * frame;

  container = widget_new_child(rv, 0, 0, width - scrollbar_width, height);
  widget_add_flags(container, WF_CLIPPING);

  frame = widget_new_frame(container, 0, 0, width - scrollbar_width, height);
  frame->z_ = rv->z_;
  widget_delete_flags(frame, WF_CAST_SHADOW | WF_DRAW_BORDERS | WF_BACKGROUND);

  struct widget * prev;

  y = 0;
  prev = NULL;
  for(unsigned int i = 0; i < items->size; i++)
    if(items->data[i] != NULL)
      {
        struct widget * obj;

        obj = widget_new_button(frame, 0, y, items->data[i]);
        widget_delete_flags(obj, WF_DRAW_BORDERS);
        widget_set_ulong(obj, "alpha", 0x80);
        widget_set_width(obj, widget_width(frame));
        widget_set_ulong(obj, "value", i);
        widget_set_pointer(obj, "list_object", rv);
        widget_set_on_release(obj, on_item_release);
        widget_set_on_focus(obj, on_item_focus);
        y += widget_height(obj);

        if(prev != NULL)
          widget_set_navigation_updown(prev, obj);
        prev = obj;
      }
  widget_set_height(frame, y);
  widget_set_pointer(rv, "focus_down_object", prev);

  if(scrollbar_width > 0)
    {
      struct widget * scrollbar, * scrollbar_button;
      int bheight;

      scrollbar = widget_new_frame(rv, width - scrollbar_width, 0, scrollbar_width, height);
      scrollbar->z_ = rv->z_;
      widget_delete_flags(scrollbar, WF_CAST_SHADOW);
      widget_set_on_activate_at(scrollbar, on_scrollbar_clicked);
      widget_set_enabled(scrollbar, true);
      widget_add_flags(scrollbar, WF_FOCUSABLE);
      
      scrollbar_button = widget_new_button(scrollbar, 1, 1, NULL);
      widget_set_width(scrollbar_button, scrollbar_width - 2);
      bheight = 0;
      if(items->size > 0)
        bheight += (height - 2) * visible_items / items->size;
      if(bheight > height - 2)
        bheight = height - 2;
      widget_set_height(scrollbar_button, bheight);
      widget_set_on_activate_at(scrollbar_button, on_scrollbar_button_clicked);
      widget_set_pointer(scrollbar_button, "frame", frame);
      widget_set_pointer(scrollbar, "scrollbar_button", scrollbar_button);
      widget_set_pointer(rv, "scrollbar_button", scrollbar_button);
    }

  return rv;
}


static void on_item_release(struct widget * this, enum WIDGET_BUTTON button)
{
  struct widget * list_object;
  ui_func_on_activate_t func;

  list_object = widget_get_pointer(this, "list_object");
  widget_set_ulong(list_object, "selected_index", widget_get_ulong(this, "value"));
  func = widget_on_release(list_object);
  if(func != NULL)
    func(list_object, button);
}

static void on_item_focus(struct widget * this)
{
  struct widget * list_object;
  struct widget * scrollbar_button;

  list_object = widget_get_pointer(this, "list_object");
  scrollbar_button = widget_get_pointer(list_object, "scrollbar_button");
  if(scrollbar_button != NULL)
    {
      int y, ay, list_y;
      int h, list_h;
      struct widget * scrollbar, * frame;

      scrollbar = widget_parent(scrollbar_button);
      frame = widget_get_pointer(scrollbar_button, "frame");

      list_y = widget_absolute_y(list_object);
      list_h = widget_height(list_object);
      y = widget_y(this);
      ay = widget_absolute_y(this);
      h = widget_height(this);
      if(ay < list_y)
        { /* This is above the top, scroll so that this is the top-most. */
          scroll_to(scrollbar_button, y * widget_height(scrollbar) / widget_height(frame));
        }
      else if(ay + h > list_y + list_h)
        { /* This is below the bottom, scroll so that this is the bottom-most. */
          scroll_to(scrollbar_button, (y - list_h + h) * widget_height(scrollbar) / (widget_height(frame) - h));
        }
    }
}


static void scroll_to(struct widget * scrollbar_button, int y)
{
  struct widget * scrollbar;
  int maxy;

  scrollbar = widget_parent(scrollbar_button);

  maxy = widget_height(scrollbar) - 1 - widget_height(scrollbar_button);
  if(y < 1)
    y = 1;
  else if(y > maxy)
    y = maxy;

  widget_set_y(scrollbar_button, y);

  y -= 1;
  maxy -= 1;
  if(maxy > 0)
    {
      struct widget * frame;
      int framey;

      frame = widget_get_pointer(scrollbar_button, "frame");
      framey = -(y * (widget_height(frame) - widget_height(scrollbar)) / maxy);
      widget_set_y(frame, framey);
    }
}


static void on_scrollbar_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED, int x DG_UNUSED, int y)
{
  struct widget * scrollbar_button;
  int sby, h;

  scrollbar_button = widget_get_pointer(this, "scrollbar_button");
  sby = widget_y(scrollbar_button);
  h = widget_height(scrollbar_button);
  if(y < sby)
    sby -= h;
  else
    sby += h;

  scroll_to(scrollbar_button, sby);
}


static void on_scrollbar_button_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED, int x DG_UNUSED, int y)
{
  struct widget * tmp;

  tmp = widget_new_child(widget_root(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  tmp->z_ = this->z_ + 1;
  widget_set_pointer(tmp, "scrollbar_button", this);
  widget_set_long(tmp, "click_offset_y", y);
  widget_set_enabled(tmp, true);
  widget_set_on_release(tmp, on_scrollbar_button_release);
  widget_set_on_mouse_move(tmp, on_scrollbar_button_move);
  widget_set_focus(tmp);
  widget_set_activated(tmp);
}

static void on_scrollbar_button_release(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  widget_delete(this);
}

static void on_scrollbar_button_move(struct widget * this, int x DG_UNUSED, int y)
{
  struct widget * scrollbar;
  struct widget * scrollbar_button;

  scrollbar_button = widget_get_pointer(this, "scrollbar_button");
  scrollbar = widget_parent(scrollbar_button);

  y -= widget_get_long(this, "click_offset_y");
  y -= widget_absolute_y(scrollbar);

  scroll_to(scrollbar_button, y);
}
