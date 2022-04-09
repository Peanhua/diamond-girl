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

#include "globals.h"
#include "names.h"
#include "quest.h"
#include "random.h"
#include <assert.h>

char * quest_new_human_name(enum RELATION_TYPE relation_type, enum GENDER gender, double name_chance_players, double name_chance_existing)
{
  char name[128];
  int r;

  /* Firstname */
  snprintf(name, sizeof name, "%s", names_get(gender, 4, 12));

  /* Surname */
  r = get_rand(100);
  if(r < name_chance_players * 100.0) /* Same as player */
    snprintf(name + strlen(name), sizeof name - strlen(name), " %s", "Diamond");
  else if(globals.questlines_size > 0 && r < name_chance_players * 100.0 + name_chance_existing * 100.0)
    { /* Existing */
      int rr;
      char * space;
      
      rr = get_rand(globals.questlines_size);
      if(relation_type == RELATION_TYPE_GREAT_GRANDMOTHER ||
         relation_type == RELATION_TYPE_GREAT_GRANDFATHER ||
         relation_type == RELATION_TYPE_GRANDMOTHER       ||
         relation_type == RELATION_TYPE_GRANDFATHER       ||
         relation_type == RELATION_TYPE_MOTHER            ||
         relation_type == RELATION_TYPE_FATHER            ||
         relation_type == RELATION_TYPE_SISTER            ||
         relation_type == RELATION_TYPE_BROTHER           ||
         relation_type == RELATION_TYPE_AUNT              ||
         relation_type == RELATION_TYPE_UNCLE             ||
         relation_type == RELATION_TYPE_COUSIN            ||
         relation_type == RELATION_TYPE_RELATIVE          ||
         relation_type == RELATION_TYPE_FEMALE_FRIEND     ||
         relation_type == RELATION_TYPE_MALE_FRIEND         )
        space = strchr(globals.questlines[rr]->first_questgiver.name, ' ');
      else
        space = strchr(globals.questlines[rr]->ancient_person.name, ' ');
      assert(space != NULL);
      if(space != NULL)
        snprintf(name + strlen(name), sizeof name - strlen(name), "%s", space);
      else
        snprintf(name + strlen(name), sizeof name - strlen(name), " FAILURE");
    }
  else /* Generate new */
    snprintf(name + strlen(name), sizeof name - strlen(name), " %s", names_get(GENDER_FAMILY, 3, 10));

  return strdup(name);
}
