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
#include "quest.h"
#include "stack.h"
#include "treasure.h"
#include <assert.h>
#include <libintl.h>


char const * quest_description(struct quest * quest)
{
  static char buf[1024 * 4];
  struct quest * prevquest;
  
  assert(quest != NULL);
  strcpy(buf, "");

  prevquest = NULL;
  for(unsigned int i = 1; prevquest == NULL && i < quest->questline->quests->size; i++)
    if(quest->questline->quests->data[i] == quest)
      prevquest = quest->questline->quests->data[i - 1];
  
  switch(quest->action_to_open)
    {
    case QUEST_ACTION_EXAMINE_NOTES:
      switch(quest->description_id)
        {
        case 0:
          snprintf(buf, sizeof buf, gettext("My %s %s has told me about %s named %s.  According to old legends, %s had %s.\nI have tracked it to %s."),
                   relation_type_name(quest->questline->first_questgiver.relation_to_player),
                   quest->questline->first_questgiver.name,
                   relation_type_name(quest->questline->ancient_person.relation_to_player),
                   quest->questline->ancient_person.name,
                   quest->questline->ancient_person.gender == GENDER_FEMALE ? gettext("she") : gettext("he"),
                   treasure_longname(quest->treasure_object),
                   cave_displayname(quest->treasure_cave));
          buf[0] = toupper(buf[0]);
          break;
        case 1:
          snprintf(buf, sizeof buf, gettext("I have spent more time studying the notes of %s %s, and found writing of ancient %s.  It seems to be buried in %s."),
                   relation_type_name(quest->questline->ancient_person.relation_to_player),
                   quest->questline->ancient_person.name,
                   treasure_longname(quest->treasure_object),
                   cave_displayname(quest->treasure_cave));
          break;
        case 2:
          snprintf(buf, sizeof buf, gettext("More thorough study of the notes of %s %s revealed knowledge about %s named '%s'.  The old people buried it somewhere in %s."),
                   relation_type_name(quest->questline->ancient_person.relation_to_player),
                   quest->questline->ancient_person.name,
                   treasure_type_name(quest->treasure_object->type),
                   quest->treasure_object->name,
                   cave_displayname(quest->treasure_cave));
          break;
        }
      break;

    case QUEST_ACTION_EXAMINE_OBJECT:
      assert(prevquest != NULL);
      switch(quest->description_id)
        {
        case 0:
          snprintf(buf, sizeof buf, gettext("Closer inspection of the %s %s '%s' revealed writing: %s"),
                   treasure_material_name(prevquest->treasure_object->material),
                   treasure_type_name(prevquest->treasure_object->type),
                   prevquest->treasure_object->name,
                   cave_displayname(quest->treasure_cave));
          break;
        case 1:
          { /* Make some of the cave name characters garbage. */
            char tmp[20 + 1];
            int  garbageinds[] = { 1, 3, 5, 6, 9, 10, 11, 16, 18, 19, 20, -1 };
            int gi;
            char const * cavename;
            int          cavenamelen;

            cavename    = cave_displayname(quest->treasure_cave);
            cavenamelen = strlen(cavename);
            
            gi = 0;
            for(int i = 0; i < cavenamelen && i < 20; i++)
              if(i == garbageinds[gi])
                {
                  tmp[i] = '#';
                  gi++;
                }
              else
                tmp[i] = cavename[i];
            tmp[20] = '\0';

            if(strlen(tmp) > 8)
              for(int i = 0; i < 3; i++)
                tmp[strlen(tmp) - i - 1] = '.';
            
            snprintf(buf, sizeof buf, gettext("There is a damaged writing on the %s %s '%s', it reads: %s"), 
                                              treasure_material_name(prevquest->treasure_object->material),
                                              treasure_type_name(prevquest->treasure_object->type),
                                              prevquest->treasure_object->name,
                                              tmp);
          }
          break;
        case 2:
          snprintf(buf, sizeof buf, gettext("The %s seems to be one of many, perhaps there are more nearby."),
                   treasure_longname(prevquest->treasure_object));
          break;
        }
      break;
      
    case QUEST_ACTION_VISIT_LIBRARY:
      if(quest->questline->ancient_person.gender == GENDER_FEMALE)
        snprintf(buf, sizeof buf, gettext("I have found a book of %s %s from the library.  Her %s must have been buried somewhere in %s.  I decided to pursue my childhood dream of finding it."),
                 relation_type_name(quest->questline->ancient_person.relation_to_player),
                 quest->questline->ancient_person.name,
                 treasure_longname(quest->treasure_object),
                 cave_displayname(quest->treasure_cave));
      else
        snprintf(buf, sizeof buf, gettext("I have found a book of %s %s from the library.  His %s must have been buried somewhere in %s.  I decided to pursue my childhood dream of finding it."),
                 relation_type_name(quest->questline->ancient_person.relation_to_player),
                 quest->questline->ancient_person.name,
                 treasure_longname(quest->treasure_object),
                 cave_displayname(quest->treasure_cave));
      break;
    case QUEST_ACTION_VISIT_CAFE:
      assert(0);
      break;
    }

  return buf;
}