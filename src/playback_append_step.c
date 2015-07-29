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


struct playback_step * playback_append_step(struct playback * playback)
{
  assert(playback != NULL);

  if(playback->steps_size + 1 >= playback->steps_allocated)
    {
      struct playback_step * tmp;
      uint16_t n;

      if(playback->steps_allocated == 0)
        n = 256;
      else
        n = playback->steps_allocated * 2;
      tmp = realloc(playback->steps, sizeof(struct playback_step) * n);
      assert(tmp != NULL);
      if(tmp != NULL)
        {
          playback->steps = tmp;
          playback->steps_allocated = n;
        }
      else
        fprintf(stderr, "%s: Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
    }

  
  struct playback_step * step;

  if(playback->steps_size + 1 < playback->steps_allocated)
    {
      step = &playback->steps[playback->steps_size];
      playback->steps_size++;
      playback_step_initialize(step);
    }
  else
    step = NULL;

  assert(step != NULL);

  return step;
}
