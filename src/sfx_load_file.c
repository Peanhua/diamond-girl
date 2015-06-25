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


bool sfx_load_file(struct sfx * sfx, const char * filename)
{
  bool rv;
  char fn[1024];

  rv = false;

  if(filename != NULL)
    snprintf(fn, sizeof fn, "%s", filename);
  else
    snprintf(fn, sizeof fn, "%s/%s.wav", get_data_filename("sfx"), sfx->base_filename);

  SNDFILE * fp;
  SF_INFO info;

  info.format = 0;
  fp = sf_open(fn, SFM_READ, &info);
  if(fp != NULL)
    {
      bool done;
      int16_t * wav;
      int wavlen;

      done = false;
      wav = NULL;
      wavlen = 0;
      while(done == false)
        {
          int n;
          float buf[128];

          n = sf_read_float(fp, buf, 128);
          if(n > 0)
            {
              int16_t * tmp;

              tmp = realloc(wav, (wavlen + n) * sizeof(int16_t));
              assert(tmp != NULL);
              if(tmp != NULL)
                {
                  wav = tmp;
                  for(int i = 0; i < n; i++)
                    wav[wavlen + i] = buf[i] * 32767.0f;
                  wavlen += n;
                }
            }
          if(n < 128)
            done = true;
        }
      
      sf_close(fp);

      alGenBuffers(1, &sfx->openal_buffer);
      alBufferData(sfx->openal_buffer, AL_FORMAT_MONO16, wav, wavlen * sizeof(int16_t), 44100);
      rv = true;

      free(wav);
    }
  
  return rv;
}
