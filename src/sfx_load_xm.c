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
#include <unistd.h>
#include <mikmod.h>
#include <string.h>
#include <assert.h>

bool sfx_load_xm(struct sfx * sfx)
{
  bool rv;
  char fn[1024];

  rv = false;
  snprintf(fn, sizeof fn, "sfx/%s.xm", sfx->base_filename);
  if(access(get_data_filename(fn), R_OK) == 0)
    {
      /* Generate .wav from .xm */
      char * drivers, * pos;
      int driver_found;

      drivers = MikMod_InfoDriver();

      pos = drivers;
      driver_found = 0;
      while(pos != NULL && !driver_found)
        {
          int n;
          char * ep;

          n = strtoul(pos, &ep, 10);
          pos = ep;
          if(pos != NULL)
            {
              char * eol;
              char const * const drivername = " Wav disk writer ";

              eol = strchr(pos, '\n');
              if(!strncmp(pos, drivername, strlen(drivername)))
                {
                  driver_found = 1;
                  md_device = n;
                  *eol = '\0';
                }

              if(eol != NULL)
                pos = eol + 1;
              else
                pos = NULL;
            }
        }

      md_mode |= DMODE_16BITS | DMODE_HQMIXER | DMODE_SOFT_MUSIC | DMODE_SOFT_SNDFX | DMODE_INTERP;
      md_mode &= ~DMODE_STEREO;
      md_reverb = 0; /* todo */
      md_pansep = 0;

      char params[1024];

      snprintf(params, sizeof params, "file=%s/%s.wav", get_user_homedir(), sfx->base_filename);
      if(!MikMod_Init(params))
        {
          MODULE * module;
     
          module = Player_Load((char *) get_data_filename(fn), 8, 0);
          if(module != NULL)
            {
              Player_Start(module);
              while (Player_Active())
                MikMod_Update();
          
              Player_Stop();
              Player_Free(module);
            }
      
          MikMod_Exit();

          /* Load the generated .wav */
          snprintf(fn, sizeof fn, "%s/%s.wav", get_user_homedir(), sfx->base_filename);
          rv = sfx_load_file(sfx, fn);
          assert(rv == true);
      
          remove(fn);
        }
      else
        {
          fprintf(stderr, "%s(): MikMod_Init() failed: %s\n", __FUNCTION__, MikMod_strerror(MikMod_errno));
        }
#ifdef HAVE_MIKMOD_FREE
      MikMod_free(drivers);
#endif
    }

  return rv;
}
