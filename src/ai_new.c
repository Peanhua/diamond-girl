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

#include "ai.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

struct ai * ai_new(void)
{
  struct ai * ai;

  ai = malloc(sizeof(struct ai));
  assert(ai != NULL);
  if(ai != NULL)
    {
      ai->engine          = NULL;
      ai->playback_title  = NULL;
      ai->quit            = false;
      ai->use_planning    = true;
      ai->engine          = NULL;
      ai->map             = NULL;
      ai->playback        = NULL;
      ai->get_next_move   = NULL;
      ai->death           = NULL;
      ai->victory         = NULL;
      ai->cave_restart    = NULL;
    }
  else
    fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));

  return ai;
}
