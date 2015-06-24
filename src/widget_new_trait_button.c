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
#include "font.h"
#include "widget.h"
#include "traits.h"
#include "gfx_image.h"
#include "gfx.h"
#include <assert.h>
#include <libintl.h>



static struct
{
  trait_t        trait;
  char *         filename;
  uint64_t       cost;
  enum GFX_IMAGE image_id;
  enum GFX_IMAGE disabled_image_id;
} traits[] =
  {
    { TRAIT_KEY,            "key",              2000, GFX_IMAGE_TRAIT_KEY,            GFX_IMAGE_TRAIT_KEY_DISABLED            },
    { TRAIT_ADVENTURE_MODE, "adventure_mode",   5000, GFX_IMAGE_TRAIT_ADVENTURE_MODE, GFX_IMAGE_TRAIT_ADVENTURE_MODE_DISABLED },
    { TRAIT_RIBBON,         "ribbon",          40000, GFX_IMAGE_TRAIT_RIBBON,         GFX_IMAGE_TRAIT_RIBBON_DISABLED         },
    { TRAIT_GREEDY,         "greedy",          40000, GFX_IMAGE_TRAIT_GREEDY,         GFX_IMAGE_TRAIT_GREEDY_DISABLED         },
    { TRAIT_TIME_CONTROL,   "time_control",   100000, GFX_IMAGE_TRAIT_TIME_CONTROL,   GFX_IMAGE_TRAIT_TIME_CONTROL_DISABLED   },
    { TRAIT_POWER_PUSH,     "power_push",      40000, GFX_IMAGE_TRAIT_POWER_PUSH,     GFX_IMAGE_TRAIT_POWER_PUSH_DISABLED     },
    { TRAIT_DIAMOND_PUSH,   "diamond_push",    40000, GFX_IMAGE_TRAIT_DIAMOND_PUSH,   GFX_IMAGE_TRAIT_DIAMOND_PUSH_DISABLED   },
    { TRAIT_RECYCLER,       "recycler",        40000, GFX_IMAGE_TRAIT_RECYCLER,       GFX_IMAGE_TRAIT_RECYCLER_DISABLED       },
    { TRAIT_STARS1,         "stars1",          20000, GFX_IMAGE_TRAIT_STARS1,         GFX_IMAGE_TRAIT_STARS1_DISABLED         },
    { TRAIT_STARS2,         "stars2",          40000, GFX_IMAGE_TRAIT_STARS2,         GFX_IMAGE_TRAIT_STARS2_DISABLED         },
    { TRAIT_STARS3,         "stars3",          60000, GFX_IMAGE_TRAIT_STARS3,         GFX_IMAGE_TRAIT_STARS3_DISABLED         },
    { TRAIT_CHAOS,          "chaos",           30000, GFX_IMAGE_TRAIT_CHAOS,          GFX_IMAGE_TRAIT_CHAOS_DISABLED          },
    { TRAIT_DYNAMITE,       "dynamite",       100000, GFX_IMAGE_TRAIT_DYNAMITE,       GFX_IMAGE_TRAIT_DYNAMITE_DISABLED       },
    { TRAIT_IRON_GIRL,      "iron_girl",       20000, GFX_IMAGE_TRAIT_IRON_GIRL,      GFX_IMAGE_TRAIT_IRON_GIRL_DISABLED      },
    { TRAIT_PYJAMA_PARTY,   "pyjama_party",     5000, GFX_IMAGE_TRAIT_PYJAMA_PARTY,   GFX_IMAGE_TRAIT_PYJAMA_PARTY_DISABLED   },
    { 0,                    NULL,                  0, GFX_IMAGE_SIZEOF_,              GFX_IMAGE_SIZEOF_                       }
  };

struct widget * traitinfo;

static void on_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_trait_info_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_buy_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_recycle_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_traitinfo_exit(bool pressed, SDL_Event * event);
static void on_traitinfo_unload(struct widget * this);

