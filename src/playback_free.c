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

#include "playback.h"
#include "treasureinfo.h"
#include <assert.h>


struct playback * playback_free(struct playback * playback)
{
  assert(playback != NULL);
  if(playback != NULL)
    {
      free(playback->cave);
      playback->cave = NULL;

      free(playback->map_hash);
      playback->map_hash = NULL;

      free(playback->steps);
      playback->steps = NULL;

      if(playback->next != NULL)
        playback->next = playback_free(playback->next);

      if(playback->treasure != NULL)
        {
          free(playback->treasure->item);
          playback->treasure->item = NULL;

          free(playback->treasure);
          playback->treasure = NULL;
        }

      free(playback);
    }

  return NULL;
}
