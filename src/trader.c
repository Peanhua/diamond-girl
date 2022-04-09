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

#include "globals.h"
#include "names.h"
#include "random.h"
#include "trader.h"
#include <assert.h>

struct trader * trader_new(void)
{
  struct trader * trader;

  trader = malloc(sizeof *trader);
  assert(trader != NULL);
  if(trader != NULL)
    {
      char buf[128];
      
      trader->hours_to_stay = 24 + get_rand(24 * 6);

      if(get_rand(2) == 0)
        trader->gender = GENDER_FEMALE;
      else
        trader->gender = GENDER_MALE;

      /* Set some default visiting times, arrives 06:00 and leaves 22:00. */
      trader->daily_visit_arrival = 6;
      trader->daily_visit_length  = 22 - trader->daily_visit_arrival;

      trader->cost_modifier       = 0;
      trader->item_buy_count      = 1;
      
      snprintf(buf, sizeof buf, "%s", names_get(trader->gender, 3, 7));
      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), " %s", names_get(GENDER_FAMILY, 3, 14));
      trader->name = strdup(buf);
      if(trader->name == NULL)
        {
          free(trader);
          trader = NULL;
        }
    }
  
  return trader;
}

struct trader * trader_free(struct trader * trader)
{
  assert(trader != NULL);

  free(trader->name);
  free(trader);

  return NULL;
}


char * trader_save(struct trader * trader)
{
  char buf[1024];

  snprintf(buf, sizeof buf, "%u %u %u %u %d %d %s",
           (unsigned int) trader->gender,
           (unsigned int) trader->hours_to_stay,
           (unsigned int) trader->daily_visit_arrival,
           (unsigned int) trader->daily_visit_length,
           (int)          trader->cost_modifier,
           (int)          trader->item_buy_count,
           trader->name);
  return strdup(buf);
}


struct trader * trader_load(char * savestring)
{
  struct trader * trader;

  assert(savestring != NULL);
  if(savestring != NULL)
    {
      trader = malloc(sizeof *trader);
      assert(trader != NULL);
      if(trader != NULL)
        {
          char * p;

          trader->gender = strtoul(savestring, &p, 0);
          assert(p != NULL);

          if(p != NULL)
            trader->hours_to_stay = strtoul(p, &p, 0);
          assert(p != NULL);

          if(p != NULL)
            trader->daily_visit_arrival = strtoul(p, &p, 0);
          assert(p != NULL);

          if(p != NULL)
            trader->daily_visit_length = strtoul(p, &p, 0);
          assert(p != NULL);

          if(p != NULL)
            trader->cost_modifier = strtol(p, &p, 0);
          assert(p != NULL);

          if(p != NULL)
            trader->item_buy_count = strtol(p, &p, 0);
          assert(p != NULL);
          
          if(p != NULL)
            {
              while(*p == ' ')
                p++;
              trader->name = strdup(p);
              assert(trader->name != NULL);
              if(trader->name == NULL)
                {
                  free(trader);
                  trader = NULL;
                }
            }
          else
            {
              free(trader);
              trader = NULL;
            }
        }
    }
  else
    trader = NULL;

  assert(trader != NULL);

  return trader;
}



struct trader * trader_current(void)
{
  struct trader * current;

  current = globals.traders[globals.active_trader];
  while(current != NULL && (unsigned long) globals.active_trader_start + current->hours_to_stay * 60 * 60 < (unsigned long) time(NULL))
    {
      globals.active_trader_start += current->hours_to_stay * 60 * 60;

      while(current == globals.traders[globals.active_trader])
        globals.active_trader = get_rand(MAX_TRADERS);

      current = globals.traders[globals.active_trader];
      assert(current != NULL);
    }

  return current;
}
