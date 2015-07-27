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

#include "sfx.h"
#include "sfx_synth.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

bool sfx_load_synth(struct sfx * sfx)
{
  bool rv;
  char fn[1024];

  rv = false;
  snprintf(fn, sizeof fn, "sfx/%s.sfx", sfx->base_filename);
  sfx->synth = sfx_synth_load(fn);
  if(sfx->synth != NULL)
    {
      int16_t tmp;

      rv = AL_NONE;
      sfx->waveform_length = 0;
      while(sfx_synth_get_samples(sfx->synth, &tmp, 1) == 1)
        sfx->waveform_length++;

      if(sfx->waveform_length > 0)
        {
          sfx->waveform = malloc(sizeof(int16_t) * sfx->waveform_length);
          assert(sfx->waveform != NULL);
          if(sfx->waveform != NULL)
            {
              sfx_synth_restart(sfx->synth);

#ifndef NDEBUG
              uint32_t nlen;

              nlen =
#endif
                sfx_synth_get_samples(sfx->synth, sfx->waveform, sfx->waveform_length);
              assert(nlen == sfx->waveform_length);

              if(sfx->waveform_length > 0)
                {
                  alGenBuffers(1, &sfx->openal_buffer);
                  alBufferData(sfx->openal_buffer, AL_FORMAT_MONO16, sfx->waveform, sfx->waveform_length * sizeof(int16_t), 44100);

                  rv = true;
#ifndef NDEBUG
                  if(sfx->synth->state.clipped == true)
                    {
                      printf("%s(): Warning, the sound was clipped.\n", __FUNCTION__);
                      rv = false;
                    }
#endif
                }
            }
        }
      else
        {
          fprintf(stderr, "%s(): Error in '%s', zero length for a synth sound.\n", __FUNCTION__, fn);
          assert(0);
        }
    }
  else
    {
      fprintf(stderr, "%s(): Error in '%s', empty synth sound.\n", __FUNCTION__, fn);
      assert(0);
    }

  return rv;
}
