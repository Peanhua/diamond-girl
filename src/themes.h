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

#ifndef THEMES_H_
#define THEMES_H_

#include "diamond_girl.h"
#include "map.h"
#include <stdbool.h>

struct theme
{
  char * name;
  char * directory;
  char * text;
  bool   enabled;
  struct
  {
    enum MAP_GLYPH      glyph;
    enum MOVE_DIRECTION direction;
  }      icon;
};

enum THEME
  {
    THEME_SPECIAL_DAY,
    THEME_TRAIT,
    THEME_DEFAULT
  };

extern void            themes_initialize(void);
extern void            themes_cleanup(void);
extern struct theme ** themes_get(void);
extern struct theme *  theme_get(enum THEME type);
extern struct theme *  theme_set(enum THEME type, const char * name);

#endif
