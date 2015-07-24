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
#include <assert.h>

static struct
{
  trait_t        trait;
  struct trait   data;
} traits[] =
  { /* Sorted by cave_name, cave_level: */
    { TRAIT_EDIT,           { "edit",           "beginners_cave",  0,   1000, GFX_IMAGE_TRAIT_EDIT,           GFX_IMAGE_TRAIT_EDIT_DISABLED           } },
    
    { TRAIT_KEY,            { "key",            "a",               5,   2000, GFX_IMAGE_TRAIT_KEY,            GFX_IMAGE_TRAIT_KEY_DISABLED            } },
    { TRAIT_STARS1,         { "stars1",         "a",              10,  20000, GFX_IMAGE_TRAIT_STARS1,         GFX_IMAGE_TRAIT_STARS1_DISABLED         } },
    { TRAIT_STARS2,         { "stars2",         "a",              15,  40000, GFX_IMAGE_TRAIT_STARS2,         GFX_IMAGE_TRAIT_STARS2_DISABLED         } },
    { TRAIT_STARS3,         { "stars3",         "a",              20,  60000, GFX_IMAGE_TRAIT_STARS3,         GFX_IMAGE_TRAIT_STARS3_DISABLED         } },
    { TRAIT_RIBBON,         { "ribbon",         "a",               0,  40000, GFX_IMAGE_TRAIT_RIBBON,         GFX_IMAGE_TRAIT_RIBBON_DISABLED         } },

    { TRAIT_ADVENTURE_MODE, { "adventure_mode", "/random",         3,   5000, GFX_IMAGE_TRAIT_ADVENTURE_MODE, GFX_IMAGE_TRAIT_ADVENTURE_MODE_DISABLED } },
    { TRAIT_PYJAMA_PARTY,   { "pyjama_party",   "/random",         5,   5000, GFX_IMAGE_TRAIT_PYJAMA_PARTY,   GFX_IMAGE_TRAIT_PYJAMA_PARTY_DISABLED   } },
    { TRAIT_QUESTS,         { "quests",         "/random",        10,   5000, GFX_IMAGE_TRAIT_QUESTS,         GFX_IMAGE_TRAIT_QUESTS_DISABLED         } },
    { TRAIT_IRON_GIRL,      { "iron_girl",      "/random",        15,  20000, GFX_IMAGE_TRAIT_IRON_GIRL,      GFX_IMAGE_TRAIT_IRON_GIRL_DISABLED      } },
    { TRAIT_POWER_PUSH,     { "power_push",     "/random",        25,  40000, GFX_IMAGE_TRAIT_POWER_PUSH,     GFX_IMAGE_TRAIT_POWER_PUSH_DISABLED     } },
    { TRAIT_QUICK_CONTACT,  { "quick_contact",  "/random",        30,  10000, GFX_IMAGE_TRAIT_QUICK_CONTACT,  GFX_IMAGE_TRAIT_QUICK_CONTACT_DISABLED  } },
    { TRAIT_GREEDY,         { "greedy",         "/random",        50,  40000, GFX_IMAGE_TRAIT_GREEDY,         GFX_IMAGE_TRAIT_GREEDY_DISABLED         } },
    { TRAIT_DYNAMITE,       { "dynamite",       "/random",        75, 100000, GFX_IMAGE_TRAIT_DYNAMITE,       GFX_IMAGE_TRAIT_DYNAMITE_DISABLED       } },
    { TRAIT_TIME_CONTROL,   { "time_control",   "/random",         0, 100000, GFX_IMAGE_TRAIT_TIME_CONTROL,   GFX_IMAGE_TRAIT_TIME_CONTROL_DISABLED   } },

    { TRAIT_CHAOS,          { "chaos",          "/well",          10,  30000, GFX_IMAGE_TRAIT_CHAOS,          GFX_IMAGE_TRAIT_CHAOS_DISABLED          } },
    { TRAIT_DIAMOND_PUSH,   { "diamond_push",   "/well",          20,  40000, GFX_IMAGE_TRAIT_DIAMOND_PUSH,   GFX_IMAGE_TRAIT_DIAMOND_PUSH_DISABLED   } },
    { TRAIT_RECYCLER,       { "recycler",       "/well",           0,  40000, GFX_IMAGE_TRAIT_RECYCLER,       GFX_IMAGE_TRAIT_RECYCLER_DISABLED       } },

    { TRAIT_ALL,            { NULL,             NULL,              0,      0, GFX_IMAGE_SIZEOF_,              GFX_IMAGE_SIZEOF_                       } }
  };

struct trait * trait_get(trait_t trait)
{
  struct trait * data;

  data = NULL;
  for(int i = 0; data == NULL && traits[i].trait != TRAIT_ALL; i++)
    if(traits[i].trait == trait)
      data = &traits[i].data;

  assert(data != NULL);
  assert(data->filename != NULL);
  
  return data;
}
