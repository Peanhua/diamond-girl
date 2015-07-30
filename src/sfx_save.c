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
#include "sfx.h"
#include <sndfile.h>
#include <assert.h>

bool sfx_save(struct sfx * sfx, char const * filename)
{
  bool success;

  success = false;
  if(sfx != NULL && sfx->waveform != NULL)
    {
      SNDFILE * fp;
      SF_INFO info;

      info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
      info.samplerate = 44100;
      info.channels = 1;
      fp = sf_open(filename, SFM_WRITE, &info);
      if(fp != NULL)
        {
          success = true;
          sf_set_string(fp, SF_STR_TITLE, sfx->base_filename);
          sf_set_string(fp, SF_STR_SOFTWARE, PACKAGE_STRING);
          for(uint32_t i = 0; i < sfx->waveform_length; i++)
            {
              short tmp;

              tmp = sfx->waveform[i];
              if(sf_write_short(fp, &tmp, 1) != 1)
                {
                  success = false;
                  fprintf(stderr, "%s(): Failed.\n", __FUNCTION__);
                }
            }
          sf_close(fp);
        }
    }

  assert(success == true);
  return success;
}
