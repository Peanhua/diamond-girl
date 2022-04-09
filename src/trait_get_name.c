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

#include "traits.h"
#include <assert.h>
#include <libintl.h>

char * trait_get_name(trait_t trait)
{
  char * name;

  switch(trait)
    {
    case TRAIT_KEY:            name = gettext("Key");            break;
    case TRAIT_ADVENTURE_MODE: name = gettext("Adventure Mode"); break;
    case TRAIT_RIBBON:         name = gettext("Ribbon");         break;
    case TRAIT_GREEDY:         name = gettext("Greedy");         break;
    case TRAIT_TIME_CONTROL:   name = gettext("Time Control");   break;
    case TRAIT_POWER_PUSH:     name = gettext("Power Push");     break;
    case TRAIT_DIAMOND_PUSH:   name = gettext("Diamond Push");   break;
    case TRAIT_RECYCLER:       name = gettext("Recycler");       break;
    case TRAIT_STARS1:         name = gettext("Stars 1");        break;
    case TRAIT_STARS2:         name = gettext("Stars 2");        break;
    case TRAIT_STARS3:         name = gettext("Stars 3");        break;
    case TRAIT_CHAOS:          name = gettext("Chaos");          break;
    case TRAIT_DYNAMITE:       name = gettext("Dynamite");       break;
    case TRAIT_IRON_GIRL:      name = gettext("Iron Girl");      break;
    case TRAIT_PYJAMA_PARTY:   name = gettext("Pyjama Party");   break;
    case TRAIT_QUESTS:         name = gettext("Quests");         break;
    case TRAIT_EDIT:           name = gettext("Edit");           break;
    case TRAIT_QUICK_CONTACT:  name = gettext("Quick Contact");  break;
    default:                   name = NULL; assert(0);           break;
    }

  return name;
}
