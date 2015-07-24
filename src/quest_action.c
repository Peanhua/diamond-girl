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
#include "globals.h"
#include "quest.h"
#include "td_object.h"
#include "trader.h"
#include "treasure.h"
#include "widget.h"
#include <assert.h>
#include <ctype.h>
#include <libintl.h>

static void on_accept_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_sell_items_clicked(struct widget * this, enum WIDGET_BUTTON button);
static bool open_other_questlines(enum QUEST_ACTION action);

void quest_action(struct questline * questline, enum QUEST_ACTION action)
{
  if(open_other_questlines(action) == true)
    return;
  
  char              title[128];
  char              buf[1024];
  struct quest *    quest;
  struct quest *    headerquest;
  struct treasure * object;
  bool              generic_failure;

  headerquest = quest = questline->quests->data[questline->current_quest];
  generic_failure = false;
  assert(questline->current_phase != QUESTLINE_PHASE_NONE);
  if(questline->current_phase == QUESTLINE_PHASE_OPENED)
    { /* Treasure is not yet collected, so display "history". */
      struct quest * prevquest;

      if(questline->current_quest == 0)
        prevquest = questline->quests->data[0];
      else
        prevquest = questline->quests->data[questline->current_quest - 1];

      headerquest = prevquest;
      object = prevquest->treasure_object;

      if(quest->action_to_open == action)
        {
          snprintf(buf, sizeof buf, quest_description(quest));
        }
      else
        {
          generic_failure = true;
        }
    }
  else if(questline->current_phase == QUESTLINE_PHASE_TREASURE_COLLECTED)
    { /* Treasure collected, check the next quest. */
      object = quest->treasure_object;
      
      if(questline->current_quest + 1 < questline->quests->size)
        { 
          struct quest * nextquest;

          nextquest = questline->quests->data[questline->current_quest + 1];
          if(nextquest->action_to_open == action)
            { /* Next quest is opened by the given action, so we do that. */
              questline->current_quest++;
              quest_open(nextquest);
              snprintf(buf, sizeof buf, quest_description(nextquest));
            }
          else
            { /* Next quest is not opened by the given action, so we find nothing now. */
              generic_failure = true;
            }
        }
      else
        { /* End of quests, so we find nothing now. */
          if(action == QUEST_ACTION_EXAMINE_OBJECT)
            snprintf(buf, sizeof buf, gettext("This is it, the last treasure associated to the %s.\n\nR.I.P. %s"),
                     relation_type_name(questline->ancient_person.relation_to_player),
                     questline->ancient_person.name);
          else
            generic_failure = true;
        }
    }

  struct widget * header;
  struct widget * button;

  header = NULL;
  button = NULL;
  
  switch(action)
    {
    case QUEST_ACTION_EXAMINE_NOTES:
      snprintf(title, sizeof title, gettext("Notes of %s"), questline->ancient_person.name);

      header = widget_new_gfx_image(widget_root(), 0, 0, GFX_IMAGE_NOTES);
      widget_delete_flags(header, WF_BACKGROUND);

      if(generic_failure == true)
        switch(questline->type)
          {
          case QUEST_TYPE_RELATIVE:
            snprintf(buf, sizeof buf, gettext("I've gone through the notes of %s again,\nbut found nothing new."), questline->ancient_person.name);
            break;
          case QUEST_TYPE_CHILDHOOD_DREAM:
            snprintf(buf, sizeof buf, "%s", gettext("I've gone through the notes again,\nbut found nothing new."));
            break;
          case QUEST_TYPE_SIZEOF_:
            assert(0);
            break;
          }
      break;

    case QUEST_ACTION_EXAMINE_OBJECT:
      assert(object != NULL);
      snprintf(title, sizeof title, gettext("Latest find"));
      title[0] = toupper(title[0]);

      header = widget_new_quest_treasure_info(widget_root(), 0, 0, headerquest, true);

      if(generic_failure == true)
        snprintf(buf, sizeof buf, gettext("I've carefully examined the %s,\nbut didn't find anything special on it."), treasure_type_name(object->type));
      break;

    case QUEST_ACTION_VISIT_LIBRARY:
      snprintf(title, sizeof title, "%s", gettext("Library"));

      header = widget_new_gfx_image(widget_root(), 0, 0, GFX_IMAGE_BOOKPILE);
      widget_delete_flags(header, WF_BACKGROUND);
      
      if(generic_failure == true)
        snprintf(buf, sizeof buf, gettext("None of the books seems to be interesting right now."));
      break;

    case QUEST_ACTION_VISIT_CAFE:
      snprintf(title, sizeof title, "%s", gettext("Cafe"));

      header = widget_new_gfx_image(widget_root(), 0, 0, GFX_IMAGE_CAFE);
      widget_delete_flags(header, WF_BACKGROUND);

      if(generic_failure == true)
        {
          struct trader * trader;
          time_t          now;
          unsigned int    now_hour;
          struct tm *     tm;
          bool            trader_is_here;

          now      = time(NULL);
          tm       = localtime(&now);
          now_hour = tm->tm_hour;
          trader   = trader_current();

          if(traits_get_active() & TRAIT_QUICK_CONTACT)
            trader_is_here = true;
          else if(trader->daily_visit_arrival <= now_hour && now_hour < trader->daily_visit_arrival + trader->daily_visit_length)
            trader_is_here = true;
          else
            trader_is_here = false;
          
          if(questline->current_phase == QUESTLINE_PHASE_OPENED)
            if(quest->opened + 60 * 60 * quest->hours_to_salesman < now)
              {
                if(trader_is_here == true)
                  {
                    unsigned int cost;
                    struct cave * cave;
                    
                    cost = treasure_price(quest->treasure_object) * 3 + 20 * questline->current_quest;
                    
                    cave = cave_get(GAME_MODE_ADVENTURE, quest->treasure_cave);
                    if(cave == NULL || cave->max_starting_level < (int) quest->treasure_level)
                      cost += 100 + 10 * questline->current_quest;
                    
                    cost *= 1.0 + (double) trader->cost_modifier / 100.0;
                    
                    if(traits_get_score() >= cost)
                      {
                        snprintf(buf, sizeof buf, " %s ", gettext("Accept offer"));
                        button = widget_new_button(widget_root(), 0, 0, buf);
                        if(button != NULL)
                          {
                            widget_set_ulong(button, "cost", cost);
                            widget_set_questline_pointer(button, "questline", questline);
                            widget_set_on_release(button, on_accept_clicked);
                          }
                      }

                    if(trader->gender == GENDER_FEMALE)
                      snprintf(buf, sizeof buf, gettext("I see %s in the cafe, she offers to fetch the %s %s '%s' for %u.\nI have %u."),
                               trader->name,
                               treasure_material_name(quest->treasure_object->material),
                               treasure_type_name(quest->treasure_object->type),
                               quest->treasure_object->name,
                               cost,
                               (unsigned int) traits_get_score());
                    else
                      snprintf(buf, sizeof buf, gettext("I see %s in the cafe, he offers to fetch the %s %s '%s' for %u.\nI have %u."),
                               trader->name,
                               treasure_material_name(quest->treasure_object->material),
                               treasure_type_name(quest->treasure_object->type),
                               quest->treasure_object->name,
                               cost,
                               (unsigned int) traits_get_score());
                    
                    generic_failure = false;
                  }
                else
                  {
                    int hours;

                    if(now_hour < trader->daily_visit_arrival)
                      hours = (now_hour + 24) - (trader->daily_visit_arrival + trader->daily_visit_length);
                    else
                      hours = now_hour - (trader->daily_visit_arrival + trader->daily_visit_length);

                    if(hours <= 1) snprintf(buf, sizeof buf, gettext("The bartender says %s was here a moment ago."), trader->name);
                    else           snprintf(buf, sizeof buf, gettext("The bartender says %s was here %d hours ago."), trader->name, hours);
                    
                    snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n\n%s", gettext("I drink my favorite beverage and read the local newspaper."));
                    
                    generic_failure = false;
                  }
              }

          /* Give player a chance to buy back previous quests treasure item if the player has sold it and that item is required for opening the next quest. */
          if(generic_failure == true)
            if(quest->treasure_sold == true)
              if(questline->current_phase == QUESTLINE_PHASE_TREASURE_COLLECTED)
                if(questline->current_quest + 1 < questline->quests->size)
                  {
                    struct quest * nextquest;
                  
                    nextquest = questline->quests->data[questline->current_quest + 1];
                    if(nextquest->action_to_open == QUEST_ACTION_EXAMINE_OBJECT)
                      {
                        unsigned int cost;

                        cost = treasure_price(quest->treasure_object) * 3;
                        cost *= 1.0 + (double) trader->cost_modifier / 100.0;

                        snprintf(buf, sizeof buf, " %s ", gettext("Accept offer"));
                        button = widget_new_button(widget_root(), 0, 0, buf);
                        if(button != NULL)
                          {
                            widget_set_ulong(button, "cost", cost);
                            widget_set_questline_pointer(button, "questline", questline);
                            widget_set_pointer(button, "trader", 'P', trader);
                            widget_set_on_release(button, on_accept_clicked);
                          }
                        
                        if(trader->gender == GENDER_FEMALE)
                          snprintf(buf, sizeof buf, gettext("%s is in the cafe, she says she could sell the %s %s '%s' for %u.\nI have %u."),
                                   trader->name,
                                   treasure_material_name(quest->treasure_object->material),
                                   treasure_type_name(quest->treasure_object->type),
                                   quest->treasure_object->name,
                                   cost,
                                   (unsigned int) traits_get_score());
                        else
                          snprintf(buf, sizeof buf, gettext("%s is in the cafe, he says she could sell the %s %s '%s' for %u.\nI have %u."),
                                   trader->name,
                                   treasure_material_name(quest->treasure_object->material),
                                   treasure_type_name(quest->treasure_object->type),
                                   quest->treasure_object->name,
                                   cost,
                                   (unsigned int) traits_get_score());

                        generic_failure = false;
                      }
                  }

          if(generic_failure == true)
            if(trader_is_here == true)
              if(trader->item_buy_count > 0)
                {
                  bool has_treasures;

                  has_treasures = false;
                  for(unsigned int i = 0; has_treasures == false && i < globals.questlines_size; i++)
                    {
                      struct questline * ql;
                    
                      ql = globals.questlines[i];
                      if(questline_treasure_get_collected_count(ql) > 0)
                        has_treasures = true;
                    }

                  if(has_treasures == true)
                    {
                      snprintf(buf, sizeof buf, " %s ", gettext("Sell items"));
                      button = widget_new_button(widget_root(), 0, 0, buf);
                      if(button != NULL)
                        {
                          widget_set_questline_pointer(button, "questline", questline);
                          widget_set_pointer(button, "trader", 'P', trader);
                          widget_set_on_release(button, on_sell_items_clicked);
                        }

                      if(trader->gender == GENDER_FEMALE)
                        snprintf(buf, sizeof buf, gettext("I see %s in the cafe, she offers to buy some items."), trader->name);
                      else
                        snprintf(buf, sizeof buf, gettext("I see %s in the cafe, he offers to buy some items."), trader->name);
                    
                      generic_failure = false;
                    }
                }
        }
      
      if(generic_failure == true)
        snprintf(buf, sizeof buf, gettext("I drink my favorite beverage and read the local newspaper."));
      break;
    }

  if(header)
    widget_delete_flags(header, WF_DRAW_BORDERS);
  
  widget_new_message_dialog(title, header, buf, button);
}

