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
#include "gfx.h"
#include "girl.h"

/**
 * Scroll the map slowly, stop when the girl is about in the middle.
 */

void map_scroll_to_girl(struct map * map, int finespeed)
{
  int mmx, mmy;
  SDL_Surface * screen;

  screen = gfx_game_screen();

  mmx = map->girl->mob->x - (screen->w / 24 - 2) / 2;
  mmy = map->girl->mob->y - ((screen->h - 50) / 24 - 2) / 2;
  if(map->map_x < mmx)
    map->map_fine_x -= finespeed;
  else if(map->map_x > mmx)
    map->map_fine_x += finespeed;
  if(map->map_y < mmy)
    map->map_fine_y -= finespeed;
  else if(map->map_y > mmy)
    map->map_fine_y += finespeed;

  if(map->map_fine_x < 0)
    {
      map->map_x++;
      map->map_fine_x = 23;
    }
  else if(map->map_fine_x >= 24)
    {
      map->map_x--;
      map->map_fine_x = 0;
    }
  if(map->map_fine_y < 0)
    {
      map->map_y++;
      map->map_fine_y = 23;
    }
  else if(map->map_fine_y >= 24)
    {
      map->map_y--;
      map->map_fine_y = 0;
    }
}
