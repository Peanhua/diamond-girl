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

#ifndef MAP_H_
#define MAP_H_

#include "diamond_girl.h"

#include <stdbool.h>

/* Header for the map state files. */
#define DIAMOND_GIRL_MAP_STATE_V1 "dgms1"

enum MAP_GLYPH
{
  MAP_EMPTY,
  MAP_BORDER,
  MAP_ILLEGAL,
  MAP_SAND,
  MAP_PLAYER,
  MAP_PLAYER_ARMOUR0,
  MAP_PLAYER_ARMOUR1,
  MAP_PLAYER_ARMOUR2,
  MAP_BOULDER,
  MAP_BOULDER_FALLING,
  MAP_DIAMOND,
  MAP_DIAMOND_FALLING,
  MAP_ENEMY1,
  MAP_ENEMY2,
  MAP_SMOKE,
  MAP_SMOKE1,
  MAP_SMOKE2,
  MAP_EXIT_LEVEL,
  MAP_EXTRA_LIFE_ANIM,
  MAP_BRICK,
  MAP_BRICK_EXPANDING,
  MAP_BRICK_UNBREAKABLE,
  MAP_BRICK_MORPHER,
  MAP_AMEBA,
  MAP_SLIME,
  MAP_SIZEOF_
};

struct maptarget;
struct gfxbuf;
struct cave;
struct gamedata;

/* Map OBject aka MOB */
enum MAP_OBJECT
  {
    MOB_PLAYER,
    MOB_SIZEOF_
  };

struct map_object
{
  enum MAP_OBJECT     type;               /* The type of this MOB.                                          */
  bool                alive;              /* Alive/existing or not.                                         */
  int                 armour;             /* Current armour level, if any.                                  */
  int                 x, y;               /* Position.                                                      */
  int8_t              offset_x, offset_y; /* Offset from the current position.                              */
  enum MOVE_DIRECTION direction;          /* Facing/movement direction.                                     */
  enum MAP_GLYPH      moved_over_glyph;   /* What was in this position [x,y] before this MOB moved into it. */
  int                 animation;          /* Currently running animation or 0.                              */
  int                 animation_repeats;  /* The number of times the animation cycle is repeated.           */
  uint8_t             chat_chance;        /* The percentage how eagerly the girl will chat something.       */
  char                chat_bubble[16];    /* Displayed next to the girl while timer is greater than zero.   */
  unsigned int        chat_bubble_timer;  /* Number of frames left to display the chat bubble.              */
};


struct map
{
  char * cave_name;
  int    level;
  bool   is_intermission;
  int  width, height;
  int  start_x, start_y;
  int  exit_x, exit_y;
  int  diamonds;
  int  diamonds_needed;
  int  diamond_score;
  int  time_score;
  unsigned char       * data;
  char                * processed;
  enum MOVE_DIRECTION * move_directions;
  int8_t              * move_offsets; /* x and y offsets of movement */
  unsigned int        * order;        /* The order in which the map is processed. */
  int  time;              /* The map time limit in seconds. */
  int  ameba_time;
  int  game_speed;
  bool game_paused;
  bool borders;
#ifdef WITH_OPENGL
  struct gfxbuf * drawbuf;
  struct gfxbuf * borderbuf;
#endif

  struct maptarget * player_target;
  int    morpher_is_on;
  int    morpher_end_time;  /* When the morpher will turn into boulders or diamonds. */
  int    game_time;         /* The current game time left, in frames. Start value is map->time * map->frames_per_second. */
  int    game_timer;        /* From zero to game_speed, to increase game_time by one. */
  int    frames_per_second; /* How many frames per second. */
  int    ameba_blocked;       /* -1 = ameba can expand, 0 = unset, 1 = ameba is being compressed */
  int    ameba_blocked_timer; /* When reached to zero, turn the ameba into diamonds. */
  void * ameba_random_state;  /* State for ameba randomness. */
  int    anim_timer;          /* Map glyph animation timer. */
  int    extra_life_anim;
  bool   exit_open;          /* Has the exit been opened yet. */
  
  /* Level start animation stuff. */
  int             level_start_anim_on;      /* Timer counting towards zero, once zero, the level start animation is ended (or is not on). */
  int             level_start_anim_player;  /* Timer to blink the player glyph during level start animation. */
  unsigned char * level_start_anim_map;     /* The map->data is stored here temporarily while the level start animation is on. */

  /* The display information. */
  int  background_type; /* 0 = normal, 1 = starfield */
  int  glyph_set;       /* 0 = normal, 1 = alternative */
  bool flash_screen;    /* true = flash the screen with white and set to false, false = normal draw */
  int  tilt;            /* Tilt the map down this much. */
  int  map_x, map_y;             /* Top-left mapgrid corner of the map to draw. */
  int  map_fine_x, map_fine_y;   /* Pixel accurate positioning (scrolling). */
  struct girl * girl;            /* The very special main character of the map. */
  bool          in_editor;         /* If it's in editor mode. */
  uint8_t       display_colour[4]; /* The colour to use for displaying the map (used in glColor() call), range=0x00..0xff. */
  bool          fast_forwarding;


  /* Callbacks */
  /* player_movement() returns true if the map hasn't changed (that is: the pointer to it has not changed), false if the map has been changed. */
  bool (*player_movement)(struct map * map, struct gamedata * gamedata);
  void (*player_death)(struct gamedata * gamedata, bool sounds);
  void (*player_next_level)(struct gamedata * gamedata);
  void (*toggle_pause)(struct map * map, bool onoff);
};


struct mappath
{
  struct mappath * parent;
  int              x, y;
  int              cost;
  int              estimate;
  bool             open;
};

struct maptarget
{
  int x, y;
  int original_distance;

  int age;
  struct mappath * path;
};



extern void map_editor(struct cave * cave, int level);

extern struct map * map_get(const char * cave, int level); /* Load or generate a map. Use instead of map_load(), map_random(), etc. */
extern struct map * map_random(int level);
extern struct map * map_well(int level);
extern struct map * map_load(const char * cave, int level);
extern void         map_save(struct map * map);
extern bool         map_load_state(struct map * map, const char * filename);
extern bool         map_save_state(struct map * map, const char * filename);
extern struct map * map_free(struct map * map);
extern struct map * map_new(int width, int height);
extern struct map * map_copy(struct map * dst, struct map * src);
extern void         map_physics_tick(struct map * map, struct gamedata * gamedata);
extern void         map_animations_tick(struct map * map);
extern void         map_scroll_to_girl(struct map * map, int finespeed);

extern void map_level_start_animation_start(struct map * map);
extern void map_level_start_animation_stop(struct map * map);
extern void map_level_start_animation_tick(struct map * map);

extern struct mappath * map_find_path(struct map * map, int fromx, int fromy, int tox, int toy,
                                      bool (*is_passable_cb)(struct map * map, int x, int y, int xmod, int ymod));
extern struct mappath * map_free_path(struct mappath * path);

extern struct maptarget * maptarget_free(struct maptarget * target);

extern struct image * map_thumbnail_image(struct map * map);

extern char * map_calculate_hash(struct map * map);


#endif
