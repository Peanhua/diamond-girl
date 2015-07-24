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

#include "traits.h"
#include "cave.h"
#include <string.h>
#include <assert.h>

trait_t traits_level_gives(struct cave * cave, unsigned int level, bool also_previous_levels)
{
  assert(cave != NULL);

  if(level == (unsigned int) cave->level_count)
    level = 0; // level 0 equals to "last level"

  
  trait_t rv;

  rv = 0;
  for(unsigned int i = 0; i < TRAIT_SIZEOF_; i++)
    {
      struct trait * traitdata;
      trait_t trait;

      trait = 1 << i;
      traitdata = trait_get(trait);
      if(traitdata != NULL)
        if(!strcmp(traitdata->cave_name, cave->name))
          {
            if(traitdata->cave_level == level)
              rv |= trait;
            
            if(also_previous_levels)
              if(level == 0 || (traitdata->cave_level > 0 && traitdata->cave_level < level))
                rv |= trait;
          }
    }

  return rv;
}
