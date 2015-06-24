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

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "game.h"
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>

struct girl;


struct globals
{
  int  fullscreen;
  int  opengl;
  bool opengl_swap_control;
  bool opengl_power_of_two_textures;
  bool opengl_power_of_two_textures_benchmarked;
  bool opengl_compressed_textures;
  int  opengl_max_texture_size;
  bool opengl_1_4;
  bool opengl_1_5;
  int  opengl_max_draw_map_vertices;
  bool gfx_girl_animation;    /* Whether we can use the extra animation(s) of the girl. */
  bool gfx_diamond_animation; /* Whether we can use the extra animation(s) of the diamond. */
  bool skip_intro;
  bool use_sfx;
  bool use_music;
  int  max_channels; /* The maximum number of channels for sfx. */
  bool fps_counter_active;
  bool map_tilting;
  bool smooth_classic_mode;
  bool write_settings; /* If TRUE (default), settings are written on the disk, overwritting their current contents.
                        * Set to FALSE if user wants to manually control the settings.lua file.
                        */
  char * cave_selection;
  int    level_selection;

  char * screen_capture_path;
  int    screen_capture_frameskip;

  char playback_name[128];
  char play_sfx_name[1024];

  bool ai_hilite_current_target;

  bool use_joysticks;
  SDL_Joystick ** joysticks;
  int             joysticks_count;
  bool *          joysticks_analog; /* If the joystick is analog or not. */
  int *           joysticks_digital_counter; /* Increased by one everytime a digital event comes and the joystick is in analog mode.
                                              * When a threshold is passed, the joystick is set (back) to digital mode.
                                              */

  bool   locale_save; // True if the locale is saved to settings.
  char * locale;      // Full locale name.
  char * language;    // The language part of the locale, for example "en" or "fi".
  
#ifndef NDEBUG
  bool backtrace; /* Generate backtraces in some places for debugging. */
  bool quit_after_initialization;
#endif


  unsigned int   volume;
  enum GAME_MODE title_game_mode; /* The current game mode in title screen. */
  bool           iron_girl_mode;

  struct girl **         pyjama_party_girls;
  int                    pyjama_party_girls_size;
};


extern struct globals globals;

extern bool settings_read(void);
extern void settings_read_v1(void);
extern bool settings_read_v2(void);
extern void settings_write(void);
extern void settings_default(void);

extern void set_cave_selection(const char * cave);
extern bool set_language(const char * locale);

#endif
