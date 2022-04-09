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
#include "font.h"
#include "widget.h"
#include "globals.h"
#include "girl.h"
#include "highscore.h"
#include "random.h"
#include "gfx.h"
#include "cave.h"
#include <assert.h>
#include <libintl.h>

static void on_invite_more_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_invite_more_no_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_invite_more_yes_clicked(struct widget * this, enum WIDGET_BUTTON button);

static void on_girl_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_caving_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_exit(bool pressed, SDL_Event * event);
static void on_window_unload(struct widget * this);
static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void refresh(struct widget * partystatus_widget);


static struct widget * window;


struct widget * widget_new_title_partystatus(struct widget * parent, int x, int y)
{
  struct widget * rv;

  window = NULL;

  rv = widget_new_frame(parent, x, y, 400, 411);
  widget_delete_flags(rv, WF_CAST_SHADOW | WF_DRAW_BORDERS | WF_BACKGROUND);

  refresh(rv);

  return rv;
}


static void refresh(struct widget * partystatus_widget)
{
  struct widget * frame, * obj;
  char buf[128];
  struct widget * invite_button;
  struct widget * save_nav;
  struct cave * cave;

  cave = cave_get(globals.title_game_mode, globals.cave_selection);

  frame = widget_get_widget_pointer(partystatus_widget, "contents");
  save_nav = NULL;
  if(frame != NULL)
    {
      invite_button = widget_get_widget_pointer(frame, "invite_button");
      if(invite_button != NULL)
        save_nav = invite_button->navigation_down_;
      widget_delete(frame);
    }

  frame = widget_new_child(partystatus_widget, 0, 0, widget_width(partystatus_widget), widget_height(partystatus_widget));
  widget_set_widget_pointer(partystatus_widget, "contents", frame);

  int x, x2, y;
  int cost;
  struct
  {
    char *        label;
    unsigned long value;
  } statlines[] =
      {
        { NULL, cave->highscores->total.score              },
        { NULL, cave->highscores->total.caves_entered      },
        { NULL, cave->highscores->total.levels_completed   },
        { NULL, cave->highscores->total.diamonds_collected },
        { NULL, 0                                          }
      };
  
  x = 10;
  y = 10;


  statlines[0].label = gettext("Score:");
  statlines[1].label = gettext("Caves entered:");
  statlines[2].label = gettext("Levels completed:");
  statlines[3].label = gettext("Diamonds collected:");
  
  obj = widget_new_text(frame, x, y, gettext("Pyjama party status:"));
  y += widget_height(obj);

  x2 = 0;
  for(int i = 0; statlines[i].label != NULL; i++)
    {
      int tmp;

      tmp = font_width(statlines[i].label);
      if(tmp > x2)
        x2 = tmp;
    }
  x2 += x + 10;

  for(int i = 0; statlines[i].label != NULL; i++)
    {
      obj = widget_new_text(frame, 10 + x, y, statlines[i].label);
      snprintf(buf, sizeof buf, "%lu", (long unsigned) statlines[i].value);
      obj = widget_new_text(frame, 10 + x2, y, buf);
      y += widget_height(obj);
    }


  int fheight;

  y += 20;
  x2 = x + 175 + 10;

  snprintf(buf, sizeof buf, "%s %d", gettext("Girls:"), globals.pyjama_party_girls_size);
  obj = widget_new_text(frame, x, y, buf);
  obj = widget_new_text(frame, x2, y, gettext("Cavings:"));
  y += widget_height(obj);

  cost = (1 + globals.pyjama_party_girls_size) * 500;
  invite_button = widget_new_button(frame, x, 0, gettext("Invite more"));
  widget_set_widget_pointer(partystatus_widget, "focus_down_object", invite_button);
  widget_set_widget_pointer(frame, "invite_button", invite_button);
  widget_set_width(invite_button, 175);
  widget_set_ulong(invite_button, "cost", cost);
  widget_set_on_release(invite_button, on_invite_more_clicked);
  if(cost > (int) traits_get_score())
    widget_set_enabled(invite_button, false);
  if(save_nav != NULL)
    widget_set_navigation_updown(invite_button, save_nav);

  struct stack * items;
  struct widget * fd;

  fheight = widget_height(frame) - y - widget_height(invite_button);

  items = stack_new();
  for(int i = 0; i < globals.pyjama_party_girls_size; i++)
    stack_push(items, globals.pyjama_party_girls[i]->name);
  obj = widget_new_list(frame, x, y, 175, fheight, items);
  widget_set_on_release(obj, on_girl_clicked);
  fd = widget_get_widget_pointer(obj, "focus_down_object");
  if(fd != NULL)
    widget_set_navigation_updown(fd, invite_button);

  items = stack_new();
  for(unsigned int i = 0; i < cave->highscores->highscores_size; i++)
    {
      struct tm * tm;
      struct highscore_entry * entry;

      entry = cave->highscores->highscores[i];
      tm = localtime(&entry->timestamp);
      strftime(buf, sizeof buf, gettext("%Y-%m-%d %H:%M"), tm);
      stack_push(items, strdup(buf));
    }
  obj = widget_new_list(frame, x2, y, 180, fheight, items);
  widget_set_on_release(obj, on_caving_clicked);

  y += fheight;

  widget_set_y(invite_button, y);
  y += widget_height(invite_button);
}


