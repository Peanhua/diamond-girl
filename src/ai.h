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

#ifndef AI_H_
#define AI_H_

#include "diamond_girl.h"
#include "map.h"
#include "game.h"

struct map;
struct playback;
struct playback_step;


struct ai
{
  bool   quit;           /* AI wants to quit. */
  bool   use_planning;   /* Run the planner at the end of map initialization. */
  char * engine;         /* The name of the engine currently in use. */
  char * playback_title; /* The title of the playback, or NULL. */

  struct map *      map;
  struct playback * playback;

  struct playback_step * (*get_next_move)(struct ai * ai, bool game_active); /* Called once per game tick to obtain the girls next move. */
  void                   (*diamond_collected)(struct ai * ai);               /* Called whenever the girl collects a diamond. */
  void                   (*death)(struct ai * ai);                           /* Called whenever the girl dies. */
  void                   (*victory)(struct ai * ai);                         /* Called whenever the girl completes a level. */
  void                   (*cave_restart)(struct ai * ai);                    /* Called whenever the girl completes the final level of a cave,
                                                                              * call to cave_restart() is executed after calling victory(). */
};

struct ai_knowledge
{
  char * address;
  float  value;
  float  experience;
};


struct diamond
{
  int dist;
  int x;
  int y;
};

extern struct ai * ai_new(void);
extern struct ai * ai_free(struct ai * ai);

/* Register new knowledge at 'address', strength is between -1 .. 1.0. */
extern void  ai_brains_load(const char * name);
extern void  ai_brains_save(const char * name);
extern void  ai_brains_cleanup(void);
extern float ai_brains_add(const char * address, float strength);
extern float ai_brains_get(const char * address);

extern void  ai_brains_dump(void);
extern void  ai_brains_fart(void);

/* Utility functions. */
extern void    ai_set_engine_name(struct ai * ai, char const * const name);
extern uint8_t ai_get_possible_moves(struct ai * ai);
extern struct maptarget * ai_get_next_target(struct map * map, enum MAP_GLYPH seek_item, struct maptarget * blacklisted_target, int favor_updown, int favor_leftright);

/* Utility functions to check if a movement is possible using certain rules. 
 * Used by ai_diamond_hunter.
 */
extern bool ai_is_passable(struct map * map, int x, int y, int xmod, int ymod, bool cautious_mode, bool panic_mode, bool avoid_dead_ends);
extern bool ai_is_passable_direction(struct map * map, enum MOVE_DIRECTION direction, bool cautious_mode, bool panic_mode);
extern bool ai_is_passable_mfp_cb(struct map * map, int x, int y, int xmod, int ymod);
extern bool ai_is_passable_mfp_cb_cautiousoff(struct map * map, int x, int y, int xmod, int ymod);

/* Plan what to do in the current level, trigger one of the stuff below. */
extern void ai_plan_level(struct ai * ai, enum GAME_MODE game_mode);

/* Set AI to playback the given playback. */
extern bool ai_playback(struct ai * ai, enum GAME_MODE game_mode, struct playback * playback, char * playback_title);

/* Set AI to hunt diamonds. */
extern void ai_diamond_hunter(struct ai * ai);

/* Set AI to be a girl of the pyjama party. */
extern void ai_pyjama_party(struct ai * ai);


#endif
