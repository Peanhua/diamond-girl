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

#include "event.h"
#include "font.h"
#include "globals.h"
#include "quest.h"
#include "stack.h"
#include "trader.h"
#include "treasure.h"
#include "widget.h"
#include <assert.h>
#include <libintl.h>

static struct widget * window;
static struct widget * info;
static struct widget * sell;
static struct widget * list;
static struct widget * select_questline;

static struct stack * quests;

static struct trader * active_trader;

static void on_list_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_questline_selected(struct widget * this);
static void on_sell_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_sell_sell_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void add_items_to_list(struct stack * items, struct questline * questline);
static void refresh_item_list(struct questline * questline);
static void refresh_item(unsigned int selected);
static void link_select_questline_and_list(void);

void quest_treasures(struct questline * questline, struct trader * trader)
{
  quests = NULL;
  active_trader = trader;
  info = NULL;
  sell = NULL;
  list = NULL;
  select_questline = NULL;
  window = widget_new_window(NULL, 5 * 3 + 350 + 350, 500, gettext("My collection"));
  if(window != NULL)
    {
      widget_set_y(window, 70);

      refresh_item_list(questline);
      refresh_item(0);

      { /* Select list for questlines. */
        struct widget * label;

        label = widget_new_text(window, 5, widget_y(list) + widget_height(list) + font_height(), gettext("Questline:"));
        
        
        struct stack * qlines;
        int ind, item_index;

        item_index = 0;
        qlines = stack_new();
        {
          struct ui_widget_select_option * option;
          
          option = widget_new_select_option(NULL, gettext("All"));
          stack_push(qlines, option);
        }
        ind = 0;
        for(unsigned int i = 0; i < globals.questlines_size; i++)
          {
            struct questline * ql;

            ql = globals.questlines[i];
            if(questline_treasure_get_collected_count(ql) > 0)
              {
                struct ui_widget_select_option * option;
                
                option = widget_new_select_option(ql, questline_name(ql));
                stack_push(qlines, option);

                ind++;
                if(ql == questline)
                  item_index = ind;
              }
          }

        select_questline = widget_new_select(window, widget_x(label) + widget_width(label) + 5, widget_y(label), 350, item_index, qlines);
        if(select_questline != NULL)
          widget_set_pointer(select_questline, "on_select", 'P', on_questline_selected);

        if(sell != NULL)
          {
            widget_set_navigation_down(sell, select_questline);
            widget_set_navigation_right(select_questline, sell);
          }
        
        link_select_questline_and_list();
      }
      

      struct widget * closebutton;
      char buf[64];

      snprintf(buf, sizeof buf, "  %s  ", gettext("Close"));
      closebutton = widget_new_button(window, 0, widget_y(select_questline) + widget_height(select_questline) + font_height(), buf);
      widget_set_x(closebutton, (widget_width(window) - widget_width(closebutton)) / 2);
      widget_set_navigation_updown(select_questline, closebutton);

      
      widget_set_widget_pointer(window, "previously_selected_object", widget_focus());
      widget_set_modal(window);
      widget_set_focus(closebutton);
      
      ui_wait_for_window_close(window, closebutton);
    }
}


static void on_list_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  unsigned int item_index;

  item_index = widget_get_ulong(this, "selected_index");
  refresh_item(item_index);
}


static void on_questline_selected(struct widget * this)
{
  unsigned int item_index;

  item_index = widget_get_ulong(this, "selected_index");
  assert(item_index <= globals.questlines_size);
  if(item_index <= globals.questlines_size)
    {
      struct stack * options;
      struct ui_widget_select_option * o;
      struct questline * ql;
      
      options = widget_get_select_options(this);
      o = options->data[item_index];
      ql = o->data;
      refresh_item_list(ql);
      refresh_item(0);
    }
}


static void add_items_to_list(struct stack * items, struct questline * questline)
{
  for(unsigned int i = 0; i <= questline->current_quest; i++)
    {
      struct quest * quest;

      quest = questline->quests->data[i];
      if(quest->completed > 0 && quest->treasure_sold == false)
        {
          char buf[1024];
          
          snprintf(buf, sizeof buf, "%s %s '%s'",
                   treasure_material_name(quest->treasure_object->material),
                   treasure_type_name(quest->treasure_object->type),
                   quest->treasure_object->name);
          buf[0] = toupper(buf[0]);
          stack_push(items, strdup(buf)); // todo: fix memory leak
          stack_push(quests, quest);
        }
    }
}


