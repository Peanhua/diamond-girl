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

#ifndef TITLE_H_
#define TITLE_H_

#include <stdbool.h>

struct gamedata;
struct map;
struct playback;
struct widget;
struct cave;


extern void title(void);

extern struct gamedata * title_load_playback(void);

extern void draw_title_logo_initialize(struct map * the_map);
extern void draw_title_logo_cleanup(void);
extern void draw_title_logo(void);

extern void title_map_tick(struct map * map);
extern void title_help_tick(struct widget * map_widget);

extern bool title_map_girl_movement(struct map * map, struct gamedata * gamedata);
extern void title_update_newgame_navigation(struct widget * root);

extern void draw_title_starfield_initialize(void);
extern void draw_title_starfield(void);

extern void title_game_intro(void);
extern void title_game(struct cave * cave, struct playback * playback, char * playback_title);

#endif
