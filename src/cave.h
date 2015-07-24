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

#ifndef CAVE_H_
#define CAVE_H_

#include "game.h"
#include <stdbool.h>

struct highscorelist;
struct playback;
struct treasureinfo;

#define DIAMOND_GIRL_CAVEFILE_V2 "dgc1" /* Oops, "wrong" magic number there, too late to change because it got into a released version. */
#define DIAMOND_GIRL_CAVEFILE_V3 "dgc3"
#define DIAMOND_GIRL_CAVEFILE_V4 "dgc4"

struct cave
{
  enum GAME_MODE game_mode;
  char *         name;
  int            level_count;
  int            max_starting_level;
  bool           editable;

  struct
  {
    bool                  exists; /* If true, then a savegame exists == the data here is usable. */
    int                   game_level;
    int                   game_speed_modifier;
    int                   starting_girls;
    int                   girls;
    uint64_t              score;
    int                   diamond_score;
    uint64_t              diamonds;
    struct playback *     playback;
    bool *                pyjama_party_girls; /* The alive statuses of the girls. */
    int                   pyjama_party_girls_passed;
    struct map *          map;
    struct treasureinfo * treasure;
  }              savegame;
  
  struct highscorelist * highscores;
};

extern struct cave * cave_get(enum GAME_MODE game_mode, char * cave_name);
extern void          cave_cleanup(void);
extern struct cave * cave_load(enum GAME_MODE game_mode, const char * cave_name);
extern bool          cave_save(struct cave * cave);

extern char **      get_cave_names(bool sorted);
extern char **      free_cave_names(char ** cave_names);

extern char *       cave_filename(const char * cave);
extern char *       cave_displayname(char * cave_name);

extern unsigned int get_cave_level_count(const char * cave);


#endif
