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
#include "quest.h"
#include "stack.h"
#include <assert.h>
#include <libintl.h>

void quest_open(struct quest * quest)
{
  assert(quest != NULL);
  assert(quest->opened == 0);
  assert(quest->questline != NULL);
  assert(quest->questline->quests->size > 0);
  assert(quest->questline->current_quest < quest->questline->quests->size);
  assert(quest->questline->quests->data[quest->questline->current_quest] == quest);

  /* Update */
  quest->opened = time(NULL);
  
  if(quest->questline->quests->data[quest->questline->current_quest] == quest)
    {
      assert(quest->questline->current_phase != QUESTLINE_PHASE_OPENED);
      quest->questline->current_phase = QUESTLINE_PHASE_OPENED;
    }


  /* Add diary entry */
  char buf[1024];

  switch(quest->action_to_open)
    { /* Add prefix text for the description, describing the action that was taken. */
    case QUEST_ACTION_EXAMINE_NOTES:
      strcpy(buf, "");
      break;
    case QUEST_ACTION_EXAMINE_OBJECT:
      snprintf(buf, sizeof buf, gettext("Examined the latest find."));
      break;
    case QUEST_ACTION_VISIT_LIBRARY:
      strcpy(buf, "");
      break;
    case QUEST_ACTION_VISIT_CAFE:
      strcpy(buf, "");
      break;
    }
  if(strlen(buf) > 0)
    snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "  ");
  snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "%s", quest_description(quest));

  questline_diary_add(quest->questline, quest->opened, NULL, buf);


  /* Trigger event */
  event_trigger(EVENT_TYPE_QUEST_STATE_CHANGED, 0);
}
