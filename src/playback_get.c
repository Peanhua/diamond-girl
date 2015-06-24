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

uint32_t playback_get_levels_completed(struct playback * playback)
{
  uint32_t rv;

  assert(playback != NULL);
  rv = 0;
  while(playback != NULL)
    {
      if(playback->next != NULL && playback->level != playback->next->level)
        rv++;

      playback = playback->next;
    }
  return rv;
}

uint32_t playback_get_girls_died(struct playback * playback)
{
  uint32_t rv;

  assert(playback != NULL);
  rv = 0;
  while(playback != NULL)
    {
      if(playback->game_mode == GAME_MODE_CLASSIC || playback->game_mode == GAME_MODE_ADVENTURE)
        {
          if(playback->next == NULL || playback->level == playback->next->level)
            rv++;
        }
      else /* if(playback->game_mode == GAME_MODE_PYJAMA_PARTY) */
        {
          /* unable to calculate this in pyjama party mode */
        }

      playback = playback->next;
    }
  return rv;
}

