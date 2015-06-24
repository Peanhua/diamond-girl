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

#include "map.h"
#include "diamond_girl.h"
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

struct map * map_load(const char * cave, int level)
{
  char *       inbuf;
  int          inbuf_len;
  struct map * map;
  char         filename[1024];
  const char * cavefn;

  map = NULL;
  assert(cave != NULL);
  if(cave[0] == '/')
    cavefn = cave + 1;
  else
    cavefn = cave;
  snprintf(filename, sizeof filename, "maps/%s/%d", cavefn, (int) level);
  
  if(read_file(get_data_filename(filename), &inbuf, &inbuf_len) == true)
    {
      int file_done;
      int in_data, data_line;
      enum MAP_GLYPH ascii_glyphs[0xff];
      char start, end;
      struct map * tmpmap;
      int inbuf_pos;

      for(int i = 0; i < 0xff; i++)
        ascii_glyphs[i] = MAP_BORDER;

      tmpmap = map_new(4, 4);

      tmpmap->width = -1;
      tmpmap->height = -1;
      tmpmap->start_x = -1;
      tmpmap->start_y = -1;
      tmpmap->exit_x = -1;
      tmpmap->exit_y = -1;
      tmpmap->diamonds_needed = -1;
      tmpmap->diamond_score = -1;
      tmpmap->time_score = -1;
      tmpmap->time = -1;
      tmpmap->ameba_time = -1;
      tmpmap->game_speed = -1;

      start = '\0';
      end = '\0';

      file_done = 0;
      in_data   = 0;
      data_line = 0;
      inbuf_pos = 0;
      while(!file_done && inbuf_pos < inbuf_len)
        {
          char linebuf[1024];
          char * lf;
              
          snprintf(linebuf, sizeof linebuf, "%s", inbuf + inbuf_pos);

          lf = strchr(linebuf, '\n');
          if(lf != NULL)
            *lf = '\0';

          inbuf_pos += strlen(linebuf) + 1;

          if(!in_data)
            {
              char var[128];
              char *val;
              int i;
              
              for(i = 0; linebuf[i] != '\0' && linebuf[i] != '='; i++)
                var[i] = linebuf[i];
              var[i] = '\0';
              i++;
              val = &linebuf[i];

              if(!strcmp(var, "IS_INTERMISSION"))
                {
                  tmpmap->is_intermission = strtoul(val, NULL, 0) == 0 ? false : true;
                }
              else if(!strcmp(var, "TIME"))
                {
                  tmpmap->time = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "GAME_SPEED"))
                {
                  tmpmap->game_speed = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "AMEBA_TIME"))
                {
                  tmpmap->ameba_time = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "DIAMONDS_NEEDED"))
                {
                  tmpmap->diamonds_needed = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "DIAMOND_SCORE"))
                {
                  tmpmap->diamond_score = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "TIME_SCORE"))
                {
                  tmpmap->time_score = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "WIDTH"))
                {
                  tmpmap->width = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "HEIGHT"))
                {
                  tmpmap->height = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "BACKGROUND_TYPE"))
                {
                  tmpmap->background_type = strtoul(val, NULL, 0);
                }
              else if(!strcmp(var, "DIAMOND"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_DIAMOND;
                }
              else if(!strcmp(var, "SAND"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_SAND;
                }
              else if(!strcmp(var, "BOULDER"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_BOULDER;
                }
              else if(!strcmp(var, "BRICK"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_BRICK;
                }
              else if(!strcmp(var, "BRICK_EXPANDING"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_BRICK_EXPANDING;
                }
              else if(!strcmp(var, "BRICK_MORPHER"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_BRICK_MORPHER;
                }
              else if(!strcmp(var, "BRICK_UNBREAKABLE"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_BRICK_UNBREAKABLE;
                }
              else if(!strcmp(var, "AMEBA"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_AMEBA;
                }
              else if(!strcmp(var, "SLIME"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_SLIME;
                }
              else if(!strcmp(var, "EMPTY"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_EMPTY;
                }
              else if(!strcmp(var, "ENEMY1"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_ENEMY1;
                }
              else if(!strcmp(var, "ENEMY2"))
                {
                  ascii_glyphs[(int) val[0]] = MAP_ENEMY2;
                }
              else if(!strcmp(var, "START"))
                {
                  start = val[0];
                }
              else if(!strcmp(var, "END"))
                {
                  end = val[0];
                }
              else if(!strcmp(var, "DATA"))
                {
                  in_data = 1;
                      
                  assert(tmpmap->width > 0 && tmpmap->height > 0);
                  map = map_new(tmpmap->width, tmpmap->height);
                  if(map != NULL)
                    {
                      map->level     = level;
                      map->cave_name = strdup(cave);
                      assert(map->cave_name != NULL);
                      if(map->cave_name != NULL)
                        {
                          map->is_intermission = tmpmap->is_intermission;
                          map->time            = tmpmap->time;
                          map->game_speed      = tmpmap->game_speed;
                          map->ameba_time      = tmpmap->ameba_time;
                          map->diamonds_needed = tmpmap->diamonds_needed;
                          map->diamond_score   = tmpmap->diamond_score;
                          map->time_score      = tmpmap->time_score;
                          map->width           = tmpmap->width;
                          map->height          = tmpmap->height;
                          map->background_type = tmpmap->background_type;
                        }
                      else
                        {
                          fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
                          map = map_free(map);
                          file_done = 1;
                        }
                    }
                }
            }
          else
            { /* In DATA */
              if((int) strlen(linebuf) == map->width)
                {
                  if(data_line < map->height)
                    {
                      for(int i = 0; map != NULL && i < map->width; i++)
                        {
                          if(linebuf[i] == start)
                            {
                              map->start_x = i;
                              map->start_y = data_line;
                            }
                          else if(linebuf[i] == end)
                            {
                              map->exit_x = i;
                              map->exit_y = data_line;
                            }
                          else
                            {
                              if(ascii_glyphs[(int) linebuf[i]] == MAP_BORDER)
                                {
                                  fprintf(stderr, "Failed to load map '%s', illegal character '%c' at %d, linebuf='%s'\n",
                                          get_data_filename(filename), linebuf[i], (int) i, linebuf);
                                  map = map_free(map);
                                  file_done = 1;
                                }
                              map->data[i + data_line * map->width] = ascii_glyphs[(int) linebuf[i]];
                            }
                        }
                      data_line++;
                    }
                  else
                    {
                      fprintf(stderr, "Failed to load map '%s': too many mapdata rows.\n", get_data_filename(filename));
                      map = map_free(map);
                      file_done = 1;
                    }
                }
            }
        }

      /* Check some stuff about the read map. */
      if(map != NULL && data_line < map->height)
        {
          fprintf(stderr, "Failed to load map '%s': missing %d lines of mapdata\n", get_data_filename(filename), (int) map->height - data_line);
          map = map_free(map);
        }
      if(map != NULL && map->data == NULL)
        {
          fprintf(stderr, "Failed to load map '%s': missing data\n", get_data_filename(filename));
          map = map_free(map);
        }
      if(map != NULL && map->width == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "width");
          map = map_free(map);
        }
      if(map != NULL && map->height == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "height");
          map = map_free(map);
        }
      if(map != NULL && map->start_x == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "start location");
          map = map_free(map);
        }
      if(map != NULL && map->exit_x == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "end location");
          map = map_free(map);
        }
      if(map != NULL && map->diamonds_needed == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "diamonds needed");
          map = map_free(map);
        }
      if(map != NULL && map->time == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "time");
          map = map_free(map);
        }		      
      if(map != NULL && map->game_speed == -1)
        {
          map->game_speed = 3;
          fprintf(stderr, "Warning, failed to load map '%s': missing game speed, speed set to default %d\n",
                  get_data_filename(filename),
                  (int) map->game_speed);
        }		      
      if(map != NULL && map->ameba_time == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "ameba_time");
          map = map_free(map);
        }		      
      if(map != NULL && map->diamond_score == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "diamond score");
          map = map_free(map);
        }
      if(map != NULL && map->time_score == -1)
        {
          fprintf(stderr, "Failed to load map '%s': missing %s\n", get_data_filename(filename), "time score");
          map = map_free(map);
        }
  
      if(tmpmap != NULL)
        tmpmap = map_free(tmpmap);
      
      if(map != NULL)
        {
          map->data[map->start_x + map->start_y * map->width] = MAP_PLAYER;
          map->data[map->exit_x + map->exit_y * map->width] = MAP_BRICK;
        }

      free(inbuf);
    }
  
  return map;
}
