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

#include "ai.h"
#include "map.h"
#include "girl.h"

/**
 * Return the possible moves where the player is currently at.
 *
 * The returned value is a bitmask of directions (4 bits), so test it like:
 * if ( x & (1 << MOVE_UP) ) 
 */

uint8_t ai_get_possible_moves(struct ai * ai)
{
  uint8_t possible_moves;
  struct
  {
    int xdif, ydif;
    enum MOVE_DIRECTION direction;
  } all[4] =
      {
        {  0, -1, MOVE_UP    },
        {  0,  1, MOVE_DOWN  },
        { -1,  0, MOVE_LEFT  },
        {  1,  0, MOVE_RIGHT }
      };

  possible_moves = 0;

  for(int i = 0; i < 4; i++)
    {
      int pos;

      pos = ai->map->girl->mob->x + all[i].xdif + (ai->map->girl->mob->y + all[i].ydif) * ai->map->width;
      if(pos >= 0 && pos < ai->map->width * ai->map->height)
        {
          enum MAP_GLYPH glyph;
          bool pass;

          glyph = ai->map->data[pos];

          pass = false;

          if(glyph == MAP_EMPTY   ||
             glyph == MAP_SAND    ||
             glyph == MAP_DIAMOND ||
             glyph == MAP_SMOKE   ||
             glyph == MAP_SMOKE1  ||
             glyph == MAP_SMOKE2)
            {
              pass = true;
            }
          else if(glyph == MAP_EXIT_LEVEL && ai->map->exit_open == true)
            {
              pass = true;
            }
          else if(glyph == MAP_BOULDER)
            {
              pos = ai->map->girl->mob->x + all[i].xdif * 2 + (ai->map->girl->mob->y + all[i].ydif * 2) * ai->map->width;
              if(pos >= 0 && pos < ai->map->width * ai->map->height)
                {
                  enum MAP_GLYPH glyph_behind;
          
                  glyph_behind = ai->map->data[pos];
                  if(glyph_behind == MAP_EMPTY)
                    pass = true;
                }
            }

          if(pass == true)
            possible_moves |= 1 << all[i].direction;
        }
    }
  
  return possible_moves;
}
