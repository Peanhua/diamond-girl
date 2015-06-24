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

#ifndef GAME_H_
#define GAME_H_

#include "traits.h"

#include <stdbool.h>
#include <inttypes.h>
#include <SDL/SDL_framerate.h>

struct ai;
struct playback;
struct cave;
struct map;

#define MAX_PYJAMA_PARTY_SIZE 9

enum GAME_MODE
  {
    GAME_MODE_CLASSIC,
    GAME_MODE_ADVENTURE,
    GAME_MODE_PYJAMA_PARTY
  };

enum PARTYCONTROLLER
  {
    PARTYCONTROLLER_PLAYER,
    PARTYCONTROLLER_GIRL,
    PARTYCONTROLLER_PARTY
  };

struct gamedata
{
  bool              quit;     /* Set to true to exit the current game as soon as possible. */
  bool              exit_after_one_level;
  int               starting_girls;
  int               girls;    /* Number of girls left. */
  uint64_t          score;    /* The score. */
  int               diamond_score; /* The diamond score gained. */
  uint64_t          diamonds; /* Total number of diamonds collected. */
  struct cave *     cave;     /* The cave where the game takes place. */
  int               current_level;
  bool              iron_girl_mode;
  trait_t           traits;   /* Traits active during this game. */
  struct playback * playback; /* The playback recorded. */

  struct map *  map;
  struct ai *   ai;

  struct girl *        pyjama_party_girl;
  int                  pyjama_party_girls_passed; /* The number of girls passed the current level so far. */
  enum PARTYCONTROLLER pyjama_party_control;      /* PARTYCONTROLLER_PLAYER = player controls current girl,
                                                   * PARTYCONTROLLER_GIRL   = ai controls current girl,
                                                   * PARTYCONTROLLER_PARTY  = ai controls current girl and the rest of the girls in this level
                                                   */

  bool          player_shift_move;
  int           push_counter;
  int           girl_death_delay_timer;

  int need_to_clear_colorbuffer; /* Counter to zero, when zero, there's no need to clear colorbuffer. */

  int playing_speed; /* 0 = no delay = fastest
                      * 1 = normal speed like the player would be playing
                      * 2 = normal speed / 2
                      * 3 = normal speed / 3
                      * 4 = normal speed / 4
                      * 5 = normal_speed / 5
                      */
  FPSmanager framerate_manager;

  void (*init_map)(void);  /* Callback function to initialize the map. */
  void (*reset_map)(void); /* Reset some of the map, do not initialize it fully. */
};

extern struct gamedata * game(struct cave * cave, int level, bool iron_girl_mode, bool return_after_one_level, struct ai * computer_player);

extern struct gamedata * gamedata_new(struct cave * cave, bool iron_girl_mode, trait_t traits);
extern struct gamedata * gamedata_free(struct gamedata * gamedata);

extern bool game_player_movement(struct map * map, struct gamedata * gamedata);
extern void game_move_girl(struct map * map, int new_x, int new_y);
extern void game_player_next_level(struct gamedata * gamedata);
extern void game_player_death(struct gamedata * gamedata, bool sounds);
extern void game_pyjama_party_ask_next(struct gamedata * gamedata);
extern void game_add_score(int score);
extern void game_show_text(char * text);
extern void game_set_pyjama_party_control(enum PARTYCONTROLLER controller);

#endif
