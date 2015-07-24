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

#include "cave.h"
#include "diamond_girl.h"
#include "gc.h"
#include "highscore.h"
#include "map.h"
#include "playback.h"
#include "treasureinfo.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


struct cave * cave_load(enum GAME_MODE game_mode, const char * cave_name)
{
  struct cave * cave;

  cave = malloc(sizeof(struct cave));
  assert(cave != NULL);
  if(cave != NULL)
    {
      cave->game_mode          = game_mode;
      cave->name               = strdup(cave_name);
      cave->level_count        = get_cave_level_count(cave->name);
      cave->max_starting_level = 1;
      cave->savegame.exists    = false;

      if(cave_name[0] == '/'       ||
         !strcmp(cave_name, "a")   ||
         !strcmp(cave_name, "beginners_cave"))
        cave->editable = false;
      else
        cave->editable = true;
      

      {
        FILE * fp;
        char fn[1024];


        if(game_mode == GAME_MODE_PYJAMA_PARTY)
          snprintf(fn, sizeof fn, "party/data%s", cave_filename(cave->name));
        else
          snprintf(fn, sizeof fn, "data%s", cave_filename(cave->name));
        fp = fopen(get_save_filename(fn), "rb");
        if(fp != NULL)
          {
            int n;
            int version;
            char header[4];
            
            version = 1; /* Version 1 did not have any header. */
            if(fread(header, 4, 1, fp) == 1) /* Header will always be 4 characters. */
              {
                if(memcmp(header, DIAMOND_GIRL_CAVEFILE_V2, 4) == 0)
                  version = 2;
                else if(memcmp(header, DIAMOND_GIRL_CAVEFILE_V3, 4) == 0)
                  version = 3;
                else if(memcmp(header, DIAMOND_GIRL_CAVEFILE_V4, 4) == 0)
                  version = 4;
              }
            
            if(version == 1)
              rewind(fp);
            
            if(fread(&n, sizeof n, 1, fp) == 1)
              cave->max_starting_level = n;
            else
              fprintf(stderr, "Warning! Failed to read from '%s': %s\n", get_save_filename(fn), strerror(errno));

            if(version >= 2)
              { /* Load the saved game. */
                if(fread(&cave->savegame.exists, sizeof cave->savegame.exists, 1, fp) != 1)
                  {
                    fprintf(stderr, "Warning! Failed to read savegame from '%s': %s\n", get_save_filename(fn), strerror(errno));
                    cave->savegame.exists = false;
                  }
                assert(cave->savegame.exists == true || cave->savegame.exists == false);

                if(cave->savegame.exists)
                  {
                    size_t buffer_length = 
                      sizeof cave->savegame.game_level          +
                      sizeof cave->savegame.game_speed_modifier +
                      sizeof cave->savegame.starting_girls      +
                      sizeof cave->savegame.girls               +
                      sizeof cave->savegame.score               +
                      sizeof cave->savegame.diamond_score       +
                      sizeof cave->savegame.diamonds;
                    unsigned char buffer[buffer_length];

                    cave->savegame.treasure = NULL;
                    if(fread(buffer, buffer_length, 1, fp) == 1)
                      {
                        unsigned char * pos;

                        pos = buffer;

                        memcpy(&cave->savegame.game_level, pos, sizeof cave->savegame.game_level);
                        pos += sizeof cave->savegame.game_level;

                        memcpy(&cave->savegame.game_speed_modifier, pos, sizeof cave->savegame.game_speed_modifier);
                        pos += sizeof cave->savegame.game_speed_modifier;

                        memcpy(&cave->savegame.starting_girls, pos, sizeof cave->savegame.starting_girls);
                        pos += sizeof cave->savegame.starting_girls;

                        memcpy(&cave->savegame.girls, pos, sizeof cave->savegame.girls);
                        pos += sizeof cave->savegame.girls;

                        memcpy(&cave->savegame.score, pos, sizeof cave->savegame.score);
                        pos += sizeof cave->savegame.score;

                        memcpy(&cave->savegame.diamond_score, pos, sizeof cave->savegame.diamond_score);
                        pos += sizeof cave->savegame.diamond_score;

                        memcpy(&cave->savegame.diamonds, pos, sizeof cave->savegame.diamonds);
                        pos += sizeof cave->savegame.diamonds;

                        if(cave->game_mode == GAME_MODE_PYJAMA_PARTY)
                          {
                            if(version < 3)
                              { /* Version 2 had this saved all wrong, so it can not be used. */
                                fprintf(stderr, "%s: Warning! Saved game version 2 can not be used.\n", get_save_filename(fn));
                                cave->savegame.exists = false;
                              }
                            else
                              {
                                cave->savegame.pyjama_party_girls = malloc(sizeof(bool) * cave->savegame.starting_girls);
                                assert(cave->savegame.pyjama_party_girls != NULL);
                                if(cave->savegame.pyjama_party_girls != NULL)
                                  if(fread(cave->savegame.pyjama_party_girls, sizeof(bool) * cave->savegame.starting_girls, 1, fp) != 1)
                                    {
                                      fprintf(stderr, "Warning! Failed to read savegame from '%s': %s\n", get_save_filename(fn), strerror(errno));
                                      cave->savegame.exists = false;
                                    }
                                if(cave->savegame.exists)
                                  if(fread(&cave->savegame.pyjama_party_girls_passed, sizeof cave->savegame.pyjama_party_girls_passed, 1, fp) != 1)
                                    {
                                      fprintf(stderr, "Warning! Failed to read savegame from '%s': %s\n", get_save_filename(fn), strerror(errno));
                                      cave->savegame.exists = false;
                                    }
                              }
                          }

                        if(version >= 4)
                          {
                            uint16_t len;

                            if(fread(&len, sizeof len, 1, fp) == 1)
                              {
                                if(len > 0)
                                  {
                                    char buf[len + 1];

                                    if(fread(buf, len, 1, fp) == 1)
                                      {
                                        buf[len] = '\0';
                                        cave->savegame.treasure = treasureinfo_load(buf);
                                      }
                                    else
                                      {
                                        fprintf(stderr, "Warning! Failed to read savegame from '%s': %s\n", get_save_filename(fn), strerror(errno));
                                        cave->savegame.exists = false;
                                      }
                                  }
                              }
                            else
                              {
                                fprintf(stderr, "Warning! Failed to read savegame from '%s': %s\n", get_save_filename(fn), strerror(errno));
                                cave->savegame.exists = false;
                              }
                          }
                      }
                    else
                      {
                        fprintf(stderr, "Warning! Failed to read savegame from '%s': %s\n", get_save_filename(fn), strerror(errno));
                        cave->savegame.exists = false;
                      }
                  }
              }

            fclose(fp);

            if(cave->savegame.exists)
              {
                char tmpfn[strlen(fn) + strlen(".dgp") + 1];

                snprintf(tmpfn, sizeof tmpfn, "%s.dgp", fn);
                cave->savegame.playback = playback_load(get_save_filename(tmpfn));
              }

            if(cave->savegame.exists && cave->game_mode == GAME_MODE_PYJAMA_PARTY)
              {
                char tmpfn[strlen(fn) + strlen(".dgms") + 1];

                snprintf(tmpfn, sizeof tmpfn, "%s.dgms", fn);
                cave->savegame.map = map_get(cave->name, cave->savegame.game_level);
                assert(cave->savegame.map != NULL);
                map_load_state(cave->savegame.map, get_save_filename(tmpfn));
              }
          }
      }

      cave->highscores = highscores_new();
      highscores_load(cave->highscores, cave->game_mode, cave->name);

      gc_new(GCT_CAVE, cave);
    }

  return cave;
}
