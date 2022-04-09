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

#ifndef TREASUREINFO_H_
#define TREASUREINFO_H_

#include <stdbool.h>

struct treasureinfo
{
  struct treasure * item;
  unsigned int      level;
  unsigned int      x;
  unsigned int      y;
  bool              collected;
};

extern struct treasureinfo * treasureinfo_new(void);
extern struct treasureinfo * treasureinfo_copy(struct treasureinfo const * source);
extern struct treasureinfo * treasureinfo_free(struct treasureinfo * treasureinfo);
extern char *                treasureinfo_save(struct treasureinfo * treasureinfo); /* Return a string describing the treasure. */
extern struct treasureinfo * treasureinfo_load(char const * data);                  /* Generate treasureinfo from data generated with treasureinfo_save(). */

#endif
