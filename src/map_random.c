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

#include "diamond_girl.h"
#include "map.h"
#include "random.h"

#include <assert.h>

struct map * map_random(int level)
{
  struct map * map;

  map = NULL;
  level = level % 100;
  if(level == 0)
    {
      map = map_load("/random", 100);
    }
  else
    {
      void * rs;

      if(level < 26)
        rs = seed_rand_state(level +  1);
      else if(level < 45)
        rs = seed_rand_state(level +  2);
      else if(level < 46)
        rs = seed_rand_state(level +  3);
      else if(level < 51)
        rs = seed_rand_state(level +  4);
      else if(level < 53)
        rs = seed_rand_state(level +  5);
      else if(level < 61)
        rs = seed_rand_state(level +  6);
      else if(level < 76)
        rs = seed_rand_state(level +  7);
      else if(level < 78)
        rs = seed_rand_state(level +  8);
      else if(level < 91)
        rs = seed_rand_state(level +  9);
      else if(level < 92)
        rs = seed_rand_state(level + 11);
      else
        rs = seed_rand_state(level + 12);

      assert(rs != NULL);

      if(rs != NULL)
        {
          int w, h;
          int diamonds;

          w = 40 + level;
          if(w > 70)
            w = 70;
          h = 20 + level;
          if(h > 50)
            h = 50;
          map = map_new(w, h);
          assert(map != NULL);
          if(map != NULL)
            {
              map->cave_name = strdup("/random");
              map->level     = level;

              if(level > 75)
                map->diamond_score = 0;
              else if(level > 50)
                map->diamond_score = 1;
              else if(level > 40)
                map->diamond_score = 2;
              else
                map->diamond_score = 3;

              map->time_score = 1;

              diamonds = 0;
              for(int y = 0; y < map->height; y++)
                for(int x = 0; x < map->width; x++)
                  {
                    int r;

                    r = get_rand_state(100, rs);
                    if(r > 90)
                      map->data[x + y * map->width] = MAP_BOULDER;
                    else if(r > 85)
                      {
                        map->data[x + y * map->width] = MAP_DIAMOND;
                        diamonds++;
                      }
                    else if(r > 84)
                      {
                        map->data[x + y * map->width] = MAP_ENEMY1;
                      }
                    else if(r > 82)
                      {
                        map->data[x + y * map->width] = MAP_ENEMY2;
                      }
                    else if(r > 81)
                      {
                        map->data[x + y * map->width] = MAP_EMPTY;
                      }
                    else if(level > 2 && r > 80)
                      {
                        map->data[x + y * map->width] = MAP_BRICK;
                      }
                    else if(level > 3 && r > 79)
                      {
                        map->data[x + y * map->width] = MAP_BRICK_UNBREAKABLE;
                      }
                    else
                      {
                        if(level == 50 || level == 75)
                          map->data[x + y * map->width] = MAP_DIAMOND;
                        else
                          map->data[x + y * map->width] = MAP_SAND;
                      }
                  }
    
              map->diamonds_needed = diamonds / 2 + level * 2;
              if(map->diamonds_needed > diamonds)
                map->diamonds_needed = diamonds;

              map->start_x = 0;
              map->start_y = 0;
 
              do
                {
                  map->exit_x = get_rand_state(map->width, rs);
                  map->exit_y = get_rand_state(map->height, rs);
                } while(map->exit_x == map->start_x && map->exit_y == map->start_y);

              map->data[map->start_x + map->start_y * map->width] = MAP_PLAYER;
              map->data[map->exit_x + map->exit_y * map->width] = MAP_BRICK;

              map->time = 200;
              if(level < 30)
                map->time -= level * 2;
              else
                map->time += level;
              map->time += map->diamonds_needed * 2;
              if(map->time > 999)
                map->time = 999;

              map->ameba_time = map->time / 10;

              if(level < 90)
                map->game_speed = 3;
              else
                map->game_speed = 2;
            }

          free(rs);
        }
    }

  return map;
}
