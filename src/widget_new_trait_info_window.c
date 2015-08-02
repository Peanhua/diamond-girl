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

#include "cave.h"
#include "diamond_girl.h"
#include "font.h"
#include "gfx_glyph.h"
#include "globals.h"
#include "map.h"
#include "quest.h"
#include "sfx.h"
#include "traits.h"
#include "widget.h"
#include <assert.h>
#include <libintl.h>

static void on_buy_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_recycle_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_edit_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);

void widget_new_trait_info_window(trait_t trait, bool enable_controls)
{
  struct trait * traitdata;
  
  traitdata = trait_get(trait);
  if(traitdata != NULL)
    {
      char buf[128];
      struct widget * window;

      snprintf(buf, sizeof buf, "%s: %s", gettext("Trait"), trait_get_name(trait));
      window = widget_new_window(widget_root(), 420, 200, buf);
      assert(window != NULL);
      if(window != NULL)
        {
          int y;

          widget_set_modal(window);
          widget_set_image_pointer(window, "raw_image", gfx_image(traitdata->image_id));
          widget_set_widget_pointer(window, "previously_selected_object", widget_focus());

          y = 40 + font_height();

          char ** description;
          char fn[1024];
          
          snprintf(fn, sizeof fn, "data/trait-%s", traitdata->filename);
          description = read_localized_text_file(get_data_filename(fn));
          if(description != NULL)
            {
              for(int i = 0; description[i] != NULL; i++)
                {
                  widget_new_text(window, 10, y, description[i]);
                  free(description[i]);
                  y += font_height();
                }
              free(description);
            }

          y += font_height() * 2;

          struct widget * prev;
          
          prev = NULL;
          if(enable_controls)
            {
              if(traits_get_active() & trait)
                {
                  struct widget * t;

                  t = widget_new_text(window, 10, y, gettext("You have this trait."));
                  if(traits_get_active() & TRAIT_RECYCLER)
                    {
                      struct widget * b;

                      snprintf(buf + 1, (sizeof buf) - 1, gettext("Recycle for %lu"), (long unsigned) traitdata->cost * 90 / 100);
                      buf[0] = ' '; snprintf(buf + strlen(buf), sizeof buf - strlen(buf), " "); /* Pad with spaces */
                      b = widget_new_button(window, widget_x(t) + widget_width(t) + 10, y, buf);
                      widget_set_on_release(b, on_recycle_trait_clicked);
                      widget_set_ulong(b, "trait", trait);
                      y += widget_height(b);
                      prev = b;
                    }
                  y += font_height();

                  if(trait == TRAIT_EDIT)
                    {
                      struct widget * b;

                      y += font_height();
                      snprintf(buf, sizeof buf, " %s ", gettext("Edit cave"));
                      b = widget_new_button(window, 0, y, buf);
                      widget_set_on_release(b, on_edit_trait_clicked);
                      widget_center_horizontally(b);
                      y += widget_height(b);
                      if(prev != NULL)
                        widget_set_navigation_updown(prev, b);
                      prev = b;
                    }
              
                  y += font_height();
                }
              else if(traits_get_available() & trait)
                {
                  struct widget * t;
                  
                  t = widget_new_text(window, 10, y, gettext("You don't have this trait."));
                  if(traits_get_score() >= traitdata->cost)
                    {
                      struct widget * b;

                      snprintf(buf, sizeof buf, " %s ", gettext("Buy"));
                      b = widget_new_button(window, widget_x(t) + widget_width(t) + 10, y, buf);
                      widget_set_on_release(b, on_buy_trait_clicked);
                      widget_set_ulong(b, "trait", trait);
                      if(prev != NULL)
                        widget_set_navigation_updown(prev, b);
                      prev = b;
                    }
                  y += font_height();
                  
                  widget_new_text(window, 10, y, gettext("Cost:"));
                  snprintf(buf, sizeof buf, "%u", (unsigned int) traitdata->cost);
                  widget_new_text(window, 100, y, buf);
                  y += font_height();
                  
                  widget_new_text(window, 10, y, gettext("You have:"));
                  snprintf(buf, sizeof buf, "%u", (unsigned int) traits_get_score());
                  if( ! (traits_get_active() & trait) )
                    if(traits_get_score() < traitdata->cost)
                      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext(" [need %u more]"), (unsigned int) (traitdata->cost - traits_get_score()));
                  widget_new_text(window, 100, y, buf);
                  y += font_height();
                }
              else
                {
                  unsigned int level;
                  
                  level = traitdata->cave_level;
                  if(level == 0)
                    level = get_cave_level_count(traitdata->cave_name);
                  snprintf(buf, sizeof buf, gettext("Level required: %u"), level);
                  widget_new_text(window, 10, y, buf);
                  y += font_height();
                }

              y += font_height();
            }

          struct widget * closebutton;

          snprintf(buf, sizeof buf, "  %s  ", gettext("Close"));
          closebutton = widget_new_button(window, 0, y, buf);
          widget_set_x(closebutton, (widget_width(window) - widget_width(closebutton)) / 2);
          widget_set_focus(closebutton);
          y += widget_height(closebutton);
          if(prev != NULL)
            widget_set_navigation_updown(prev, closebutton);


          widget_resize_to_fit_children(window);
          widget_set_width(window, widget_width(window) + 10);
          widget_set_height(window, widget_height(window) + 10);
          widget_center(window);

          ui_wait_for_window_close(window, closebutton);
        }
    }
}







static void on_buy_trait_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  trait_t trait;
  struct widget * prev;

  prev = widget_get_widget_pointer(widget_parent(this), "previously_selected_object");
  trait = widget_get_ulong(this, "trait");
  widget_delete(widget_parent(this));

  assert(trait > 0);

  struct trait * traitdata;

  traitdata = trait_get(trait);
  if(traitdata != NULL)
    {
      unsigned int prev_active_questline;

      prev_active_questline = globals.active_questline;
      
      assert(traitdata->cost <= traits_get_score());
      if(prev != NULL)
        {
          struct image * img;

          img = gfx_image(traitdata->image_id);
          widget_set_image_pointer(prev, "raw_image", img);
        }
      traits_delete_score(traitdata->cost);
      traits_activate(trait);

      if(trait == TRAIT_QUESTS)
        if(globals.active_questline != prev_active_questline)
          {
            struct questline * ql;
              
            ql = globals.questlines[globals.active_questline];
            if(ql->current_quest == 0 && ql->current_phase == QUESTLINE_PHASE_OPENED)
              quest_action(ql, QUEST_ACTION_EXAMINE_NOTES);
          }
    }
}



static void on_recycle_trait_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  trait_t trait;
  struct widget * prev;

  prev = widget_get_widget_pointer(widget_parent(this), "previously_selected_object");
  trait = widget_get_ulong(this, "trait");
  widget_delete(widget_parent(this));

  if(trait > 0)
    {
      struct trait * traitdata;

      traitdata = trait_get(trait);
      assert(traits_get_active() & TRAIT_RECYCLER);
      if(traitdata != NULL)
        {
          if(prev != NULL)
            {
              struct image * img;

              img = gfx_image(traitdata->disabled_image_id);
              widget_set_image_pointer(prev, "raw_image", img);
            }
          traits_add_score(traitdata->cost * 90 / 100);
          traits_deactivate(trait);
        }
    }
}


static void on_edit_trait_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  sfx_music_stop();
  map_editor(cave_get(globals.title_game_mode, "user"), 1);
  sfx_music(MUSIC_TITLE, 1);
  gfx_frame0();
  ui_set_last_user_action(time(NULL));
}
