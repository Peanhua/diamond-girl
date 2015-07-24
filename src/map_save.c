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

#include "map.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void map_save(struct map * map)
{
  assert(map->cave_name != NULL);
  assert(map->cave_name[0] != '/'); /* Cave names starting with a slash (/) are generated programmatically. */

  FILE * fp;
  char filename[1024];

  snprintf(filename, sizeof filename, "maps/%s", map->cave_name);
#ifdef WIN32
  mkdir(get_data_filename(filename));
#else
  mkdir(get_data_filename(filename), S_IRWXU);
#endif
  
  snprintf(filename, sizeof filename, "maps/%s/%d", map->cave_name, (int) map->level);
  fp = fopen(get_data_filename(filename), "w");
  if(fp != NULL)
    {
      char glyphs[MAP_SIZEOF_];
      char start, end;

      for(int i = 0; i < MAP_SIZEOF_; i++)
        glyphs[i] = '-';

      glyphs[MAP_DIAMOND]           = '!';
      glyphs[MAP_SAND]              = '%';
      glyphs[MAP_BOULDER]           = '*';
      glyphs[MAP_BRICK]             = '#';
      glyphs[MAP_BRICK_EXPANDING]   = 'z';
      glyphs[MAP_BRICK_MORPHER]     = 'x';
      glyphs[MAP_BRICK_UNBREAKABLE] = 'Z';
      glyphs[MAP_AMEBA]             = 'a';
      glyphs[MAP_SLIME]             = 's';
      glyphs[MAP_EMPTY]             = '.';
      glyphs[MAP_ENEMY1]            = '1';
      glyphs[MAP_ENEMY2]            = '2';
      start                         = 'S';
      end                           = 'E';
      

      fprintf(fp, "# Lucy the Diamond Girl map v1.0\n");
      fprintf(fp, "IS_INTERMISSION=%d\n", (int) (map->is_intermission == true ? 1 : 0));
      fprintf(fp, "TIME=%d\n",            (int) map->time);
      fprintf(fp, "GAME_SPEED=%d\n",      (int) map->game_speed);
      fprintf(fp, "AMEBA_TIME=%d\n",      (int) map->ameba_time);
      fprintf(fp, "DIAMOND_SCORE=%d\n",   (int) map->diamond_score);
      fprintf(fp, "TIME_SCORE=%d\n",      (int) map->time_score);
      fprintf(fp, "DIAMONDS_NEEDED=%d\n", (int) map->diamonds_needed);
      fprintf(fp, "WIDTH=%d\n",           (int) map->width);
      fprintf(fp, "HEIGHT=%d\n",          (int) map->height);
      fprintf(fp, "BACKGROUND_TYPE=%d\n", (int) map->background_type);
      fprintf(fp, "\n");
      fprintf(fp, "DIAMOND=%c\n",           glyphs[MAP_DIAMOND]);
      fprintf(fp, "SAND=%c\n",              glyphs[MAP_SAND]);
      fprintf(fp, "BOULDER=%c\n",           glyphs[MAP_BOULDER]);
      fprintf(fp, "BRICK=%c\n",             glyphs[MAP_BRICK]);
      fprintf(fp, "BRICK_EXPANDING=%c\n",   glyphs[MAP_BRICK_EXPANDING]);
      fprintf(fp, "BRICK_MORPHER=%c\n",     glyphs[MAP_BRICK_MORPHER]);
      fprintf(fp, "BRICK_UNBREAKABLE=%c\n", glyphs[MAP_BRICK_UNBREAKABLE]);
      fprintf(fp, "AMEBA=%c\n",             glyphs[MAP_AMEBA]);
      fprintf(fp, "SLIME=%c\n",             glyphs[MAP_SLIME]);
      fprintf(fp, "EMPTY=%c\n",             glyphs[MAP_EMPTY]);
      fprintf(fp, "ENEMY1=%c\n",            glyphs[MAP_ENEMY1]);
      fprintf(fp, "ENEMY2=%c\n",            glyphs[MAP_ENEMY2]);
      fprintf(fp, "START=%c\n",             start);
      fprintf(fp, "END=%c\n",               end);
      fprintf(fp, "\n");
      fprintf(fp, "DATA=\n");
      for(int y = 0; y < map->height; y++)
        {
          for(int x = 0; x < map->width; x++)
            {
              int c;

              if(map->start_x == x && map->start_y == y)
                c = start;
              else if(map->exit_x == x && map->exit_y == y)
                c = end;
              else
                c = glyphs[map->data[x + y * map->width]];
              fprintf(fp, "%c", c);
            }
          fprintf(fp, "\n");
        }
      
      fclose(fp);
    }
  else
    fprintf(stderr, "Failed to open '%s' for writing: %s\n", get_data_filename(filename), strerror(errno));
}
