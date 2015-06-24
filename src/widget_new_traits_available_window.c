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
#include <assert.h>
#include <libintl.h>

static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_exit(bool pressed, SDL_Event * event);
static void on_window_unload(struct widget * this);

static struct widget * window;


struct widget * widget_new_traits_available_window(trait_t new_traits)
{
  trait_t traits[] =
    {
      TRAIT_KEY,
      TRAIT_ADVENTURE_MODE,
      TRAIT_RIBBON,
      TRAIT_GREEDY,
      TRAIT_TIME_CONTROL,
      TRAIT_POWER_PUSH,
      TRAIT_DIAMOND_PUSH,
      TRAIT_RECYCLER,
      TRAIT_STARS1,
      TRAIT_STARS2,
      TRAIT_STARS3,
      TRAIT_CHAOS,
      TRAIT_DYNAMITE,
      TRAIT_IRON_GIRL,
      TRAIT_PYJAMA_PARTY,

      TRAIT_SIZEOF_
    };
  int width;
  const int spacing = 50;

  assert(new_traits != 0);

  width = 0;
  for(int i = 0; traits[i] != TRAIT_SIZEOF_; i++)
    if(new_traits & traits[i])
      width += spacing;
  if(width < 220)
    width = 220;

  window = widget_new_window(widget_root(), 10 + width + 10, 25 + 50 + 10, gettext("New traits available!"));
  if(window != NULL)
    {
      struct widget * prev;
      int x, y;

      widget_set_modal(window);
      widget_set_on_unload(window, on_window_unload);

      ui_push_handlers();
      ui_set_navigation_handlers();
      ui_set_common_handlers();
      ui_set_handler(UIC_EXIT,   on_window_exit);
      ui_set_handler(UIC_CANCEL, on_window_exit);

      x = 10;
      y = 25;

      prev = NULL;
      for(int i = 0; traits[i] != TRAIT_SIZEOF_; i++)
        if(new_traits & traits[i])
          {
            struct widget * b;

            b = widget_new_trait_button(window, x, y, spacing - 10 + 2, spacing - 10 + 2, traits[i], true, false);
            if(b != NULL)
              {
                if(prev != NULL)
                  widget_set_navigation_leftright(prev, b);
                prev = b;
              }
            x += spacing;
          }

      struct widget * b;

      if(prev != NULL)
        y += widget_height(prev);

      y += 20;
      b = widget_new_button(window, x, y, gettext(" Close "));
      widget_set_x(b, (widget_width(window) - widget_width(b)) / 2);
      widget_set_on_release(b, on_window_close_clicked);
      widget_set_focus(b);
      if(prev != NULL)
        widget_set_navigation_updown(prev, b);
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
