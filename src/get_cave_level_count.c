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

#include "cave.h"
#include "map.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static bool is_last_level(const char * cave, int level);


static char *       cache_cave  = NULL;
static unsigned int cache_count = 0;


unsigned int get_cave_level_count(const char * cave)
{
  unsigned int count;

  if(cache_cave != NULL && !strcmp(cave, cache_cave))
    { /* Get from cache. */
      count = cache_count;
    }
  else
    {
      bool done;

      done = false;
      count = 0;
      while(done == false)
        {
          if(is_last_level(cave, count) == true)
            done = true;
          else
            count++;
        }

      /* Update cache. */
      free(cache_cave);
      cache_cave = strdup(cave);
      cache_count = count;
    }
  
  return count;
}


static bool is_last_level(const char * cave, int level)
{
  bool last_level;
  struct map * m;

  last_level = false;
  if(cave[0] == '/')
    {
      m = map_get(cave, level + 1);
      if(level > 0 && m != NULL && m->level == 1)
        last_level = true;
    }
  else
    {
      m = map_load(cave, level + 1);
      if(m == NULL)
        last_level = true;
    }
  if(m != NULL)
    m = map_free(m);

  return last_level;
}
