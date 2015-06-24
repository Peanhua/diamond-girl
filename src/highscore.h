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

#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include "traits.h"
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>

struct playback;
struct gamedata;
enum GAME_MODE;

struct highscore_entry
{
  time_t            timestamp;
  int               score;
  int               diamond_score;
  uint32_t          diamonds_collected;
  char *            cave;
  int               starting_girls;
  trait_t           traits;
  int               level;
  char              notes[128];
  bool              playback_dirty;
  int               playback_id; /* unique id within one cave */
  struct playback * playback;
};

struct highscorelist
{
  char *                    filename;
  struct highscore_entry ** highscores;
  size_t                    highscores_size;
  int                       highscores_dirty;

  struct
  {
    uint64_t score;
    uint64_t caves_entered;
    uint64_t levels_completed;
    uint64_t diamonds_collected;
    uint64_t girls_died;
  } total;
};


extern struct highscorelist *    highscores_new(void);
extern struct highscorelist *    highscores_free(struct highscorelist * list);
extern void                      highscores_load(struct highscorelist * list, enum GAME_MODE game_mode, const char * cave);
extern void                      highscores_save(struct highscorelist * list, enum GAME_MODE game_mode, const char * cave);
extern void                      highscores_delete(struct highscorelist * list);
extern struct highscore_entry ** highscores_get(struct highscorelist * list, size_t * size_ptr);

extern int                       highscore_new(struct highscorelist * list, struct gamedata * gamedata, char * notes);
extern struct playback *         highscore_playback(struct highscore_entry * entry, enum GAME_MODE game_mode);

extern char * highscore_filename(enum GAME_MODE gamemode, const char * cave);

#endif
