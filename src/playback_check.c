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

#include "playback.h"
#include "map.h"
#include <assert.h>
#include <libintl.h>

/* Check playback for validity.
 * Return NULL or error message.
 */
char * playback_check(struct playback * playback)
{
  char * rv;
  char buf[1024];
  
  rv = NULL;

  while(rv == NULL && playback != NULL)
    {
      if(playback->map_hash != NULL)
        {
          struct map * map;
          char * hash;

          map = map_get(playback->cave, playback->level);
          if(map != NULL)
            {
              hash = map_calculate_hash(map);
              if(hash != NULL)
                {
                  if(strcmp(hash, playback->map_hash))
                    {
                      snprintf(buf, sizeof buf, gettext("Checksum error on map %s:%d."), playback->cave, playback->level);
                      rv = strdup(buf);
                    }
                }
              map = map_free(map);
            }
        }

      playback = playback->next;
    }

  return rv;
}
