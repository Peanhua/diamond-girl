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

#include "globals.h"
#include "quest.h"
#include "random.h"
#include "stack.h"
#include "trader.h"

struct questline * questline_generate(enum QUEST_TYPE type)
{
  struct questline * questline;
  enum RELATION_TYPE reltype, ap_reltype;
  unsigned int typecount;

  typecount = 0;
  for(unsigned int j = 0; j < globals.questlines_size; j++)
    if(globals.questlines[j]->type == type)
      typecount++;
  
  questline = NULL;
  reltype    = RELATION_TYPE_SIZEOF_;
  ap_reltype = RELATION_TYPE_SIZEOF_;
  
  if(type == QUEST_TYPE_RELATIVE)
    {
      struct
      {
        enum RELATION_TYPE type;
        unsigned int       count_max;
        unsigned int       count_current;
        unsigned int       chance_to_use;
      } availtypes[] =
          {
            { RELATION_TYPE_GREAT_GRANDMOTHER, 1, 0, 11 },
            { RELATION_TYPE_GREAT_GRANDFATHER, 1, 0, 11 },
            { RELATION_TYPE_GRANDMOTHER,       1, 0,  5 },
            { RELATION_TYPE_GRANDFATHER,       1, 0,  5 },
            { RELATION_TYPE_MOTHER,            1, 0,  2 },
            { RELATION_TYPE_FATHER,            1, 0,  2 },
            { RELATION_TYPE_SISTER,            1, 0,  1 },
            { RELATION_TYPE_BROTHER,           1, 0,  1 },
            { RELATION_TYPE_AUNT,              2, 0, 11 },
            { RELATION_TYPE_UNCLE,             2, 0, 11 },
            { RELATION_TYPE_COUSIN,            3, 0, 10 },
            { RELATION_TYPE_RELATIVE,          3, 0, 10 },
            { RELATION_TYPE_FEMALE_FRIEND,     5, 0, 10 },
            { RELATION_TYPE_MALE_FRIEND,       5, 0, 10 },
            { RELATION_TYPE_SIZEOF_,           0, 0,  0 }
          };

      unsigned int totalchance;
      unsigned int maxcount;
      
      totalchance = 0;
      maxcount = 0;
      for(unsigned int i = 0; availtypes[i].type != RELATION_TYPE_SIZEOF_; i++)
        {
          int count;
            
          count = 0;
          for(unsigned int j = 0; j < globals.questlines_size; j++)
            if(globals.questlines[j]->type == type)
              if(globals.questlines[j]->first_questgiver.relation_to_player == availtypes[i].type)
                count++;
          availtypes[i].count_current = count;

          totalchance += availtypes[i].chance_to_use;
          maxcount    += availtypes[i].count_max;
        }

      if(typecount < maxcount)
        {
          reltype = RELATION_TYPE_SIZEOF_;
          while(reltype == RELATION_TYPE_SIZEOF_)
            {
              int rtr;
              int rti;
            
              rtr = get_rand(totalchance);
              rti = 0;
              while(rtr > 0 && availtypes[rti].type != RELATION_TYPE_SIZEOF_)
                {
                  rtr -= availtypes[rti].chance_to_use;
                  if(rtr > 0)
                    rti++;
                }

              if(availtypes[rti].count_current < availtypes[rti].count_max)
                reltype = availtypes[rti].type;
            }
        }
    }
  else if(type == QUEST_TYPE_LIBRARY && typecount < 1)
    {
      if(get_rand(100) < 50)
        reltype = RELATION_TYPE_FEMALE_FRIEND;
      else
        reltype = RELATION_TYPE_MALE_FRIEND;        
    }
  else if(type == QUEST_TYPE_ZOMBIES && typecount < 7)
    {
      enum RELATION_TYPE t[5 * 2] =
        {
          RELATION_TYPE_ANCIENT_KING,   RELATION_TYPE_ANCIENT_QUEEN,
          RELATION_TYPE_ANCIENT_PRINCE, RELATION_TYPE_ANCIENT_PRINCESS,
          RELATION_TYPE_ANCIENT_KNIGHT, RELATION_TYPE_ANCIENT_PRINCESS,
          RELATION_TYPE_ANCIENT_KNIGHT, RELATION_TYPE_ANCIENT_QUEEN,
          RELATION_TYPE_ANCIENT_KNIGHT, RELATION_TYPE_ANCIENT_KING
        };

      int i;

      i = get_rand(5) * 2;
      if(get_rand(100) < 60)
        {
          reltype    = t[i];
          ap_reltype = t[i + 1];
        }
      else
        { /* swapped order */
          reltype    = t[i + 1];
          ap_reltype = t[i];
        }
    }


