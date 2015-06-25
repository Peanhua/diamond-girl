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
#include "game.h"
#include "cave.h"
#include "playback.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

struct gamedata * title_load_playback(void)
{
#ifdef PROFILING

  return NULL;

#else

  struct gamedata * rv;
  char fn[128];

  rv = NULL;

  snprintf(fn, sizeof fn, "last-%d.dgp", (int) globals.title_game_mode);
  if(access(get_save_filename(fn), R_OK) == 0)
    {
      struct playback * pb;

      pb = playback_load(get_save_filename(fn));
      if(pb != NULL)
        {
          struct cave * cave;

          unlink(get_save_filename(fn));

          cave = cave_get(pb->game_mode, pb->cave);
          rv = gamedata_new(cave, pb->iron_girl_mode, pb->traits);
          assert(rv != NULL);
          if(rv != NULL)
            rv->playback = pb;
          else
            fprintf(stderr, "%s:%s(): Failed to allocate memory: %s\n", __FILE__, __FUNCTION__, strerror(errno));
        }
    }

  return rv;
#endif
}
