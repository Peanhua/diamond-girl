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

#include "playback.h"
#include <assert.h>

struct playback * playback_copy(struct playback * playback)
{
  struct playback * rv;

  rv = NULL;
  assert(playback != NULL);
  if(playback != NULL)
    {
      rv = playback_new();
      assert(rv != NULL);
      if(rv != NULL)
        {
          rv->game_mode      = playback->game_mode;
          rv->iron_girl_mode = playback->iron_girl_mode;
          rv->traits         = playback->traits;
          rv->cave           = strdup(playback->cave);
          rv->level          = playback->level;
          rv->not_for_ai     = playback->not_for_ai;

          if(playback->map_hash != NULL)
            rv->map_hash     = strdup(playback->map_hash);
          
          for(uint16_t i = 0; i < playback->steps_size; i++)
            {
              struct playback_step * step;

              step = playback_append_step(rv);
              assert(step != NULL);
              if(step != NULL)
                {
#ifdef DEBUG_PLAYBACK
                  step->timestamp       = playback->steps[i].timestamp;
                  step->timestamp_timer = playback->steps[i].timestamp_timer;
#endif
                  step->position[0]     = playback->steps[i].position[0];
                  step->position[1]     = playback->steps[i].position[1];
                  step->direction       = playback->steps[i].direction;
                  step->manipulate      = playback->steps[i].manipulate;
                  step->commit_suicide  = playback->steps[i].commit_suicide;
                }
            }
          rv->current_step   = playback->current_step;

          if(playback->next != NULL)
            rv->next = playback_copy(playback->next);
        }
    }
      
  return rv;
}
