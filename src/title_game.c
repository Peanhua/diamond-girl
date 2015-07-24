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

#include "diamond_girl.h"
#include "title.h"
#include "ai.h"
#include "game.h"
#include "playback.h"
#include "globals.h"
#include "traits.h"
#include "cave.h"
#include "ui.h"

#include <assert.h>


void title_game(struct cave * cave, struct playback * playback, char * playback_title)
{
  struct ai * ai;

  ai = ai_new();
  assert(ai != NULL);
  if(ai != NULL)
    {
      char * pberr;
      int starting_level;
      struct cave tmp_cave;
      trait_t  saved_traits_active;
      trait_t  saved_traits_available;
      uint64_t saved_traits_score;
      struct treasureinfo * treasure;

      saved_traits_active    = traits_get_active();
      saved_traits_available = traits_get_available();
      saved_traits_score     = traits_get_score();

      starting_level = 1;
      tmp_cave = *cave;
      treasure = NULL;

      pberr = NULL;
      if(playback != NULL)
        { /* Playback, override AI planning. */
          pberr = playback_check(playback);
          if(pberr == NULL)
            {
              struct cave * tmp;

              ai_playback(ai, playback->game_mode, playback, playback_title);

              tmp = cave_get(playback->game_mode, playback->cave);
              tmp_cave = *tmp;

              starting_level = playback->level;
              traits_set(playback->traits, playback->traits, 0);
              treasure = playback->treasure;

              ai->use_planning = false;
            }
        }
      else
        {
          tmp_cave.game_mode = GAME_MODE_ADVENTURE;
          traits_set(TRAIT_ADVENTURE_MODE | TRAIT_IRON_GIRL | TRAIT_RIBBON,
                     TRAIT_ADVENTURE_MODE | TRAIT_IRON_GIRL | TRAIT_RIBBON,
                     0);
        }

      if(pberr == NULL)
        {
          struct gamedata * gr;

          ui_state_push();
          gr = game(&tmp_cave, starting_level, false, false, ai, treasure);
          gr = gamedata_free(gr);
          ui_state_pop();
        }
      else
        free(pberr);

      ai = ai_free(ai);

      traits_set(saved_traits_active, saved_traits_available, saved_traits_score);
    }
}