struct widget * widget_new_trait_button(struct widget * parent, int x, int y, int width, int height, trait_t trait, bool always_enabled_image, bool controls)
{
  struct widget * rv;

  rv = widget_new_button(parent, x, y, NULL);
  assert(rv != NULL);
  if(rv != NULL)
    {
      widget_set_width(rv,  width);
      widget_set_height(rv, height);
      widget_delete_flags(rv, WF_BACKGROUND | WF_CAST_SHADOW);

      int ind;

      ind = -1;
      for(int i = 0; ind == -1 && traits[i].trait != 0; i++)
        if(traits[i].trait == trait)
          ind = i;
      
      if(ind >= 0)
        {
          widget_set_on_release(rv, on_trait_clicked);
          widget_set_ulong(rv, "trait", traits[ind].trait);
          widget_set_ulong(rv, "controls", controls);

          struct image * img;

          if(always_enabled_image == true || traits_get_active() & traits[ind].trait)
            img = gfx_image(traits[ind].image_id);
          else
            img = gfx_image(traits[ind].disabled_image_id);
          
          if(img != NULL)
            {
              widget_add_flags(rv, WF_SCALE_RAW_IMAGE);
              widget_set_pointer(rv, "raw_image", img);
            }
        }
    }

  return rv;
}



static void on_trait_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  trait_t trait;
  bool controls;

  trait    = widget_get_ulong(this, "trait");
  controls = widget_get_ulong(this, "controls");

  if(trait > 0)
    {
      struct widget * win;

      int trait_ind;
      

      trait_ind = -1;
      for(int i = 0; traits[i].trait > 0 && trait_ind == -1; i++)
        if(traits[i].trait == trait)
          trait_ind = i;

      if(trait_ind != -1)
        {
          char buf[128];

          snprintf(buf, sizeof buf, "%s: %s", gettext("Trait"), trait_get_name(traits[trait_ind].trait));
          win = widget_new_window(widget_root(), 420, 200, buf);
          traitinfo = win;
          assert(win != NULL);
          if(win != NULL)
            {
              int y;

              widget_set_modal(win);
              widget_set_pointer(win, "raw_image", widget_get_pointer(this, "raw_image"));
              widget_set_pointer(win, "previously_selected_object", this);
              widget_set_on_unload(win, on_traitinfo_unload);

              ui_push_handlers();
              ui_set_handler(UIC_EXIT,   on_traitinfo_exit);
              ui_set_handler(UIC_CANCEL, on_traitinfo_exit);

              y = 40 + font_height();

              char ** description;
              char fn[1024];

              snprintf(fn, sizeof fn, "data/trait-%s", traits[trait_ind].filename);
              description = read_localized_text_file(get_data_filename(fn));
              if(description != NULL)
                {
                  for(int i = 0; description[i] != NULL; i++)
                    {
                      widget_new_text(win, 10, y, description[i]);
                      free(description[i]);
                      y += font_height();
                    }
                  free(description);
                }

              y += font_height() * 2;

              if(controls)
                {
                  if(traits_get_active() & trait)
                    {
                      struct widget * t;

                      t = widget_new_text(win, 10, y, gettext("You have this trait."));
                      if(traits_get_active() & TRAIT_RECYCLER)
                        {
                          struct widget * b;

                          snprintf(buf, sizeof buf, gettext(" Recycle for %lu "), (long unsigned) traits[trait_ind].cost * 90 / 100);
                          b = widget_new_button(win, widget_x(t) + widget_width(t) + 10, y, buf);
                          widget_set_on_release(b, on_recycle_trait_clicked);
                          widget_set_ulong(b, "trait", trait);
                        }
                      y += font_height();
                    }
                  else if(traits_get_available() & trait)
                    {
                      struct widget * t;
                  
                      t = widget_new_text(win, 10, y, gettext("You don't have this trait."));
                      if(traits_get_score() >= traits[trait_ind].cost)
                        {
                          struct widget * b;
                      
                          b = widget_new_button(win, widget_x(t) + widget_width(t) + 10, y, gettext(" Buy "));
                          widget_set_on_release(b, on_buy_trait_clicked);
                          widget_set_ulong(b, "trait", trait);
                        }
                      y += font_height();
                  
                      widget_new_text(win, 10, y, gettext("Cost:"));
                      snprintf(buf, sizeof buf, "%u", (unsigned int) traits[trait_ind].cost);
                      widget_new_text(win, 100, y, buf);
                      y += font_height();
                  
                      widget_new_text(win, 10, y, gettext("You have:"));
                      snprintf(buf, sizeof buf, "%u", (unsigned int) traits_get_score());
                      if( ! (traits_get_active() & trait) )
                        if(traits_get_score() < traits[trait_ind].cost)
                          snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext(" [need %u more]"), (unsigned int) (traits[trait_ind].cost - traits_get_score()));
                      widget_new_text(win, 100, y, buf);
                      y += font_height();
                    }
                  else
                    {
                      int level;

                      level = widget_get_ulong(this, "obtained_at_level");
                      snprintf(buf, sizeof buf, gettext("Level required: %d"), level);
                      widget_new_text(win, 10, y, buf);
                      y += font_height();
                    }
          
                  y += font_height();
                }

              struct widget * b;

              b = widget_new_button(win, 0, y, gettext("  Close  "));
              widget_set_x(b, (widget_width(win) - widget_width(b)) / 2);
              widget_set_on_release(b, on_trait_info_close_clicked);
              widget_set_focus(b);
              y += widget_height(b);


              widget_set_height(win, y + 10);
              widget_set_y(win, widget_height(widget_parent(win)) / 2 - widget_height(win) / 2);
            }
        }
    }
}