static void refresh_item_list(struct questline * questline)
{
  if(list != NULL)
    widget_delete(list);
  
  struct stack * items;

  if(quests != NULL)
    stack_free(quests);
  quests = stack_new();

  items = stack_new();

  if(questline != NULL)
    add_items_to_list(items, questline);
  else
    for(unsigned int i = 0; i < globals.questlines_size; i++)
      add_items_to_list(items, globals.questlines[i]);

  list = widget_new_list(window, 5, font_height() + 5, 350, widget_height(window) - font_height() * 6 - 5, items);
  widget_set_on_release(list, on_list_clicked);
  widget_set_questline_pointer(list, "questline", questline);

  link_select_questline_and_list();
}

static void link_select_questline_and_list(void)
{
  if(select_questline != NULL)
    {
      struct widget * fd;
      
      fd = widget_get_widget_pointer(list, "focus_down_object");
      if(fd != NULL)
        widget_set_navigation_updown(fd, select_questline);
    }
}


static void refresh_item(unsigned int selected)
{
  if(list != NULL)
    widget_list_set(list, selected);
  
  if(info != NULL)
    info = widget_delete(info);
  if(sell != NULL)
    sell = widget_delete(sell);

  if(selected < quests->size)
    {
      char buf[128];
      
      info = widget_new_quest_treasure_info(window, 5 + 350 + 5, font_height() + 5, quests->data[selected], true);

      snprintf(buf, sizeof buf, " %s ", gettext("Sell to"));
      if(active_trader == NULL)
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext("local market"));
      else
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "%s", active_trader->name);
      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), " ");

      sell = widget_new_button(window, widget_x(info), widget_y(info) + widget_height(info) + 5, buf);
      
      widget_set_on_release(sell, on_sell_clicked);
      widget_set_pointer(sell, "quest", 'P', quests->data[selected]);

      struct widget * fd;
      
      fd = widget_get_widget_pointer(list, "focus_down_object");
      if(fd != NULL)
        widget_set_navigation_left(sell, fd);
      widget_list_set_item_navigation_right(list, sell);
      
      if(select_questline != NULL)
        {
          widget_set_navigation_down(sell, select_questline);
          widget_set_navigation_right(select_questline, sell);
        }
    }
}


static void on_sell_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct quest * quest;
  double price;

  quest = widget_get_pointer(this, "quest", 'P');
  assert(quest->treasure_sold == false);

  price = treasure_price(quest->treasure_object);

  if(active_trader != NULL)
    price = price * (1.0 - (double) active_trader->cost_modifier / 100.0);
                    
  if(traits_get(GAME_MODE_ADVENTURE) & TRAIT_GREEDY)
    price += price / 10;

  struct widget * header;
  struct widget * sell_button;
  char buf[1024];

  header = NULL;

  if(active_trader == NULL || active_trader->item_buy_count > 0)
    {
      snprintf(buf, sizeof buf, "  %s  ", gettext("Sell"));
      sell_button = widget_new_button(widget_root(), 0, 0, buf);
      widget_set_pointer(sell_button, "quest", 'P', quest);
      widget_set_ulong(sell_button, "price", price);
      widget_set_on_release(sell_button, on_sell_sell_clicked);
  
      snprintf(buf, sizeof buf, gettext("Sell %s for %u?"), treasure_longname(quest->treasure_object), (unsigned int) price);
    }
  else
    {
      sell_button = NULL;

      if(active_trader->gender == GENDER_FEMALE)
        snprintf(buf, sizeof buf, gettext("%s says that she would love to buy the %s, but she is out of money now. I should come back tomorrow."),
                 active_trader->name,
                 treasure_type_name(quest->treasure_object->type));
      else /* if(active_trader->gender == GENDER_MALE) */
        snprintf(buf, sizeof buf, gettext("%s says that he would love to buy the %s, but he is out of money now. I should come back tomorrow."),
                 active_trader->name,
                 treasure_type_name(quest->treasure_object->type));
    }

  widget_new_message_dialog(gettext("Sell item"), header, buf, sell_button);
}


static void on_sell_sell_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct quest * quest;
  unsigned int price;
  
  quest = widget_get_pointer(this, "quest", 'P');
  price = widget_get_ulong(this, "price");

  /* Record the transaction. */
  quest->treasure_sold = true;
  traits_add_score(price);
  if(active_trader != NULL)
    active_trader->item_buy_count--;
  event_trigger(EVENT_TYPE_QUEST_STATE_CHANGED, 0);

  /* Update screen. */
  struct questline * ql;
  unsigned int       newsel;
  
  assert(list != NULL);
  ql = widget_get_questline_pointer(list, "questline");

  newsel = widget_get_ulong(list, "selected_index");
  if(newsel > 0)
    newsel--;
  
  refresh_item_list(ql);
  refresh_item(newsel);
}
