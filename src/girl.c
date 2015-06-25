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

#include "girl.h"
#include "random.h"
#include "map.h"
#include "globals.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

struct girl * girl_new(void)
{
  struct girl * girl;
  struct map_object * mob;

  girl = NULL;

  mob = malloc(sizeof(struct map_object));
  assert(mob != NULL);
  if(mob != NULL)
    {
      mob->type       = MOB_PLAYER;
      mob->alive      = true;
      mob->armour     = 2;
      mob->x          = 0;
      mob->y          = 0;
      mob->direction         = MOVE_NONE;
      mob->moved_over_glyph  = MAP_SIZEOF_;
      mob->animation         = 0;
      mob->animation_repeats = 0;
      mob->chat_chance       = 0;
      mob->chat_bubble[0]    = '\0';
      mob->chat_bubble_timer = 0;

      girl = malloc(sizeof(struct girl));
      assert(girl != NULL);
      if(girl != NULL)
        {
          const char * name;
          bool ok;
          
          ok = false;
          name = NULL;
          while(ok == false)
            {
              name = get_random_name();
              ok = true;
              for(int i = 0; ok == true && i < globals.pyjama_party_girls_size; i++)
                if(globals.pyjama_party_girls[i] != NULL)
                  if(!strcmp(name, globals.pyjama_party_girls[i]->name))
                    ok = false;
            }
          assert(name != NULL);
          strcpy(girl->name,        name);
          girl->birth_time        = time(NULL);
          girl->mob               = mob;
          girl->traits            = 0;
          girl->possible_traits   = 0;
          girl->diamonds          = 0;
          girl->diamonds_to_trait = 0;
        }
      else
        fprintf(stderr, "%s(): Failed to allocate memory for the girl: %s\n", __FUNCTION__, strerror(errno));
    }
  else
    fprintf(stderr, "%s(): Failed to allocate memory for the girl: %s\n", __FUNCTION__, strerror(errno));

  return girl;
}


void girl_chat(struct girl * girl, char * message)
{
  if(girl->mob->chat_chance > 0)
    if(get_rand(100) < girl->mob->chat_chance)
      {
        snprintf(girl->mob->chat_bubble, sizeof girl->mob->chat_bubble, "%s", message);
        girl->mob->chat_bubble_timer = 15 + strlen(message) * 2;
      }
}
