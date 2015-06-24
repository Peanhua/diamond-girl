/*
  Diamond Girl - Game where player collects diamonds.
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

#include <bzlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

bool playback_save(char const * const filename, struct playback * playback)
{
  assert(filename != NULL);
  assert(playback != NULL);

  bool success;
  FILE * fp;
  char fn[strlen(filename) + strlen(".bz2") + 1];

  success = true;

  strcpy(fn, filename);
  strcat(fn, ".bz2");

  fp = fopen(fn, "wb");
  if(fp != NULL)
    {
      BZFILE * bfp;
      int bzerror;

      bzerror = BZ_OK;

      bfp = BZ2_bzWriteOpen(&bzerror, fp, 1, 0, 0);
      if(bzerror == BZ_OK)
        {
          char buf[64];

#ifdef DEBUG_PLAYBACK
          snprintf(buf, sizeof buf, "DIAMOND GIRL PLAYBACK " PLAYBACK_FILE_VERSION "-debug\n");
#else
          snprintf(buf, sizeof buf, "DIAMOND GIRL PLAYBACK " PLAYBACK_FILE_VERSION "\n");
#endif
          BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));

          while(bzerror == BZ_OK && success == true && playback != NULL)
            {
              if(bzerror == BZ_OK)
                {
                  snprintf(buf, sizeof buf, "CAVE=%s\n", playback->cave);
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              if(bzerror == BZ_OK)
                {
                  snprintf(buf, sizeof buf, "GAME_MODE=%d\n", (int) playback->game_mode);
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              if(bzerror == BZ_OK)
                {
                  snprintf(buf, sizeof buf, "IRON_GIRL_MODE=%d\n", (int) (playback->iron_girl_mode ? 1 : 0));
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              if(bzerror == BZ_OK)
                {
                  snprintf(buf, sizeof buf, "TRAITS=%u\n", (unsigned int) playback->traits);
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              if(bzerror == BZ_OK)
                {
                  snprintf(buf, sizeof buf, "LEVEL=%d\n", playback->level);
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              if(bzerror == BZ_OK)
                {
                  snprintf(buf, sizeof buf, "NOT_FOR_AI=%d\n", (int) (playback->not_for_ai == true ? 1 : 0));
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              if(bzerror == BZ_OK)
                if(playback->map_hash != NULL)
                  {
                    snprintf(buf, sizeof buf, "MAP_HASH_MD5=%s\n", playback->map_hash);
                    BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                  }

              for(unsigned int i = 0; bzerror == BZ_OK && i < playback->steps_size; i++)
                {
#ifdef DEBUG_PLAYBACK
                  snprintf(buf, sizeof buf, "STEP=%d %d %d %d %d %d %d\n",
                          (int) playback->steps[i].timestamp,
                          (int) playback->steps[i].timestamp_timer,
                          (int) playback->steps[i].position[0],
                          (int) playback->steps[i].position[1],
                          (int) playback->steps[i].direction,
                          (int) playback->steps[i].manipulate ? 1 : 0,
                          (int) playback->steps[i].commit_suicide ? 1 : 0);
#else
                  snprintf(buf, sizeof buf, "STEP=%d %d %d %d %d\n",
                          (int) playback->steps[i].position[0],
                          (int) playback->steps[i].position[1],
                          (int) playback->steps[i].direction,
                          (int) playback->steps[i].manipulate ? 1 : 0,
                          (int) playback->steps[i].commit_suicide ? 1 : 0);
#endif
                  BZ2_bzWrite(&bzerror, bfp, buf, strlen(buf));
                }

              playback = playback->next;
            }

          if(bzerror != BZ_OK)
            {
              success = false;
              fprintf(stderr, "%s(): Compression write failed: %d\n", __FUNCTION__, bzerror);
            }

          BZ2_bzWriteClose(&bzerror, bfp, 0, NULL, NULL);
          if(bzerror != BZ_OK)
            {
              success = false;
              fprintf(stderr, "%s(): Compression write close failed: %d\n", __FUNCTION__, bzerror);
            }
        }
      else
        {
          success = false;
          fprintf(stderr, "%s(): Compression write initialization failed: %d\n", __FUNCTION__, bzerror);
        }
        
      fclose(fp);

      /* Don't keep broken files around. */
      if(success == false)
        unlink(fn);
    }
  else
    {
      fprintf(stderr, "%s(): Failed to open '%s' for writing: %s\n", __FUNCTION__, fn, strerror(errno));
      success = false;
    }

  return success;
}
