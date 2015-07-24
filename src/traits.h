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

#ifndef TRAITS_H_
#define TRAITS_H_


struct cave;
struct image;
enum GAME_MODE;

#include "gfx_image.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

#define TRAIT_ADVENTURE_MODE (1<< 0)
#define TRAIT_RIBBON         (1<< 1)
#define TRAIT_GREEDY         (1<< 2)
#define TRAIT_TIME_CONTROL   (1<< 3)
#define TRAIT_POWER_PUSH     (1<< 4)
#define TRAIT_DIAMOND_PUSH   (1<< 5)
#define TRAIT_RECYCLER       (1<< 6)
#define TRAIT_STARS1         (1<< 7)
#define TRAIT_STARS2         (1<< 8)
#define TRAIT_STARS3         (1<< 9)
#define TRAIT_CHAOS          (1<<10)
#define TRAIT_DYNAMITE       (1<<11)
#define TRAIT_KEY            (1<<12)
#define TRAIT_IRON_GIRL      (1<<13)
#define TRAIT_PYJAMA_PARTY   (1<<14)
#define TRAIT_QUESTS         (1<<15)
#define TRAIT_EDIT           (1<<16)
#define TRAIT_QUICK_CONTACT  (1<<17)
#define TRAIT_SIZEOF_        18
#define TRAIT_ALL            ((1UL<<TRAIT_SIZEOF_) - 1)

typedef uint32_t trait_t;

struct trait
{
  char *         filename;
  char *         cave_name;
  unsigned int   cave_level;
  uint64_t       cost;
  enum GFX_IMAGE image_id;
  enum GFX_IMAGE disabled_image_id;
};

extern trait_t traits_sorted[TRAIT_SIZEOF_]; /* All traits listed in sorted order for displaying to the player. */

extern void    traits_initialize(void); /* Initialize and load. */
extern void    traits_cleanup(void);    /* Save and cleanup. */
extern void    traits_save(void);       /* Save only. */

extern trait_t traits_get(enum GAME_MODE game_mode); /* Return those traits that are active and valid for the game mode. */
extern trait_t traits_get_active(void);
extern trait_t traits_get_available(void);
extern void    traits_set(trait_t active, trait_t available, uint64_t score);
extern void    traits_activate(trait_t traits);
extern void    traits_deactivate(trait_t traits);
extern void    traits_make_available(trait_t traits);
extern trait_t traits_level_gives(struct cave * cave, unsigned int level, bool also_previous_levels);

extern uint64_t traits_get_score(void);
extern void     traits_add_score(uint64_t score);
extern void     traits_delete_score(uint64_t score);

extern struct trait * trait_get(trait_t trait);
extern char *         trait_get_name(trait_t trait);


#endif
