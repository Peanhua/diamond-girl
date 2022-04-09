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
#include <errno.h>
#include <string.h>


static struct mappath ** list  = NULL;
static int list_size           = 0;
static int list_allocated_size = 0;

static bool (*is_passable_callback)(struct map * map, int x, int y, int xmod, int ymod);

static void path_add(struct map * map, struct mappath * parent, int xmod, int ymod, int tox, int toy);
static bool is_passable(struct map * map, int x, int y, int xmod, int ymod);

struct mappath * map_find_path(struct map * map, int fromx, int fromy, int tox, int toy,
                               bool (*is_passable_cb)(struct map * map, int x, int y, int xmod, int ymod))
{
  struct mappath * rv;
  const int max_list_size = 512;

  rv = NULL;
  if(fromx >= 0 && fromx < map->width && fromy >= 0 && fromy < map->height &&
     tox   >= 0 && tox   < map->width && toy   >= 0 && toy   < map->height)
    {
      struct mappath * current;

      list_size = map->width * map->height;
      if(list_size > max_list_size)
        list_size = max_list_size;
      if(list == NULL || list_size > list_allocated_size)
        {
          struct mappath ** tmp;
          
          tmp = realloc(list, sizeof(struct mappath *) * list_size);
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              list = tmp;
              list_allocated_size = list_size;
            }
        }
      assert(list != NULL);
      if(list != NULL)
        {
          is_passable_callback = is_passable_cb;

          for(int i = 0; i < list_size; i++)
            list[i] = NULL;

          current = malloc(sizeof(struct mappath));
          assert(current != NULL);
          current->parent = NULL;
          current->x = fromx;
          current->y = fromy;
          current->estimate = abs(fromx - tox) + abs(fromy - toy);
          current->cost = 0;
          current->open = true;
          list[0] = current;

          while(current != NULL)
            {
              struct mappath * next;

              //path_add(map, current, -1, -1, tox, toy);
              path_add(map, current,  0, -1, tox, toy);
              //path_add(map, current, +1, -1, tox, toy);
              path_add(map, current, -1,  0, tox, toy);
              path_add(map, current, +1,  0, tox, toy);
              //path_add(map, current, -1, +1, tox, toy);
              path_add(map, current,  0, +1, tox, toy);
              //path_add(map, current, +1, +1, tox, toy);

              /* close current as it has been done */
              current->open = false;

              { /* search next from the list */
                int mincost, i;

                next = NULL;
                mincost = 0;
                for(i = 0; i < list_size; i++)
                  if(list[i] != NULL)
                    if(list[i]->open == true)
                      if(list[i]->cost + list[i]->estimate < mincost || mincost == 0)
                        {
                          mincost = list[i]->cost + list[i]->estimate;
                          next = list[i];
                        }
              }

              current = next;
              if(current == NULL)
                break;
              if(current->open == false)
                break;
              if(current->x == tox && current->y == toy)
                { /* Path found. */
                  rv = current;
                  break;
                }
            }
  
          if(rv != NULL)
            { /* Cleanup the list from orphaned nodes. */
              for(int i = 0; i < list_size; i++)
                if(list[i] != NULL)
                  {
                    struct mappath * tmp;
                    bool found;

                    tmp = rv;
                    found = false;
                    while(found == false && tmp != NULL)
                      {
                        if(tmp == list[i])
                          found = true;
                        tmp = tmp->parent;
                      }
                    if(found == false)
                      free(list[i]);
                  }
            }
          else
            {
              for(int i = 0; i < list_size; i++)
                if(list[i] != NULL)
                  free(list[i]);
            }
        }
      else
        fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
    }

  return rv;
}

static void path_add(struct map * map, struct mappath * parent, int xmod, int ymod, int tox, int toy)
{
  int x, y;

  x = parent->x;
  y = parent->y;

  if(is_passable(map, x, y, xmod, ymod))
    { /* Not blocked, can proceed. */
      x += xmod;
      y += ymod;
      /* search if the list already contains this */
      struct mappath * current;
      bool found;

      current = NULL;
      found = false;
      for(int i = 0; i < list_size && found == false; i++)
        if(list[i] != NULL)
          if(list[i]->x == x && list[i]->y == y)
            {
              found = true;
              current = list[i];
            }

      if(found == true)
        { /* found */
          if(current->cost <= parent->cost + 1)
            { /* we have been there with fewer costs than it would take to come from here, discard this path */
              ;
            }
          else
            { /* we have been there with bigger costs, overwrite it: we'll come this way instead */
              current->parent = parent;
            }
        }
      else if(found == false)
        { /* add new */
          current = malloc(sizeof(struct mappath));
          assert(current != NULL);
          current->open = true;
          current->x = x;
          current->y = y;
          current->estimate = abs(current->x - tox) + abs(current->y - toy);
          current->cost = parent->cost + 1; /* it costs "one" to move from parent to here */
          current->parent = parent; /* we came from here */
          for(int i = 0; i < list_size && current != NULL; i++)
            if(list[i] == NULL)
              {
                list[i] = current;
                current = NULL;
              }

          if(current != NULL)
            { /* No space in list! */
#ifndef NDEBUG
              //printf("%s:%s() no space in list\n", __FILE__, __FUNCTION__);
#endif
              free(current);
              current = NULL;
            }
        }
    }
}


static bool is_passable(struct map * map, int x, int y, int xmod, int ymod)
{
  bool rv;

  if(x + xmod >= 0 && x + xmod < map->width && y + ymod >= 0 && y + ymod < map->height)
    {
      if(is_passable_callback != NULL)
        {
          rv = is_passable_callback(map, x, y, xmod, ymod);
        }
      else
        {
          enum MAP_GLYPH glyph;
          
          x += xmod;
          y += ymod;

          glyph = map->data[x + y * map->width];
          
          if(glyph == MAP_EMPTY   ||
             glyph == MAP_SAND    ||
             glyph == MAP_DIAMOND ||
             glyph == MAP_SMOKE   ||
             glyph == MAP_SMOKE1  ||
             glyph == MAP_SMOKE2  ||
             ( glyph == MAP_EXIT_LEVEL && map->exit_open == true ) )
            rv = true;
          else
            rv = false;
        }
    }
  else
    rv = false;

  return rv;
}

