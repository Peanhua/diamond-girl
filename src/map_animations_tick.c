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
#include "gfx_glyph.h"
#include "random.h"
#include "girl.h"
#include "globals.h"

void map_animations_tick(struct map * map)
{
  if(map->game_paused == false)
    {
      map->anim_timer++;
      if(map->anim_timer >= map->frames_per_second / 10)
        {
          map->anim_timer = 0;

          gfx_next_frame();

          if(map->girl->mob->chat_bubble_timer > 0)
            map->girl->mob->chat_bubble_timer--;

          if(map->is_intermission == false)
            {
              if(map->girl->mob->direction == MOVE_NONE)
                {
                  if(map->girl->mob->animation > 0)
                    { /* Stop animation when it ends. */
                      if(gfx_get_frame(MAP_PLAYER_ARMOUR2) == 0)
                        {
                          map->girl->mob->animation_repeats--;
                          if(map->girl->mob->animation_repeats <= 0)
                            map->girl->mob->animation = 0;
                        }
                    }
                  else
                    { /* Start animation randomly. */
                      int r;
              
                      r = get_rand(1000);
                      if(r < 30)
                        { /* eye flashing */
                          if(globals.gfx_girl_animation)
                            {
                              map->girl->mob->animation = 1;
                              map->girl->mob->animation_repeats = 1 + get_rand(2);
                            }
                        }
                      else if(r < 33)
                        { /* "run down" */
                          map->girl->mob->animation = 2;
                          map->girl->mob->animation_repeats = 3 + get_rand(3);
                        }
                    }
                }
              else
                map->girl->mob->animation = 0; /* Player moved, don't do (and stop currently running) animations now. */
            }

          if(gfx_get_frame(MAP_DIAMOND) == 0)
            if(globals.gfx_diamond_animation)
              {
                int count;

                /* A level based twinkling effect. */
                count = 3 + ((map->width + map->height) & 0xf);
                for(int i = 0; i < count; i++)
                  {
                    enum MAP_GLYPH g;
                    int pos;
                
                    pos = get_rand(map->width) + get_rand(map->height) * map->width;
                    g = map->data[pos];
                    if(g == MAP_DIAMOND)
                      if(map->move_directions[pos] == MOVE_NONE)
                        map->move_directions[pos] = get_rand(2);
                  }
              }
        }

      if(globals.map_tilting == true)
        if(map->tilt > 0)
          map->tilt--;
    }
}