static void on_traitinfo_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * w;

      w = widget_get_pointer(traitinfo, "previously_selected_object");

      widget_delete(traitinfo);

      if(w != NULL)
        widget_set_focus(w);
    }
}

static void on_trait_info_close_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  on_traitinfo_exit(true, NULL);
}


static void on_buy_trait_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  trait_t trait;
  struct widget * prev;

  prev = widget_get_pointer(widget_parent(this), "previously_selected_object");
  trait = widget_get_ulong(this, "trait");
  widget_delete(widget_parent(this));

  if(trait > 0)
    {
      int ind;

      ind = -1;
      for(int i = 0; ind == -1 && traits[i].trait > 0; i++)
        if(traits[i].trait == trait)
          ind = i;

      assert(ind >= 0);
      assert(ind < 0 || traits[ind].cost <= traits_get_score());
      if(ind >= 0)
        {
          assert(prev != NULL);
          if(prev != NULL)
            {
              struct image * img;

              img = gfx_image(traits[ind].image_id);
              widget_set_pointer(prev, "raw_image", img);
            }
          traits_delete_score(traits[ind].cost);
          traits_activate(traits[ind].trait);
        }
    }
}


static void on_recycle_trait_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  trait_t trait;
  struct widget * prev;

  prev = widget_get_pointer(widget_parent(this), "previously_selected_object");
  trait = widget_get_ulong(this, "trait");
  widget_delete(widget_parent(this));

  if(trait > 0)
    {
      int ind;

      ind = -1;
      for(int i = 0; ind == -1 && traits[i].trait > 0; i++)
        if(traits[i].trait == trait)
          ind = i;

      assert(ind >= 0);
      assert(traits_get_active() & TRAIT_RECYCLER);
      if(ind >= 0)
        {
          assert(prev != NULL);
          if(prev != NULL)
            {
              struct image * img;

              img = gfx_image(traits[ind].disabled_image_id);
              widget_set_pointer(prev, "raw_image", img);
            }
          traits_add_score(traits[ind].cost * 90 / 100);
          traits_deactivate(traits[ind].trait);
        }
    }
}


static void on_traitinfo_unload(struct widget * this DG_UNUSED)
{
  ui_pop_handlers();
}
