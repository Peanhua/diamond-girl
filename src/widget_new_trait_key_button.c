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
#include "widget.h"
#include "traits.h"
#include "gfx_image.h"
#include "cave.h"
#include "gfx.h"
#include "twinkle.h"
#include <assert.h>
#include <libintl.h>


static void on_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_unload(struct widget * this);
static void on_window_exit(bool pressed, SDL_Event * event);
static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_unlock_clicked(struct widget * this, enum WIDGET_BUTTON button);

static uint64_t get_cost(struct cave * cave, unsigned int level);

static struct widget * window;



struct widget * widget_new_trait_key_button(struct widget * parent, int x, int y, int width, int height, struct cave * cave, unsigned int level)
{
  struct widget * rv;

  rv = widget_new_trait_button(parent, x, y, width, height, TRAIT_KEY, true, false);
  assert(rv != NULL);
  widget_set_on_release(rv, on_clicked);
  widget_set_cave_pointer(rv, "cave", cave);
  widget_set_ulong(rv, "level", level);

  return rv;
}

static void on_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  uint64_t score, score_needed;
  struct cave * cave;
  unsigned int level;

  cave = widget_get_cave_pointer(this, "cave");
  level = widget_get_ulong(this, "level");

  score = traits_get_score();
  score_needed = get_cost(cave, level);

  window = widget_new_window(widget_root(), 420, 200, gettext("Unlock level"));
  widget_set_image(window, "image", GFX_IMAGE_TRAIT_KEY);
  widget_set_widget_pointer(window, "previously_selected_object", this);
  widget_set_on_unload(window, on_window_unload);

  int y;
  struct widget * obj, * unlock_obj;
  char buf[128];

  y = 30;

  obj = widget_new_text(window, 0, y, cave_displayname(cave->name));
  widget_set_x(obj, (widget_width(window) - widget_width(obj)) / 2);
  y += widget_height(obj);

  snprintf(buf, sizeof buf, gettext("Level: %u"), level);
  obj = widget_new_text(window, 0, y, buf);
  widget_set_x(obj, (widget_width(window) - widget_width(obj)) / 2);
  y += widget_height(obj);

  snprintf(buf, sizeof buf, "%s %lu", gettext("Cost:"), (unsigned long) score_needed);
  obj = widget_new_text(window, 0, y, buf);
  widget_set_x(obj, (widget_width(window) - widget_width(obj)) / 2);
  y += widget_height(obj);

  snprintf(buf, sizeof buf, gettext("You have: %lu"), (unsigned long) score);
  obj = widget_new_text(window, 0, y, buf);
  widget_set_x(obj, (widget_width(window) - widget_width(obj)) / 2);
  y += widget_height(obj);

  y += 20;

  if(score >= score_needed)
    {
      snprintf(buf, sizeof buf, " %s ", gettext("Unlock this level"));
      unlock_obj = widget_new_button(window, 0, y, buf);
      widget_set_x(unlock_obj, (widget_width(window) - widget_width(unlock_obj)) / 2);
      widget_set_cave_pointer(unlock_obj, "cave", cave);
      widget_set_ulong(unlock_obj,   "level",     level);
      widget_set_on_release(unlock_obj, on_unlock_clicked);
      y += widget_height(unlock_obj);
    }
  else
    {
      unlock_obj = NULL;
      obj = widget_new_text(window, 0, y, gettext("You don't have enough to unlock this level."));
      widget_set_x(obj, (widget_width(window) - widget_width(obj)) / 2);
      y += widget_height(obj);
    }

  y += 20;
  snprintf(buf, sizeof buf, "  %s  ", gettext("Cancel"));
  obj = widget_new_button(window, 0, y, buf);
  widget_set_x(obj, (widget_width(window) - widget_width(obj)) / 2);
  widget_set_on_release(obj, on_window_close_clicked);
  if(unlock_obj != NULL)
    widget_set_navigation_updown(unlock_obj, obj);
  widget_set_focus(obj);
  y += widget_height(obj);
  y += 10;
  

  widget_set_height(window, y);


  widget_set_modal(window);
  ui_push_handlers();
  ui_set_handler(UIC_EXIT,   on_window_exit);
  ui_set_handler(UIC_CANCEL, on_window_exit);
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

      w = widget_get_widget_pointer(window, "previously_selected_object");

      widget_delete(window);

      if(w != NULL)
        widget_set_focus(w);
    }
}

static void on_window_close_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  on_window_exit(true, NULL);
}


static void on_unlock_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{ /* Note that we are not making new traits available in here.
   * This is on purpose, to force the player to play at least one level.
   */
  uint64_t cost;
  struct cave * cave;
  unsigned int level;

  cave = widget_get_cave_pointer(this, "cave");
  level = widget_get_ulong(this, "level");

  cost = get_cost(cave, level);
  traits_delete_score(cost);
  set_max_level_selection(level + 1);

  twinkle_area(widget_absolute_x(window),
               widget_absolute_y(window),
               widget_width(window),
               widget_height(window),
               100);

  on_window_exit(true, NULL);
}



static uint64_t get_cost(struct cave * cave, unsigned int level)
{
  uint64_t cost;
  int perc;

  perc = 100 * level / cave->level_count;
  cost = perc * 1000;

  return cost;
}
