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

#include "ai.h"
#include "stack.h"
#include "map.h"
#include "girl.h"
#include <assert.h>

static int diamonds_stack_compare_function(const void * a, const void * b);


struct maptarget * ai_get_next_target(struct map * map, enum MAP_GLYPH seek_item, struct maptarget * blacklisted_target, int favor_updown, int favor_leftright)
{
  struct stack * diamonds;
  struct maptarget * target;

  target = NULL;

  diamonds = stack_new();
  assert(diamonds != NULL);
  if(diamonds != NULL)
    {
      stack_set_compare_function(diamonds, diamonds_stack_compare_function);

      for(int y = 0; y < map->height; y++)
        for(int x = 0; x < map->width; x++)
          if(blacklisted_target == NULL || blacklisted_target->x != x || blacklisted_target->y != y)
            {
              enum MAP_GLYPH glyph;

              glyph = map->data[x + y * map->width];

              if(glyph == seek_item)
                {
                  int dist;
                  struct diamond * d;
                
                  dist = abs(map->girl->mob->x - x) + abs(map->girl->mob->y - y);

                  if(favor_leftright == -1)
                    dist += x;
                  else if(favor_leftright == 1)
                    dist += map->width - x;
                
                  if(favor_updown == -1)
                    dist += y;
                  else if(favor_updown == 1)
                    dist += map->height - y;
                
                  d = malloc(sizeof(struct diamond));
                  assert(d != NULL);
                  if(d != NULL)
                    {
                      d->dist = dist;
                      d->x = x;
                      d->y = y;
                      stack_push(diamonds, d);
                    }
                }
            }


      struct mappath * path;

      path = NULL;
      for(int i = diamonds->size - 1; path == NULL && i >= 0; i--)
        if(diamonds->data[i] != NULL)
          {
            struct diamond * diamond;

            diamond = diamonds->data[i];

            if(path == NULL)
              {
                path = map_find_path(map,
                                     map->girl->mob->x, map->girl->mob->y,
                                     diamond->x,    diamond->y,
                                     ai_is_passable_mfp_cb);
                if(path != NULL)
                  {
                    target = malloc(sizeof(struct maptarget));
                    assert(target != NULL);
                    if(target != NULL)
                      {
                        struct mappath * p;
                        int pathlen;

                        pathlen = 0;
                        p = path;
                        while(p->parent != NULL && p->parent->parent != NULL)
                          {
                            p = p->parent;
                            pathlen++;
                          }
                        target->x = diamond->x;
                        target->y = diamond->y;
                        target->original_distance = pathlen;
                        target->age = 0;
                        target->path = path;
                      }
                  }
              }
            free(diamond);
          }

      diamonds = stack_free(diamonds);
    }

  return target;
}


static int diamonds_stack_compare_function(const void * a, const void * b)
{
  const struct diamond * da, * db;
  int rv;

  da = * (struct diamond * const *) a;
  db = * (struct diamond * const *) b;

  if(da->dist > db->dist)
    rv = -1;
  else if(da->dist < db->dist)
    rv = 1;
  else
    rv = 0;

  return rv;
}
