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
#include <AL/al.h>
#include <assert.h>
#include <stdio.h>

bool sfx_load(struct sfx * sfx)
{
  bool rv;

  rv = false;
  sfx->openal_buffer = AL_NONE;

#ifdef WITH_COMPRESSED_DATA
  if(rv == false)
    rv = sfx_load_ogg(sfx);
#endif

  if(rv == false)
    rv = sfx_load_file(sfx, NULL);

  if(rv == false)
    rv = sfx_load_xm(sfx);

  if(rv == false)
    {
      char fn[1024];

      snprintf(fn, sizeof fn, "sfx/%s.sfx", sfx->base_filename);
      sfx->synth = sfx_synth_load(fn);
      if(sfx->synth != NULL)
        {
          sfx->openal_buffer = sfx_synth_to_buffer(sfx->synth);
          if(sfx->openal_buffer != AL_NONE)
            rv = true;
        }
    }

  if(rv == false)
    fprintf(stderr, "%s(): Failed to load sfx '%s'.\n", __FUNCTION__, sfx->base_filename);
  assert(rv == true);

  return rv;
}
