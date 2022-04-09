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

#include "game.h"
#include <assert.h>
#include <stdlib.h>

struct gamedata * gamedata_new(struct cave * cave, bool iron_girl_mode, trait_t traits)
{
  struct gamedata * gamedata;

  assert(cave != NULL);
  gamedata = malloc(sizeof(struct gamedata));
  assert(gamedata != NULL);
  if(gamedata != NULL)
    {
      gamedata->quit                   = false;
      gamedata->exit_after_one_level   = false;
      gamedata->starting_girls         = 0;
      gamedata->girls                  = 0;
      gamedata->score                  = 0;
      gamedata->diamond_score          = 0;
      gamedata->diamonds               = 0;
      gamedata->cave                   = cave;
      gamedata->current_level          = 0;
      gamedata->iron_girl_mode         = iron_girl_mode;
      gamedata->current_greedy_score   = 0;
      gamedata->traits                 = traits;
      gamedata->playback               = NULL;
      gamedata->treasure               = NULL;
      gamedata->map                    = NULL;
      gamedata->ai                     = NULL;
      gamedata->pyjama_party_girl      = NULL;
      gamedata->pyjama_party_girls_passed = 0;
      gamedata->pyjama_party_control   = PARTYCONTROLLER_PLAYER;
      gamedata->player_shift_move      = false;
      gamedata->push_counter           = 0;
      gamedata->girl_death_delay_timer = 0;
      gamedata->init_map               = NULL;
      gamedata->reset_map              = NULL;
    }

  return gamedata;
}
