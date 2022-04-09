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

#include <assert.h>


struct playback_step * playback_next_step(struct playback * playback)
{ /* Return current step and advance current_step by one. Returns NULL once past the steps. */
  struct playback_step * step;

  assert(playback != NULL);
  assert(playback->steps_size < 0xffff - 1); /* Wouldn't work, the last step would not be returned. */

  step = NULL;

  if(playback->current_step < playback->steps_size)
    step = &playback->steps[playback->current_step];

  playback->current_step++;
  if(playback->current_step >= playback->steps_size)
    playback->current_step = 0xffff;

  return step;
}
