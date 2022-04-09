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
#include "event.h"
#include "globals.h"
#include "quest.h"
#include "stack.h"
#include "trader.h"
#include "treasure.h"
#include <assert.h>
#include <libintl.h>


void quest_collect_treasure(struct questline * questline)
{
  struct quest * quest;

  assert(questline != NULL);
  quest = questline->quests->data[questline->current_quest];
  
  if(questline->current_phase == QUESTLINE_PHASE_OPENED)
    {
      int progress;

      progress = (questline->current_quest + 1) * 100 / questline->quests->size;
      
      questline->current_phase = QUESTLINE_PHASE_TREASURE_COLLECTED;
      quest->completed = time(NULL);
  
      char buf[1024];

      switch(quest->completion_id)
        {
        case 0:
          snprintf(buf, sizeof buf, gettext("I've found %s from %s, level %u!"),
                   treasure_longname(quest->treasure_object),
                   cave_displayname(quest->treasure_cave),
                   quest->treasure_level);
          break;
        default:
          assert(quest->completion_id - 1 < MAX_TRADERS);
          if(quest->completion_id - 1 < MAX_TRADERS)
            {
              struct trader * trader;
          
              trader = globals.traders[quest->completion_id - 1];
              assert(trader != NULL);
              snprintf(buf, sizeof buf, gettext("I hired %s to recover %s from %s for me."),
                       trader->name,
                       treasure_longname(quest->treasure_object),
                       cave_displayname(quest->treasure_cave));
            }
          break;
        }
      
      questline_diary_add(questline, quest->completed, quest->treasure_object, buf);

      if(progress < 50)
        { /* If 50% is passed, then next zombiequestline becomes available. */
          int newprogress;

          newprogress = (questline->current_quest + 1) * 100 / questline->quests->size;
          if(newprogress >= 50)
            {
              struct questline * ql;

              ql = questline_generate(QUEST_TYPE_ZOMBIES);
              if(ql != NULL)
                {
                  globals.questlines[globals.questlines_size] = ql;
                  globals.questlines_size++;
                }
            }
        }
    }
  else if(questline->current_phase == QUESTLINE_PHASE_TREASURE_COLLECTED)
    { /* Player had sold the item earlier and is now buying it back. */
      quest->treasure_sold = false;

      char buf[1024];
      struct trader * trader;

      assert(globals.active_trader < MAX_TRADERS);
      trader = globals.traders[globals.active_trader];
      assert(trader != NULL);
      
      snprintf(buf, sizeof buf, gettext("Bought %s back from %s."),
               treasure_longname(quest->treasure_object),
               trader->name);

      { /* Change quest->completed (timestamp) for questline_diary_add() -call. */
        time_t tmp;
        tmp = quest->completed;
        quest->completed = time(NULL);
        questline_diary_add(questline, quest->completed, quest->treasure_object, buf);
        quest->completed = tmp;
      }
    }
  else
    {
      assert(0);
    }
  
  
  event_trigger(EVENT_TYPE_QUEST_STATE_CHANGED, 0);
}
