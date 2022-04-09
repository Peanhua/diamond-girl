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

#include "ui.h"

#include <assert.h>


char * ui_command_name(enum UI_COMMAND command)
{
  static char * s_commands[UIC_SIZEOF_] =
    {
      "draw",
      "exit",
      "cancel",
      "up",
      "down",
      "left",
      "right",
      "pause",
      "manipulate",
      "alt_manipulate",
      "scroll_up",
      "scroll_down",
      "toggle_fullscreen",
      "slower",
      "faster",
      "hilite_target",
      "unknown_key",
      "save_playback",
      "show_help",
      "show_legend",
      "volume_down",
      "volume_up",
      "mute",
      "recording_start",
      "recording_stop",
      "catch_all_keyboard",
      "catch_all_mouse",
      "catch_all_joysticks"
    };

  assert(command < UIC_SIZEOF_);

  return s_commands[command];
}