static void on_invite_more_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  char buf[128];

  if(globals.pyjama_party_girls_size >= MAX_PYJAMA_PARTY_SIZE)
    {
      widget_set_enabled(this, false);
      snprintf(buf, sizeof buf, gettext("You have %d girls in the party.\nThat is the maximum number."), (int) globals.pyjama_party_girls_size);
      widget_new_message_window(gettext("Error"), NULL, buf);
    }
  else
    {
      struct widget * obj, * t1, * t2, * b1, * b2;
      int x, y;
      unsigned long cost;

      cost = widget_get_ulong(this, "cost");

      window = widget_new_window(widget_root(), 450, 50, gettext("Invite a girl to the party?"));
      widget_set_widget_pointer(window, "previously_selected_object", widget_focus());
      x = 10;
      y = 30;

      snprintf(buf, sizeof buf, gettext("The maximum size of a party is %d girls."), (int) MAX_PYJAMA_PARTY_SIZE);
      obj = widget_new_text(window, x, y, buf);
      y += widget_height(obj);

      obj = widget_new_text(window, x, y, gettext("The party can only grow."));
      y += widget_height(obj);

      y += 10;

      snprintf(buf, sizeof buf, gettext("Inviting another girl to the party costs %lu."), cost);
      t1 = widget_new_text(window, x, y, buf);
      y += widget_height(t1);

      snprintf(buf, sizeof buf, gettext("You have %lu."), (unsigned long) traits_get_score());
      t2 = widget_new_text(window, x, y, buf);
      y += widget_height(t2) + 20;

      snprintf(buf, sizeof buf, " %s ", gettext("Cancel"));
      b1 = widget_new_button(window, 0, y, buf);
      widget_set_on_release(b1, on_invite_more_no_clicked);
      widget_set_focus(b1);
      x = widget_width(t1) / 2;
      widget_set_x(b1, x - widget_width(b1) - 5);

      snprintf(buf, sizeof buf, " %s ", gettext("Invite"));
      b2 = widget_new_button(window, x + 5, y, buf);
      widget_set_ulong(b2, "cost", cost);
      widget_set_widget_pointer(b2, "list_object", widget_parent(widget_parent(this)));
      widget_set_on_release(b2, on_invite_more_yes_clicked);
      y += widget_height(b2);
      widget_set_navigation_leftright(b1, b2);

      widget_set_height(window, y + 10);
      widget_set_modal(window);
      ui_push_handlers();
      ui_set_handler(UIC_EXIT,   on_window_exit);
      ui_set_handler(UIC_CANCEL, on_window_exit);
      widget_set_on_unload(window, on_window_unload);
    }
}

static void on_invite_more_no_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  widget_delete(widget_parent(this));
}

static void on_invite_more_yes_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct girl ** tmp;

  tmp = realloc(globals.pyjama_party_girls, sizeof(struct girl *) * (globals.pyjama_party_girls_size + 1));
  assert(tmp != NULL);
  if(tmp != NULL)
    {
      struct girl * girl;
      trait_t traits[6] =
        {
          TRAIT_RIBBON,
          TRAIT_GREEDY,
          TRAIT_POWER_PUSH,
          TRAIT_DIAMOND_PUSH,
          TRAIT_CHAOS,
          TRAIT_DYNAMITE
        };
      int n;

      /* globals.pyjama_party_girls is set here already, because it is used in girl_new() */
      globals.pyjama_party_girls = tmp;

      girl = girl_new();
      n = 1 + get_rand(4);
      for(int i = 0; i < n; i++)
        girl->possible_traits |= traits[get_rand(6)];
      girl->diamonds_to_trait = 15000 + get_rand(10000);
      
      globals.pyjama_party_girls[globals.pyjama_party_girls_size] = girl;
      globals.pyjama_party_girls_size++;

      traits_delete_score(widget_get_ulong(this, "cost"));

      struct widget * list_object;

      list_object = widget_get_widget_pointer(this, "list_object");
      if(list_object != NULL)
        {
          refresh(list_object);

          /* Set prev.. to invite_button, because we know that this window was opened using it. */
          struct widget * frame, * invite_button;

          frame = widget_get_widget_pointer(list_object, "contents");
          invite_button = widget_get_widget_pointer(frame, "invite_button");
          widget_set_widget_pointer(window, "previously_selected_object", invite_button);
        }
    }

  widget_delete(widget_parent(this));
}

