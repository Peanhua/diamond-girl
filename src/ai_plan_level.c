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

#include "ai.h"
#include "map.h"
#include "random.h"
#include "game.h"

/**
 * Plan what to do with the level in hand.
 */

void ai_plan_level(struct ai * ai, enum GAME_MODE game_mode)
{
  float dh_goodness, pb_goodness;
  char buf[128];
  static char * lucy_playback = "Lucy (pb)";
  static char * lucy_hunter   = "Lucy (ai)";

  snprintf(buf, sizeof buf, "engine=diamond_hunter,level=%d,map_completed,cave=%s", ai->map->level, ai->map->cave_name);
  dh_goodness = ai_brains_get(buf) + 0.1f * ((float) get_rand(200) - 100.0f) / 100.0f;
  snprintf(buf, sizeof buf, "engine=playback,level=%d,map_completed,cave=%s", ai->map->level, ai->map->cave_name);
  pb_goodness = ai_brains_get(buf) + 0.1f * ((float) get_rand(200) - 100.0f) / 100.0f;

  bool done;

  done = false;
  while(done == false)
    {
      if(dh_goodness > pb_goodness)
        {
          ai->playback_title = lucy_hunter;
          ai_diamond_hunter(ai);
          done = true;
        }
      else
        { /* playback */
          ai->playback_title = lucy_playback;
          if(ai_playback(ai, game_mode, NULL, lucy_playback))
            done = true;
          else /* Disable playback from the choices and try again. */
            pb_goodness -= 2.0f;
        }
    }
}
