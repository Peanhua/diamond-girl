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
#include "random.h"

#include <assert.h>

#ifdef WITH_BUNDLED_MTWIST
# define MT_GENERATE_CODE_IN_HEADER 0
# include "../mtwist-1.5/mtwist.h"
#else
# include <mtwist.h>
#endif


/* range is the maximum number of different values from 0 to range-1 */
unsigned int get_rand(unsigned int range)
{
  unsigned int r;

  assert(range > 0);

  r = (double) range * mt_drand();

  assert(r < range);

  if(r > range - 1)
    r = range - 1;

  return r;
}

unsigned int get_rand_state(unsigned int range, void * state)
{
  unsigned int r;

  assert(range > 0);

  r = (double) range * mts_drand(state);

  assert(r < range);

  if(r > range - 1)
    r = range - 1;

  return r;
}

void seed_rand(uint32_t seed)
{
#ifdef PROFILING
  /* When profiling, always use the same seed. */
  mt_seed32(seed - seed);
#else
  mt_seed32(seed);
#endif
}

void * seed_rand_state(uint32_t seed)
{
  mt_state * state;

  state = malloc(sizeof(mt_state));
  assert(state != NULL);
  if(state != NULL)
    {
      state->stateptr = 0;
      state->initialized = 0;
#ifdef PROFILING
      mts_seed32new(state, seed - seed);
#else
      mts_seed32new(state, seed);
#endif
    }

  return state;
}


void * rand_state_duplicate(void * state)
{
  mt_state * src, * dst;

  src = state;
  dst = malloc(sizeof(mt_state));
  assert(dst != NULL);
  if(dst != NULL)
    {
      dst->stateptr    = src->stateptr;
      dst->initialized = src->initialized;
      for(int i = 0; i < MT_STATE_SIZE; i++)
        dst->statevec[i] = src->statevec[i];
    }

  return dst;
}
