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
#include "gfxbuf.h"
#include "girl.h"

#include <assert.h>
#include <stdlib.h>

struct map * map_free(struct map * map)
{
  if(map != NULL)
    {
      map->player_target = maptarget_free(map->player_target);

      free(map->level_start_anim_map);
      free(map->cave_name);
      free(map->data);
      free(map->processed);
      free(map->move_directions);
      free(map->move_offsets);
      free(map->order);
      free(map->ameba_random_state);
      if(map->girl != NULL)
        map->girl = girl_free(map->girl);

#ifdef WITH_OPENGL
      if(map->drawbuf != NULL)
        map->drawbuf = gfxbuf_free(map->drawbuf);
      if(map->borderbuf != NULL)
        map->borderbuf = gfxbuf_free(map->borderbuf);
#endif

      free(map);
    }

  return NULL;
}
