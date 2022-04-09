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

#include "map.h"
#include "random.h"
#include <assert.h>

struct map * map_well(int level)
{
  struct map * map;
  int levelmult;

  levelmult = level / 30;
  level = level % 30;
  if(level == 0)
    level = 30;

  if(level == 1)
    {
      map = map_load("/well", 1);
    }
  else
    {
      bool bonuslevel;

      if(level % 5 == 0)
        bonuslevel = true;
      else
        bonuslevel = false;

      if(bonuslevel == false)
        map = map_new(30, 30 + level * 3 + levelmult * 5);
      else
        map = map_new(30 + 30 + level * 3 + levelmult * 5, 30);

      assert(map != NULL);
      if(map != NULL)
        {
          void * rs;

          rs = seed_rand_state(level);
          assert(rs != NULL);

          map->cave_name       = strdup("/well");
          map->level           = level;
          map->is_intermission = bonuslevel;

          map->diamond_score = 1;
          map->time_score    = 1;
      
          for(int y = 0; y < map->height; y++)
            for(int x = 0; x < map->width; x++)
              {
                enum MAP_GLYPH g;

                g = MAP_SIZEOF_;

                if(x < 3 || x > map->width - 3 || y == 0 || y == map->height - 1)
                  g = MAP_BRICK_UNBREAKABLE;
                else if(x >= 27 && x < 30)
                  {
                    if(bonuslevel == false)
                      g = MAP_BRICK_UNBREAKABLE;
                    else
                      {
                        int r;
                        
                        r = abs(map->height / 2 - y) * 1000 / map->height - get_rand_state(100, rs);
                        if(r < -20)
                          g = MAP_EMPTY;
                        else if(r < -10)
                          g = MAP_SAND;
                        else if(r < 200)
                          g = MAP_BRICK;
                        else
                          g = MAP_BRICK_UNBREAKABLE;
                      }
                  }

                if(g == MAP_SIZEOF_)
                  {
                    if(y > 1 && y < map->height - 3)
                      {
                        int r;
                        
                        r = get_rand_state(1000, rs);
                        if(bonuslevel == false || x < 30)
                          {
                            if(r < 35 && y > map->height * 2 / 3 && level > 5)
                              g = MAP_ENEMY1;
                            else if(r < 40 && y > 4)
                              g = MAP_ENEMY2;
                          }
                        else
                          {
                            if(x > 30 && r < 90 + level)
                              g = MAP_DIAMOND;
                          }

                        if(g == MAP_SIZEOF_)
                          {
                            if(r < 100 && y < map->height - 10)
                              g = MAP_BOULDER;
                            else if(r < 300 + y + level * 7)
                              g = MAP_EMPTY;
                            else if(r >= 999 - level / 2)
                              g = MAP_BRICK;
                            else
                              g = MAP_SAND;
                          }
                      }
                    else
                      g = MAP_SAND;
                  }

                map->data[x + y * map->width] = g;
              }

          map->diamonds_needed = 47 + level * 3;
          map->start_x = 10;
          map->start_y = 0;
          map->exit_x = 10;
          map->exit_y = map->height - 1;

          map->data[map->start_x + map->start_y * map->width] = MAP_PLAYER;
          map->data[map->exit_x + map->exit_y * map->width] = MAP_BRICK;

      
          map->time = 200 + level * 5;
          map->game_speed = 4;

          free(rs);
        }
    }

  if(map != NULL)
    {
      /* Level multiplier effects. */
      if(levelmult > 0)
        map->game_speed -= 1;

      map->time -= levelmult * 5;
      if(map->time < 150)
        map->time = 150;
      
      map->diamond_score += levelmult / 3;
    }

  return map;
}
