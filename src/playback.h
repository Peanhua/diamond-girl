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

#ifndef PLAYBACK_H_
#define PLAYBACK_H_

/* Define DEBUG_PLAYBACK to debug the playback by adding some checks.
 * Playback files are not compatible between debug and non-debug versions.
 * #define DEBUG_PLAYBACK
 */
//#define DEBUG_PLAYBACK

#define PLAYBACK_FILE_VERSION "1.1"

#include "diamond_girl.h"
#include "globals.h"
#include "traits.h"
#include <inttypes.h>


struct playback_step
{
#ifdef DEBUG_PLAYBACK
  int                 timestamp;       /* The timestamp of the step, maps to map->game_time. */
  int                 timestamp_timer; /* The micro-timestamp, maps to map->game_timer. */
#endif
  int                 position[2];     /* Player position at the start of this step. */

  /* The controls */
  enum MOVE_DIRECTION direction;       /* The direction player is moving during this step. */
  bool                manipulate;      /* Manipulate only. */
  bool                commit_suicide;  /* Do it! */
};

struct playback
{
  struct playback * next;

  enum GAME_MODE game_mode;
  bool           iron_girl_mode;
  trait_t        traits;

  char * cave;
  int    level;
  bool   not_for_ai;       /* Don't allow AI to use this (for the tutorial playbacks). */
  char * map_hash;

  struct playback_step * steps;
  uint16_t               steps_size;

  uint16_t               current_step;
};


extern struct playback * playback_new(void);
extern struct playback * playback_free(struct playback * playback);

extern bool              playback_save(char const * const filename, struct playback * playback);
extern struct playback * playback_load(char const * const filename);
extern char *            playback_check(struct playback * playback);
extern struct playback * playback_copy(struct playback * playback);

extern struct playback_step * playback_append_step(struct playback * playback);

extern struct playback_step * playback_next_step(struct playback * playback); /* Return current step and advance current_step by one. */

extern void playback_step_initialize(struct playback_step * step);

extern uint32_t playback_get_levels_completed(struct playback * playback);
extern uint32_t playback_get_girls_died(struct playback * playback);

#endif
