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

#include "playback.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct playback * playback_new(void)
{
  struct playback * rv;

  rv = malloc(sizeof(struct playback));
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->next           = NULL;
      rv->game_mode      = GAME_MODE_CLASSIC;
      rv->iron_girl_mode = false;
      rv->traits         = 0;
      rv->cave           = NULL;
      rv->level          = 0;
      rv->treasure       = NULL;
      rv->steps          = NULL;
      rv->steps_size     = 0;
      rv->current_step   = 0;
      rv->not_for_ai     = false;
      rv->map_hash       = NULL;
    }
  else
    fprintf(stderr, "%s: Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
      
  return rv;
}
