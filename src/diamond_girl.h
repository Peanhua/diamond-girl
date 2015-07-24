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

#ifndef DIAMOND_GIRL_H
#define DIAMOND_GIRL_H

#ifdef HAVE_CONFIG_H
# include "../config.h"
#endif


#include <SDL/SDL.h>
#include <time.h>
#include <stdbool.h>

#if defined(__amigaos4__) || defined(__MORPHOS__)
# if !defined(AMIGA)
#  define AMIGA
# endif
#endif

struct ai;
struct map;
struct playback;
struct widget;

enum MOVE_DIRECTION { MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT, MOVE_NONE };

enum GENDER { GENDER_NEUTER, GENDER_FEMALE, GENDER_MALE, GENDER_FAMILY };


/* main functions */
extern void intro(void);

/* utility functions */
extern bool         parse_commandline_arguments(int argc, char * argv[]);
extern const char * get_save_filename(const char * name);
extern const char * get_data_filename(const char * name);
extern bool         read_file(const char * filename, char ** buffer_p, int * size_p);
extern bool         write_file(const char * filename, char * buffer, int size);
extern char **      read_text_file(const char * filename);
extern char **      read_localized_text_file(const char * filename);
extern void         set_max_level_selection(int new_max); /* The maximum starting level for the player. */
extern bool pyjama_party_load(void);
extern bool pyjama_party_save(void);

#if !defined(AMIGA)
extern char * get_user_homedir(void);
#endif
extern bool   setup_user_homedir(void);
extern bool   check_installation(void);

extern unsigned int round_to_power_of_two(unsigned int n);


/* Tell compiler that the argument is not being used in this function. */
#ifdef __GNUC__
# define DG_UNUSED __attribute__((unused))
#else
# define DG_UNUSED
#endif
/* Same as above, but only with non-debug builds. */
#ifdef NDEBUG
# define DG_NDEBUG_UNUSED DG_UNUSED
#else
# define DG_NDEBUG_UNUSED
#endif
/* Unused when building without OpenGL. */
#ifndef WITH_OPENGL
# define DG_UNUSED_WITHOUT_OPENGL DG_UNUSED
#else
# define DG_UNUSED_WITHOUT_OPENGL
#endif

/* Debugging */
#ifndef NDEBUG
# define DG_DEBUG do { if(globals.debugging_enabled == true) fprintf(stderr, "%s:%d:%s()\n", __FILE__, __LINE__, __FUNCTION__); } while(0)
#else
# define DG_DEBUG
#endif

#endif


#ifdef __GNUC__
# define DG_PRINTF_FORMAT1 __attribute__((format (printf, 1, 2)))
# define DG_PRINTF_FORMAT2 __attribute__((format (printf, 2, 3)))
# define DG_PRINTF_FORMAT3 __attribute__((format (printf, 3, 4)))
#else
# define DG_PRINTF_FORMAT1
# define DG_PRINTF_FORMAT2
# define DG_PRINTF_FORMAT3
#endif
