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
#include "girl.h"
#include "random.h"
#include "sfx.h"

void map_level_start_animation_tick(struct map * map)
{
  if(map->game_paused == false)
    {
      if(map->level_start_anim_on)
        map->level_start_anim_on--;

      if(map->level_start_anim_on)
        {
          if(map->level_start_anim_on)
            {
              for(int i = 0; i < map->width * map->height / 100; i++)
                {
                  int x, y;
              
                  x = get_rand(map->width);
                  y = get_rand(map->height);
                  map->data[x + y * map->width] = map->level_start_anim_map[x + y * map->width];
                }
	      
              map->level_start_anim_player++;
              if(map->level_start_anim_player > map->frames_per_second / 5)
                {
                  map->level_start_anim_player = 0;
                  if(map->data[map->start_x + map->start_y * map->width] == MAP_EMPTY)
                    map->data[map->start_x + map->start_y * map->width] = MAP_PLAYER;
                  else
                    map->data[map->start_x + map->start_y * map->width] = MAP_EMPTY;
                }
            }
        }
      else
        {
          map_level_start_animation_stop(map);
          sfx_emit(SFX_SMALL_EXPLOSION, map->girl->mob->x, map->girl->mob->y);
          map->flash_screen = true;
        }
    }
}
