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
#include "title.h"
#include "globals.h"
#include "random.h"
#include "map.h"

#include <assert.h>

void title_map_tick(struct map * map)
{
  assert(map != NULL);

  {
    static int cleartimer = 0;
    
    if(cleartimer == 0)
      {
        int x, y;
        int pos;
        
        x = get_rand(map->width);
        y = map->height - 1;
        pos = x + y * map->width;
        if(map->data[pos] == MAP_DIAMOND)
          map->data[pos] = MAP_ENEMY1;
        else if(map->data[pos] == MAP_ENEMY1)
          map->data[pos] = MAP_SMOKE2;
        
        cleartimer = map->frames_per_second / 10 + get_rand(map->frames_per_second);
        if(cleartimer < 1)
          cleartimer = 1;
      }
    else
      cleartimer--;
  }
  
  map_physics_tick(map, NULL);
}
