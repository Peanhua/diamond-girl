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
#include <errno.h>
#include <stdio.h>

bool map_save_state(struct map * map, const char * filename)
{
  bool rv;
  char * buffer;
  size_t buffer_size;

  rv = false;

  buffer_size = strlen(DIAMOND_GIRL_MAP_STATE_V1);
  buffer_size += map->width * map->height; /* map->data */
  buffer_size += sizeof map->ameba_blocked_timer;
  buffer_size += sizeof map->diamonds;
  buffer_size += sizeof map->exit_open;
  buffer_size += sizeof map->morpher_is_on;
  buffer_size += sizeof map->morpher_end_time;
  buffer_size += map->width * map->height * sizeof(enum MOVE_DIRECTION);
  buffer_size += 2 * map->width * map->height * sizeof(unsigned int); /* map->order */
  buffer_size += sizeof map->background_type;

  buffer = malloc(buffer_size);
  assert(buffer != NULL);
  if(buffer != NULL)
    {
      char * pos;
#define add(src, len) do { memcpy(pos, src, len); pos += len; } while(0)
      
      pos = buffer;

      add(DIAMOND_GIRL_MAP_STATE_V1, strlen(DIAMOND_GIRL_MAP_STATE_V1));
      add(map->data,                 map->width * map->height);
      add(&map->ameba_blocked_timer, sizeof map->ameba_blocked_timer);
      add(&map->diamonds,            sizeof map->diamonds);
      add(&map->exit_open,           sizeof map->exit_open);
      add(&map->morpher_is_on,       sizeof map->morpher_is_on);
      add(&map->morpher_end_time,    sizeof map->morpher_end_time);
      add(map->move_directions,      map->width * map->height * sizeof(enum MOVE_DIRECTION));
      add(map->order,                2 * map->width * map->height * sizeof(unsigned int));
      add(&map->background_type,     sizeof map->background_type);

      assert(pos - buffer == (int) buffer_size);

      rv = write_file(filename, buffer, buffer_size);

      free(buffer);
    }
  else
    fprintf(stderr, "%s: Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));

  return rv;
}
