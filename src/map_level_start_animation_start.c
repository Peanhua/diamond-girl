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

#include <assert.h>

void map_level_start_animation_start(struct map * map)
{
  assert(map != NULL);

  map->level_start_anim_map = map->data;
  map->data = malloc(map->width * map->height);
  if(map->data != NULL)
    {
      map->level_start_anim_on = map->frames_per_second * 5;
      map->level_start_anim_player = 0;
      for(int i = 0; i < map->width * map->height; i++)
        map->data[i] = MAP_BRICK;
      map->data[map->start_x + map->start_y * map->width] = MAP_EMPTY;
      map->glyph_set = 1;
    }
  else
    {
      fprintf(stderr, "Warning, failed to allocate memory for level start map.\n");
      map->data                 = map->level_start_anim_map;
      map->level_start_anim_map = NULL;
    }
  
}
