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

#include "title.h"
#include "map.h"
#include "random.h"
#include "girl.h"

bool title_map_girl_movement(struct map * map, struct gamedata * gamedata DG_UNUSED)
{
  /* Girl movement */
  int r;
  enum MOVE_DIRECTION newdir;
            
  r = get_rand(10);
  switch(r)
    {
    case 0:  newdir = MOVE_UP; break;
    case 1:  newdir = MOVE_DOWN; break;
    case 2:  newdir = MOVE_LEFT; break;
    case 3:  newdir = MOVE_RIGHT; break;
    default: newdir = MOVE_NONE; break;
    }
            
  map->girl->mob->direction = newdir;
            
  int newx, newy;
            
  newx = map->girl->mob->x;
  newy = map->girl->mob->y;
  switch(map->girl->mob->direction)
    {
    case MOVE_NONE:
      break;
    case MOVE_UP:
      if(newy > 0)
        newy--;
      break;
    case MOVE_DOWN:
      if(newy < map->height - 1)
        newy++;
      break;
    case MOVE_LEFT:
      if(newx > 0)
        newx--;
      break;
    case MOVE_RIGHT:
      if(newx < map->width - 1)
        newx++;
      break;
    }
            
  if(newx != map->girl->mob->x || newy != map->girl->mob->y)
    {
      int pos;
                
      pos = map->girl->mob->x + map->girl->mob->y * map->width;
      map->data[pos] = MAP_EMPTY;
      map->girl->mob->x = newx;
      map->girl->mob->y = newy;
      pos = map->girl->mob->x + map->girl->mob->y * map->width;
      map->data[pos] = MAP_PLAYER;
    }

  return true;
}
