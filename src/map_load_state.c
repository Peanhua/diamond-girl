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
#include <assert.h>
#include <stdio.h>

bool map_load_state(struct map * map, const char * filename)
{
  bool rv;
  char * buffer;
  int    buffer_size;

  rv = read_file(filename, &buffer, &buffer_size);
  if(rv == true)
    {
      if(!memcmp(buffer, DIAMOND_GIRL_MAP_STATE_V1, strlen(DIAMOND_GIRL_MAP_STATE_V1)))
        {
          char * pos;
#define copy(dst, len) do { \
            if(pos - buffer + (unsigned int) len <= (unsigned int) buffer_size) \
              { \
                memcpy(dst, pos, len); \
                pos += len; \
              } \
            else \
              { \
                rv = false; \
                fprintf(stderr, "%s: Failed to read from '%s': out of data\n", __FUNCTION__, get_save_filename(filename)); \
              } \
          } while(0)
          
          pos = buffer;
          pos += strlen(DIAMOND_GIRL_MAP_STATE_V1);

          copy(map->data,                 map->width * map->height);
          copy(&map->ameba_blocked_timer, sizeof map->ameba_blocked_timer);
          copy(&map->diamonds,            sizeof map->diamonds);
          copy(&map->exit_open,           sizeof map->exit_open);
          copy(&map->morpher_is_on,       sizeof map->morpher_is_on);
          copy(&map->morpher_end_time,    sizeof map->morpher_end_time);
          copy(map->move_directions,      map->width * map->height * sizeof(enum MOVE_DIRECTION));
          copy(map->order,                2 * map->width * map->height * sizeof(unsigned int));
          copy(&map->background_type,     sizeof map->background_type);

          assert(pos - buffer == buffer_size);
        }
      else
        {
          rv = false;
          fprintf(stderr, "%s: Failed to read from '%s': header mismatch error\n", __FUNCTION__, get_save_filename(filename));
        }

      free(buffer);
    }

  return rv;
}
