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

#include "treasure.h"
#include "treasureinfo.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct treasureinfo * treasureinfo_new(void)
{
  struct treasureinfo * ti;

  ti = malloc(sizeof *ti);
  assert(ti != NULL);
  if(ti != NULL)
    {
      ti->item = malloc(sizeof *ti->item);
      assert(ti->item != NULL);
      if(ti->item == NULL)
        {
          free(ti);
          ti = NULL;
        }
    }

  return ti;
}

struct treasureinfo * treasureinfo_copy(struct treasureinfo const * source)
{
  struct treasureinfo * ti;

  if(source != NULL)
    {
      ti = treasureinfo_new();
      if(ti != NULL)
        {
          ti->level     = source->level;
          ti->x         = source->x;
          ti->y         = source->y;
          ti->collected = source->collected;
      
          ti->item->type      = source->item->type;
          ti->item->material  = source->item->material;
          ti->item->gemstones = source->item->gemstones;
          ti->item->name      = strdup(source->item->name);
        }
    }
  else
    ti = NULL;

  return ti;
}

struct treasureinfo * treasureinfo_free(struct treasureinfo * treasureinfo)
{
  if(treasureinfo != NULL)
    {
      free(treasureinfo->item->name);
      free(treasureinfo->item);
      free(treasureinfo);
    }
  
  return NULL;
}


char * treasureinfo_save(struct treasureinfo * treasureinfo)
{
  char buf[256];
  
  snprintf(buf, sizeof buf, "%u %u %u %u %u %u %u %s",
           (unsigned int) treasureinfo->level,
           (unsigned int) treasureinfo->x,
           (unsigned int) treasureinfo->y,
           (unsigned int) treasureinfo->collected,
           (unsigned int) treasureinfo->item->type,
           (unsigned int) treasureinfo->item->material,
           (unsigned int) treasureinfo->item->gemstones,
           treasureinfo->item->name);

  return strdup(buf);
}


struct treasureinfo * treasureinfo_load(char const * data)
{
  struct treasureinfo * ti;

  ti = treasureinfo_new();
  if(ti != NULL)
    {
      char * p;
  
      ti->level           = strtoul(data, &p, 0); data = p;
      ti->x               = strtoul(data, &p, 0); data = p;
      ti->y               = strtoul(data, &p, 0); data = p;
      ti->collected       = strtoul(data, &p, 0); data = p;
      ti->item->type      = strtoul(data, &p, 0); data = p;
      ti->item->material  = strtoul(data, &p, 0); data = p;
      ti->item->gemstones = strtoul(data, &p, 0); data = p;
      ti->item->name      = strdup(data);
    }

  return ti;
}

