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


#include "diamond_girl.h"
#include "sfx.h"
#include <vorbis/vorbisfile.h>
#include <assert.h>

bool sfx_load_ogg(struct sfx * sfx)
{
  bool rv;
  int err;
  OggVorbis_File oggfp;
  char fn[1024];

  rv = false;

  snprintf(fn, sizeof fn, "sfx/%s.ogg", sfx->base_filename);
  
  err = ov_fopen(get_data_filename(fn), &oggfp);
  if(err == 0)
    {
      bool done;
      int pos;
      char * wav;
      size_t wav_len;

      done = false;
      pos = 0;
      wav = NULL;
      wav_len = 0;
      while(done == false)
        {
          long rlen;
          char buf[4096];

          rlen = ov_read(&oggfp, buf, sizeof buf, 0, 2, 1, &pos);
          if(rlen > 0)
            {
              char * tmp;

              tmp = realloc(wav, wav_len + rlen);
              assert(tmp != NULL);
              if(tmp != NULL)
                {
                  wav = tmp;
                  memcpy(wav + wav_len, buf, rlen);
                  wav_len += rlen;
                }
            }
          else
            done = true;
        }

      assert(wav != NULL);

      vorbis_info * info;

      info = ov_info(&oggfp, -1);
      assert(info->channels == 1);
      alGenBuffers(1, &sfx->openal_buffer);
      alBufferData(sfx->openal_buffer, AL_FORMAT_MONO16, wav, wav_len, info->rate);
      rv = true;

      free(wav);
      ov_clear(&oggfp);
    }
  
  return rv;
}
