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
#include "playback.h"
#include "traits.h"
#include "treasure.h"
#include "treasureinfo.h"
#include <assert.h>

struct playback * playback_load(char const * const filename)
{
  assert(filename != NULL);

  struct playback * rv, * current;
  char * inbuf;
  int    inbuf_pos;
  int    version; // 1 for 1.0 and 1.1 */

  rv = current = playback_new();
  assert(current != NULL);

  version = 1;
  
  /* These were the values for game_mode and traits, before they were saved in playbacks of releases 1.7 and earlier. */
  current->game_mode = GAME_MODE_ADVENTURE;
  current->traits    = TRAIT_ADVENTURE_MODE | TRAIT_RIBBON;

  if(read_file(filename, &inbuf, &inbuf_pos) == true)
    {
      enum
      {
        MODE_HEADER,
        MODE_DATA
      } mode = MODE_HEADER;
      bool done;
      char * inbuf_p;

      assert(inbuf != NULL);

      done = false;
      inbuf_p = inbuf;
      while(done == false)
        {
          char * buf;

          buf = inbuf_p;
          while(*inbuf_p != '\n' && *inbuf_p != '\0')
            inbuf_p++;

          if(*inbuf_p == '\n')
            { /* Split string and advance the p pointer for the next line. */
              *inbuf_p = '\0';
              inbuf_p++;
            }
          else
            { /* EOF reached
               * The file always ends with a linefeed, therefore we know, that when the end has reached, there is nothing in the buf.
               */
              done = true;
            }

          if(done == false) /* Here we assume that there is nothing in the buf once the EOF has been reached. */
            switch(mode)
              {
              case MODE_HEADER:
                {
                  char * header_1_0 = "DIAMOND GIRL PLAYBACK 1.0"
#ifdef DEBUG_PLAYBACK
                    "-debug"
#endif
                    ;
                  char * header_1_1 = "DIAMOND GIRL PLAYBACK 1.1"
#ifdef DEBUG_PLAYBACK
                    "-debug"
#endif
                    ;
                  char * header2 = "DIAMOND GIRL PLAYBACK 2"
#ifdef DEBUG_PLAYBACK
                    "-debug"
#endif
                    ;

                  if(!strcmp(buf, header2))
                    {
                      version = 2;
                      mode = MODE_DATA;
                    }
                  else if(!strcmp(buf, header_1_1) || !strcmp(buf, header_1_0))
                    {
                      mode = MODE_DATA;
                    }
                  else
                    {
                      fprintf(stderr, "%s('%s'): Illegal header: %s\n", __FUNCTION__, filename, buf);
                      rv = playback_free(rv);
                      done = true;
                    }
                }
                break;
              case MODE_DATA:
                {
                  char * var, * val;
                        
                  var = buf;
                  val = strchr(buf, '=');
                  if(val != NULL)
                    {
                      *val = '\0';
                      val++;
                    }
                  if(var != NULL && val != NULL)
                    {
                      if(!strcmp(var, "MAP_SET") || !strcmp(var, "CAVE"))
                        {
                          if(current->cave != NULL)
                            { /* Switch to next playback batch. */
                              current->next = playback_new();
                              current = current->next;
                              assert(current != NULL);
                              /* These were the values for game_mode and traits, before they were saved in playbacks of releases 1.7 and earlier. */
                              current->game_mode = GAME_MODE_ADVENTURE;
                              current->traits    = TRAIT_ADVENTURE_MODE | TRAIT_RIBBON;
                            }
                          if(!strcmp(val, "random"))
                            current->cave = strdup("/random"); /* To be backwards-compatible with old playbacks. */
                          else
                            current->cave = strdup(val);
                        }
                      else if(!strcmp(var, "GAME_MODE"))
                        {
                          current->game_mode = strtol(val, NULL, 0);
                        }
                      else if(!strcmp(var, "IRON_GIRL_MODE"))
                        {
                          current->iron_girl_mode = strtoul(val, NULL, 0) == 0 ? false : true;
                        }
                      else if(!strcmp(var, "TRAITS"))
                        {
                          current->traits = strtoul(val, NULL, 0);
                        }
                      else if(!strcmp(var, "LEVEL"))
                        {
                          current->level = strtol(val, NULL, 0);
                        }
                      else if(!strcmp(var, "NOT_FOR_AI"))
                        {
                          current->not_for_ai = strtoul(val, NULL, 0) == 0 ? false : true;
                        }
                      else if(!strcmp(var, "MAP_HASH_MD5"))
                        {
                          current->map_hash = strdup(val);
                        }
                      else if(!strcmp(var, "STEP"))
                        {
                          struct playback_step * step;
                          char * p;
                                    
                          step = playback_append_step(current);
                          assert(step != NULL);

#ifdef DEBUG_PLAYBACK
                          step->timestamp       = strtol(val, &p, 0);                     val = p;
                          step->timestamp_timer = strtol(val, &p, 0);                     val = p;
#endif
                          step->position[0]     = strtol(val, &p, 0);                     val = p;
                          step->position[1]     = strtol(val, &p, 0);                     val = p;
                          step->direction       = strtol(val, &p, 0);                     val = p;
                          step->manipulate      = strtol(val, &p, 0) == 1 ? true : false; val = p;
                          step->commit_suicide  = strtol(val, &p, 0) == 1 ? true : false; val = p;
                        }
                      else if(!strcmp(var, "TREASURE"))
                        {
                          assert(version >= 2);
                          if(version >= 2)
                            current->treasure = treasureinfo_load(val);
                        }
                    }
                  else
                    fprintf(stderr, "%s('%s'): Ignoring suspicious line: %s\n", __FUNCTION__, filename, buf);
                }
                break;
              }
        }

      free(inbuf);
    }
  else
    {
      rv = playback_free(rv);
    }

  return rv;
}
