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

#ifndef RANDOM_H_
#define RANDOM_H_

#include <inttypes.h>

extern unsigned int get_rand(unsigned int range);
extern unsigned int get_rand_state(unsigned int range, void * state);

extern void         seed_rand(uint32_t seed);
extern void *       seed_rand_state(uint32_t seed);

extern void *       rand_state_duplicate(void * state);

extern const char * get_random_name(void);

#endif