static void on_sell_items_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct questline * questline;
  struct trader * trader;

  questline = widget_get_questline_pointer(this, "questline");
  assert(questline != NULL);

  trader = widget_get_pointer(this, "trader", 'P');
  assert(trader != NULL);
  
  quest_treasures(questline, trader);
}


static void on_accept_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  unsigned int cost;
  struct questline * questline;

  cost = widget_get_ulong(this, "cost");
  assert(cost > 0);
  assert(traits_get_score() >= cost);

  questline = widget_get_questline_pointer(this, "questline");
  assert(questline != NULL);
  if(questline != NULL)
    {
      struct quest * quest;

      quest = questline->quests->data[questline->current_quest];
      if(questline->current_phase == QUESTLINE_PHASE_OPENED)
        quest->completion_id = 1 + globals.active_trader; // Store the trader id from whom it was bought (used in quest_diary).
      /* todo: record the transaction to diary if current_phase == QUESTLINE_PHASE_TREASURE_COLLECTED, that is: player is buying back the item */
      traits_delete_score(cost);
      quest_collect_treasure(questline);

      quest_action(questline, QUEST_ACTION_EXAMINE_OBJECT);
    }
}


/* Open first non-opened questline which opens with the given action. Return true if opened some other quest. */
bool open_other_questlines(enum QUEST_ACTION action)
{ 
  bool opened;
  time_t      tnow;
  struct tm * now;

  tnow = time(NULL);
  now = localtime(&tnow);
  opened = false;
  for(unsigned int i = 0; opened == false && i < globals.questlines_size; i++)
    {
      struct questline * ql;

      ql = globals.questlines[i];
      if(ql->current_quest == 0 && ql->current_phase == QUESTLINE_PHASE_NONE)
        if(ql->opening_weekday < 0 || (ql->opening_weekday == now->tm_wday && now->tm_mday <= 7))
          {
            struct quest * first;

            assert(ql->quests->size > 0);
            first = ql->quests->data[0];
            if(first->action_to_open == action)
              {
                opened = true;
                quest_open(first);
                quest_action(ql, action);
              }
          }
    }
  
  return opened;
}