static void on_girl_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct widget * obj;
  struct girl * girl;
  int girl_index;
  int x1, x2, y;
  char buf[128];
    
  girl_index = widget_get_ulong(this, "selected_index");
  assert(girl_index < globals.pyjama_party_girls_size);
  girl = globals.pyjama_party_girls[girl_index];
  window = widget_new_window(widget_root(), 350, 200, girl->name);
  widget_set_on_unload(window, on_window_unload);
  widget_set_widget_pointer(window, "previously_selected_object", widget_focus());

  x1 = 10;
  x2 = x1 + 100;
  y = 20;
  
  obj = widget_new_text(window, x1, y, gettext("Name:"));
  obj = widget_new_text(window, x2, y, girl->name);
  y += widget_height(obj);

  {
    struct tm * tm;

    tm = localtime(&girl->birth_time);
    strftime(buf, sizeof buf, gettext("%Y-%m-%d %H:%M"), tm);
    obj = widget_new_text(window, x1, y, "Invited:");
    obj = widget_new_text(window, x2, y, buf);
    y += widget_height(obj);
  }

  obj = widget_new_text(window, x1, y, gettext("Diamonds:"));
  if(girl->diamonds < girl->diamonds_to_trait)
    {
      uint64_t di, dr;
  
      di = girl->diamonds * 100 / girl->diamonds_to_trait;
      dr = (girl->diamonds * 100 * 1000 / girl->diamonds_to_trait) % 1000;
      snprintf(buf, sizeof buf, "%lu/%lu  %2d.%03d%%", (unsigned long) girl->diamonds, (unsigned long) girl->diamonds_to_trait, (int) di, (int) dr);
    }
  else
    snprintf(buf, sizeof buf, "%lu", (unsigned long) girl->diamonds);
  obj = widget_new_text(window, x2, y, buf);
  y += widget_height(obj);

  if(girl->traits)
    {
      int xpos, size, padding;

      widget_new_text(window, x1, y, gettext("Traits:"));
      
      xpos = 0;
      size = 24;
      padding = 2;
      for(trait_t trait = 0; trait < TRAIT_SIZEOF_; trait++)
        if(girl->traits & (1 << trait))
          {
            char * name;
          
            obj = widget_new_trait_button(window, x2 + xpos * (size + padding), y, size, size, (1<<trait), true, false);
            name = trait_get_name(1<<trait);
            if(name != NULL)
              widget_set_tooltip(obj, name);

            xpos++;
            if(xpos >= (220 / (size + padding)))
              {
                xpos = 0;
                y += size + padding;
              }
          }
    
      if(xpos > 0)
        y += size + padding;
    }

  y += 20;

  snprintf(buf, sizeof buf, "  %s  ", gettext("Close"));
  obj = widget_new_button(window, x1, y, buf);
  widget_set_on_release(obj, on_window_close_clicked);
  widget_set_focus(obj);
  widget_set_x(obj, widget_width(window) / 2 - widget_width(obj) / 2);
  y += widget_height(obj);

  widget_set_height(window, y + 10);
  widget_set_modal(window);
  ui_push_handlers();
  ui_set_handler(UIC_EXIT,   on_window_exit);
  ui_set_handler(UIC_CANCEL, on_window_exit);
}


static void on_caving_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct highscore_entry * caving;
  unsigned int caving_index;
  struct cave * cave;

  caving_index = widget_get_ulong(this, "selected_index");
  cave = cave_get(globals.title_game_mode, globals.cave_selection);
  assert(caving_index < cave->highscores->highscores_size);
  caving = cave->highscores->highscores[caving_index];
  widget_new_highscore_einfo(widget_root(), caving);
}


static void on_window_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    window = widget_delete(window);
}

static void on_window_unload(struct widget * this DG_UNUSED)
{
  struct widget * w;
  
  w = widget_get_widget_pointer(window, "previously_selected_object");
  if(w != NULL)
    widget_set_focus(w);
  
  window = NULL;

  ui_pop_handlers();
}

static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  widget_delete(widget_parent(this));
}
