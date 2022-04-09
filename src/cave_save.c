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

#include "cave.h"
#include "diamond_girl.h"
#include "map.h"
#include "playback.h"
#include "treasureinfo.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>


bool cave_save(struct cave * cave)
{
  FILE * fp;
  char fn[1024];
  bool success;

  success = true;

  if(cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    snprintf(fn, sizeof fn, "party/data%s", cave_filename(cave->name));
  else
    snprintf(fn, sizeof fn, "data%s", cave_filename(cave->name));
            
  fp = fopen(get_save_filename(fn), "wb");
  if(fp != NULL)
    {
      size_t buffer_length = 4 + sizeof cave->max_starting_level + sizeof cave->savegame.exists;

      if(cave->savegame.exists == true)
        {
          buffer_length +=
            sizeof cave->savegame.game_level                +
            sizeof cave->savegame.game_speed_modifier       +
            sizeof cave->savegame.starting_girls            +
            sizeof cave->savegame.girls                     +
            sizeof cave->savegame.score                     +
            sizeof cave->savegame.diamond_score             +
            sizeof cave->savegame.diamonds                  +
            sizeof(bool) * cave->savegame.starting_girls    +
            sizeof cave->savegame.pyjama_party_girls_passed
            ;
        }

      unsigned char buffer[buffer_length];
      unsigned char * pos;

      pos = buffer;

      memcpy(pos, DIAMOND_GIRL_CAVEFILE_V4, 4);
      pos += 4;

      memcpy(pos, &cave->max_starting_level, sizeof cave->max_starting_level);
      pos += sizeof cave->max_starting_level;

      memcpy(pos, &cave->savegame.exists, sizeof cave->savegame.exists);
      pos += sizeof cave->savegame.exists;

      if(cave->savegame.exists)
        {
          memcpy(pos, &cave->savegame.game_level, sizeof cave->savegame.game_level);
          pos += sizeof cave->savegame.game_level;

          memcpy(pos, &cave->savegame.game_speed_modifier, sizeof cave->savegame.game_speed_modifier);
          pos += sizeof cave->savegame.game_speed_modifier;

          memcpy(pos, &cave->savegame.starting_girls, sizeof cave->savegame.starting_girls);
          pos += sizeof cave->savegame.starting_girls;

          memcpy(pos, &cave->savegame.girls, sizeof cave->savegame.girls);
          pos += sizeof cave->savegame.girls;

          memcpy(pos, &cave->savegame.score, sizeof cave->savegame.score);
          pos += sizeof cave->savegame.score;

          memcpy(pos, &cave->savegame.diamond_score, sizeof cave->savegame.diamond_score);
          pos += sizeof cave->savegame.diamond_score;

          memcpy(pos, &cave->savegame.diamonds, sizeof cave->savegame.diamonds);
          pos += sizeof cave->savegame.diamonds;

          if(cave->game_mode == GAME_MODE_PYJAMA_PARTY)
            {
              assert(cave->savegame.pyjama_party_girls != NULL);
              memcpy(pos, cave->savegame.pyjama_party_girls, sizeof(bool) * cave->savegame.starting_girls);
              pos += sizeof(bool) * cave->savegame.starting_girls;

              memcpy(pos, &cave->savegame.pyjama_party_girls_passed, sizeof cave->savegame.pyjama_party_girls_passed);
              pos += sizeof cave->savegame.pyjama_party_girls_passed;
            }
        }

      if(fwrite(buffer, pos - buffer, 1, fp) != 1)
        {
          fprintf(stderr, "Warning! Failed to write to '%s': %s\n", get_save_filename(fn), strerror(errno));
          success = false;
        }          


      if(cave->savegame.exists)
        if(success == true)
          { /* Save the treasure. */
            uint16_t len;
            char * treasuredata;
          
            if(cave->savegame.treasure != NULL)
              {
                assert(cave->game_mode == GAME_MODE_ADVENTURE);
                treasuredata = treasureinfo_save(cave->savegame.treasure);
              }
            else
              treasuredata = NULL;
          
            if(treasuredata != NULL)
              len = strlen(treasuredata);
            else
              len = 0;

            if(fwrite(&len, sizeof len, 1, fp) == 1)
              {
                if(len > 0)
                  if(fwrite(treasuredata, len, 1, fp) != 1)
                    {
                      fprintf(stderr, "Warning! Failed to write to '%s': %s\n", get_save_filename(fn), strerror(errno));
                      success = false;
                    }
              }
            else
              {
                fprintf(stderr, "Warning! Failed to write to '%s': %s\n", get_save_filename(fn), strerror(errno));
                success = false;
              }

            free(treasuredata);
          }
              
      
      if(fclose(fp) != 0)
        {
          fprintf(stderr, "Warning! Failed to write to '%s': %s\n", get_save_filename(fn), strerror(errno));
          success = false;
        }

      if(success && cave->savegame.exists)
        if(cave->savegame.playback != NULL)
          {
            char tmpfn[strlen(fn) + strlen(".dgp") + 1];

            snprintf(tmpfn, sizeof tmpfn, "%s.dgp", fn);
            success = playback_save(get_save_filename(tmpfn), cave->savegame.playback);
          }

      if(success && cave->savegame.exists)
        if(cave->game_mode == GAME_MODE_PYJAMA_PARTY)
          {
            char tmpfn[strlen(fn) + strlen(".dgms") + 1];

            snprintf(tmpfn, sizeof tmpfn, "%s.dgms", fn);
            assert(cave->savegame.map != NULL);
            success = map_save_state(cave->savegame.map, get_save_filename(tmpfn));
          }
    }
  else
    {
      fprintf(stderr, "Warning! Failed to open '%s' for writing: %s\n", get_save_filename(fn), strerror(errno));
      success = false;
    }

  return success;
}
