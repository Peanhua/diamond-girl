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

#ifndef TRADER_H_
#define TRADER_H_

#include "diamond_girl.h"

struct trader
{
  char *       name;
  enum GENDER  gender;
  unsigned int hours_to_stay;       /* How many hours this trader lasts until it's swapped to the next trader. */
  unsigned int daily_visit_arrival; /* The starting hour, 0..23. */
  unsigned int daily_visit_length;  /* How many hours does the trader stay in the cafe. */
  int          cost_modifier;       /* 1 = 1%, -5 = -5% */
  int          item_buy_count;      /* How many items this trader will buy. */
};

extern struct trader * trader_new(void);
extern struct trader * trader_free(struct trader * trader);

extern char *          trader_save(struct trader * trader);
extern struct trader * trader_load(char * savestring);

extern struct trader * trader_current(void);

#endif
