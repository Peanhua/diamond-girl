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

#ifndef GIRL_H_
#define GIRL_H_

#include "traits.h"
#include <stdbool.h>
#include <time.h>

#define GIRLFILE_HEADER_V1 "dgr1"

struct map_object;

struct girl
{
  char                name[32];
  time_t              birth_time;
  trait_t             traits;            /* Current traits. */
  trait_t             possible_traits;   /* Traits possible to obtain. */
  uint64_t            diamonds;
  uint64_t            diamonds_to_trait; /* The number of diamonds needed for the next trait. */
  struct map_object * mob;
};

extern struct girl * girl_new(void);
extern struct girl * girl_free(struct girl * girl);

extern struct girl * girl_load(const char * filename);
extern bool          girl_save(struct girl * girl);

extern void          girl_chat(struct girl * girl, char * message);

#endif
