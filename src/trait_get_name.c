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

#include "traits.h"
#include <libintl.h>

char * trait_get_name(trait_t trait)
{
  static struct
  {
    trait_t trait_id;
    char *  name;
  } traits[] =
      {
        { TRAIT_KEY,            NULL },
        { TRAIT_ADVENTURE_MODE, NULL },
        { TRAIT_RIBBON,         NULL },
        { TRAIT_GREEDY,         NULL },
        { TRAIT_TIME_CONTROL,   NULL },
        { TRAIT_POWER_PUSH,     NULL },
        { TRAIT_DIAMOND_PUSH,   NULL },
        { TRAIT_RECYCLER,       NULL },
        { TRAIT_STARS1,         NULL },
        { TRAIT_STARS2,         NULL },
        { TRAIT_STARS3,         NULL },
        { TRAIT_CHAOS,          NULL },
        { TRAIT_DYNAMITE,       NULL },
        { TRAIT_IRON_GIRL,      NULL },
        { TRAIT_PYJAMA_PARTY,   NULL },
        { TRAIT_QUESTS,         NULL },
        { TRAIT_ALL,            NULL }
      };
  static bool initdone = false;

  if(initdone == false)
    {
      traits[0].name  = gettext("Key");
      traits[1].name  = gettext("AdventureMode");
      traits[2].name  = gettext("Ribbon");
      traits[3].name  = gettext("Greedy");
      traits[4].name  = gettext("Time Control");
      traits[5].name  = gettext("Power Push");
      traits[6].name  = gettext("Diamond Push");
      traits[7].name  = gettext("Recycler");
      traits[8].name  = gettext("Stars 1");
      traits[9].name  = gettext("Stars 2");
      traits[10].name = gettext("Stars 3");
      traits[11].name = gettext("Chaos");
      traits[12].name = gettext("Dynamite");
      traits[13].name = gettext("Iron Girl");
      traits[14].name = gettext("Pyjama Party");
      traits[15].name = gettext("Quests");
      initdone = true;
    }
  
  char * name;

  name = NULL;
  for(trait_t i = 0; name == NULL && traits[i].trait_id != TRAIT_ALL; i++)
    if(traits[i].trait_id == trait)
      name = traits[i].name;
  
  return name;
}
