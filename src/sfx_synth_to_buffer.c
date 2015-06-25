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

#include "sfx_synth.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

ALuint sfx_synth_to_buffer(struct sfx_synth * sfx_synth)
{
  ALuint rv;
  uint32_t len;
  int16_t tmp;

  rv = AL_NONE;
  len = 0;
  while(sfx_synth_get_samples(sfx_synth, &tmp, 1) == 1)
    len++;

  if(len > 0)
    {
      int16_t * wav;

      wav = malloc(sizeof(int16_t) * len);
      assert(wav != NULL);
      if(wav != NULL)
        {
          sfx_synth_restart(sfx_synth);

#ifndef NDEBUG
          uint32_t nlen;

          nlen =
#endif
            sfx_synth_get_samples(sfx_synth, wav, len);
          assert(nlen == len);

          if(len > 0)
            {
              alGenBuffers(1, &rv);
              alBufferData(rv, AL_FORMAT_MONO16, wav, len * sizeof(int16_t), 44100);

#ifndef NDEBUG
              if(sfx_synth->state.clipped == true)
                printf("%s(): Warning, the sound was clipped.\n", __FUNCTION__);
#endif
            }
          free(wav);
        }
    }
#ifndef NDEBUG
  else
    fprintf(stderr, "%s(): Warning, empty synth sound.\n", __FUNCTION__);
#endif

  return rv;
}
