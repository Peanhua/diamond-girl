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

#include "quest.h"
#include <assert.h>
#include <libintl.h>

char const * questline_name(struct questline * questline)
{
  static char buf[128];

  strcpy(buf, "");
  switch(questline->type)
    {
    case QUEST_TYPE_RELATIVE:
    case QUEST_TYPE_LIBRARY:
      snprintf(buf, sizeof buf, gettext("%s %s"),
               relation_type_name(questline->ancient_person.relation_to_player),
               questline->ancient_person.name);
      break;
    case QUEST_TYPE_ZOMBIES:
      snprintf(buf, sizeof buf, gettext("Zombie %s %s"),
               relation_type_name(questline->ancient_person.relation_to_player),
               questline->ancient_person.name);
      break;
    case QUEST_TYPE_SIZEOF_:
      break;
    }
  assert(strlen(buf) > 0);
  buf[0] = toupper(buf[0]);

  return buf;
}

