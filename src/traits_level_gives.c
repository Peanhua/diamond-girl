/*
  Diamond Girl - Game where player collects diamonds.
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
#include "cave.h"
#include <string.h>
#include <assert.h>

trait_t traits_level_gives(struct cave * cave, int level)
{
  trait_t rv;

  struct 
  {
    char *  name;
    int     level; /* level 0 is special: the last level of the cave */
    trait_t trait_given;
  } traits[] =
      {
        { "/random",  5, TRAIT_PYJAMA_PARTY   },
        { "/random", 10, TRAIT_ADVENTURE_MODE },
        { "/random", 15, TRAIT_IRON_GIRL      },
        { "/random", 25, TRAIT_POWER_PUSH     },
        { "/random", 50, TRAIT_GREEDY         },
        { "/random", 75, TRAIT_DYNAMITE       },
        { "/random",  0, TRAIT_TIME_CONTROL   },

        { "/well", 10, TRAIT_CHAOS        },
        { "/well", 20, TRAIT_DIAMOND_PUSH },
        { "/well",  0, TRAIT_RECYCLER     },

        { "a",   5, TRAIT_KEY             },
        { "a",  10, TRAIT_STARS1          },
        { "a",  15, TRAIT_STARS2          },
        { "a",  20, TRAIT_STARS3          },
        { "a",   0, TRAIT_RIBBON          },

        { NULL, -1, 0                     }
      };
  
  bool last_level;
  
  assert(cave != NULL);

  last_level = false;
  if(level == cave->level_count)
    last_level = true;

  rv = 0;
  for(int i = 0; rv == 0 && traits[i].level >= 0; i++)
    if(traits[i].level == level || (traits[i].level == 0 && last_level == true))
      if(!strcmp(traits[i].name, cave->name))
        rv = traits[i].trait_given;

  return rv;
}
