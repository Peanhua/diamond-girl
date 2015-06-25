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

#include "ui.h"

#include <string.h>

char * ui_keyboard_modifier_name(SDLMod mod)
{
  static char name[256];

  strcpy(name, "");
  if(mod & KMOD_LCTRL)  strcat(name, "Lctrl ");
  if(mod & KMOD_RCTRL)  strcat(name, "Rctrl ");
  if(mod & KMOD_RSHIFT) strcat(name, "Rshift ");
  if(mod & KMOD_LSHIFT) strcat(name, "Lshift ");
  if(mod & KMOD_RALT)   strcat(name, "Ralt ");
  if(mod & KMOD_LALT)   strcat(name, "Lalt ");

  size_t len;

  len = strlen(name);
  if(len > 0)
    name[len - 1] = '\0';
  else
    strcpy(name, "none");

  return name;
}