  if(reltype < RELATION_TYPE_SIZEOF_)
    {
      enum RELATION_TYPE ap_reltypes[8] =
        {
          RELATION_TYPE_ANCIENT_QUEEN,
          RELATION_TYPE_ANCIENT_KING,
          RELATION_TYPE_ANCIENT_PRINCESS,
          RELATION_TYPE_ANCIENT_PRINCE,
          RELATION_TYPE_ANCIENT_KNIGHT,
          RELATION_TYPE_ANCIENT_WAR_HERO,
          RELATION_TYPE_ANCIENT_PRIEST,
          RELATION_TYPE_ANCIENT_SHAMAN
        };

      if(ap_reltype == RELATION_TYPE_SIZEOF_)
        ap_reltype = ap_reltypes[get_rand(8)];

      questline = questline_new(type, reltype, ap_reltype);
      if(questline != NULL)
        {
          if(type == QUEST_TYPE_LIBRARY)
            {
              questline->opening_weekday      = 6;
              questline->opening_monthday_max = 7;
            }
          else if(type == QUEST_TYPE_ZOMBIES)
            {
              questline->opening_weekday = typecount;
              questline->opening_time_hour   = 0;
              questline->opening_time_length = 4;
            }
              
          
          /* Calculate reward = number of earlier completed questlines */
          for(unsigned int i = 0; i < globals.questlines_size; i++)
            {
              struct questline * ql;
              struct quest * q;

              ql = globals.questlines[i];
              q = ql->quests->data[ql->current_quest];

              if(q->completed > 0)
                if(ql->current_quest + 1 == ql->quests->size)
                  questline->reward++;
            }
        }
    }


  /* Generate traders if they have not yet been generated. */
  if(globals.traders[0] == NULL)
    {
      for(int i = 0; i < MAX_TRADERS; i++)
        globals.traders[i] = trader_new();
      globals.active_trader = 0;
      globals.active_trader_start = time(NULL);

      unsigned int n;
      struct trader * t;

      /* One trader has good deals. */
      n = get_rand(MAX_TRADERS);
      t = globals.traders[n];
          
      t->hours_to_stay       = t->hours_to_stay - t->hours_to_stay / 2;
      t->daily_visit_arrival = get_rand(20);
      t->daily_visit_length  = 1 + get_rand(5);
      t->cost_modifier       = -50;
          
      /* One trader has bad deals. */
      n++;
      if(n >= MAX_TRADERS)
        n = 0;
      t = globals.traders[n];

      t->daily_visit_arrival = get_rand(4);
      t->daily_visit_length  = 23;
      t->cost_modifier       = 50;

      /* Randomize the rest a bit. */
      for(int i = 0; i < MAX_TRADERS - 2; i++)
        {
          n++;
          if(n >= MAX_TRADERS)
            n = 0;

          t = globals.traders[n];

          if(t->daily_visit_arrival < 22)
            t->daily_visit_arrival += get_rand(4);
          
          t->cost_modifier = (int) get_rand(10) - 5;
        }

      /* Make sure the values are in valid range. */
      for(int i = 0; i < MAX_TRADERS; i++)
        {
          t = globals.traders[n];

          if(t->daily_visit_arrival > 24)
            t->daily_visit_arrival = 24;
          
          if(t->daily_visit_arrival + t->daily_visit_length > 24)
            t->daily_visit_length = 24 - t->daily_visit_arrival;
        }
    }

 return questline;
}
