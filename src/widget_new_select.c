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
#include "image.h"
#include "ui.h"
#include "stack.h"
#include "gfx.h"
#include "gfx_image.h"

#include <assert.h>

static struct widget * active_select;
static struct widget * background;

static void on_open_list(struct widget * this, enum WIDGET_BUTTON button);
static void on_background_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_option_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void cut_option_short(char const * source, char * destination, unsigned int destination_size, unsigned int maxwidth);

struct widget * widget_new_select(struct widget * parent, int x, int y, int width, unsigned int selected_index, struct stack * options)
{
  struct widget * widget;

  widget = widget_new_text(parent, x, y, "");
  assert(widget != NULL);
  if(widget != NULL)
    {
      widget_set_ulong(widget, "type", WT_SELECT);
      widget_set_enabled(widget, true);
      widget_set_width(widget, width);
      if(options != NULL && selected_index < options->size)
        {
          char buf[128];

          cut_option_short(options->data[selected_index], buf, sizeof buf, width - 2);
          widget_set_string(widget, "text", "%s", buf);
        }
      widget_set_ulong(widget, "selected_index", selected_index);
      widget_set_select_options(widget, options);
      widget_set_on_release(widget, on_open_list);
      widget_set_image(widget, "image", GFX_IMAGE_WIDGET_SELECT);
      widget_set_long(widget, "image_offset_x", -1);
      widget_add_flags(widget, WF_ALIGN_IMAGE_RIGHT | WF_DRAW_BORDERS | WF_BACKGROUND);
    }

  return widget;
}



static void on_open_list(struct widget * this, enum WIDGET_BUTTON button)
{
  if(widget_enabled(this) == true && button == WIDGET_BUTTON_LEFT)
    {
      struct stack * options;

      options = widget_get_select_options(this);
      if(options != NULL)
        {
          active_select = this;
          background = widget_new_child(this, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
          assert(background != NULL);
          if(background != NULL)
            {
              background->z_ += 1;
              
              widget_set_x(background, -widget_absolute_x(this));
              widget_set_y(background, -widget_absolute_y(this));
              
              widget_set_enabled(background, true);
              widget_set_on_release(background, on_background_clicked);

              struct widget * win;
              struct widget * previous_option;
              int selected;
              int y, w;

              selected = widget_get_ulong(this, "selected_index");
              {
                struct widget * tmp;
                
                tmp = widget_new_button(this, 0, 0, "x");
                y = widget_absolute_y(this) - 1;
                y -= widget_height(tmp) * selected;
                if(y < 0)
                  y = 0;
                widget_delete(tmp);
              }

              win = widget_new_frame(background,
                                     widget_absolute_x(this),
                                     y,
                                     widget_width(this),
                                     options->size * widget_height(this));
              w = widget_width(this) - 2;
              y = 1;
              previous_option = NULL;
              for(unsigned int i = 0; i < options->size; i++)
                {
                  struct widget * option;
                  char buf[128];
                  char * text;

                  text = ((struct ui_widget_select_option *) options->data[i])->text;
                  cut_option_short(text, buf, sizeof buf, w);
                  option = widget_new_button(win, 1, y, buf);
                  assert(option != NULL);
                  if(option != NULL)
                    {
                      widget_set_ulong(option, "option_index", i);
                      widget_set_width(option, w);
                      y += widget_height(option);
                      widget_set_on_release(option, on_option_clicked);
                      widget_delete_flags(option , WF_ALIGN_CENTER);

                      if(previous_option != NULL)
                        widget_set_navigation_updown(previous_option, option);

                      previous_option = option;

                      if(i == 0 || (int) i == selected)
                        widget_set_focus(option);
                    }
                }
            }
        }
    }
}

static void on_option_clicked(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct widget * w;

      widget_set_ulong(active_select, "selected_index", widget_get_ulong(this, "option_index"));
      widget_set_string(active_select, "text", "%s", widget_get_string(this, "text"));

      ui_func_on_select_t on_select;

      on_select = widget_get_pointer(active_select, "on_select", 'P');
      if(on_select != NULL)
        on_select(active_select);

      if(this->deleted_ == false)
        {
          w = background;
          background = NULL;
          widget_delete(w);

          widget_set_focus(active_select);
        }
    }
}


static void on_background_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct widget * w;

      w = background;
      background = NULL;
      widget_delete(w);
    }
}


static void cut_option_short(char const * source, char * destination, unsigned int destination_size, unsigned int maxwidth)
{
  bool done;
  
  done = false;
  for(unsigned int j = 0; done == false && j + 1 < destination_size; j++)
    {
      destination[j] = source[j];
      destination[j + 1] = '\0';
      if(font_width(destination) >= (int) maxwidth)
        {
          destination[j] = '\0';
          done = true;
        }
      else if(source[j] == '\0')
        done = true;
    }
}
