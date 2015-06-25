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

#include "diamond_girl.h"
#include "ui.h"
#include "widget.h"
#include "globals.h"
#include "sfx.h"

static void adjust_volume(int adjustment);
static void on_volume_down(bool pressed, SDL_Event * event);
static void on_volume_up(bool pressed, SDL_Event * event);
static void on_mute(bool pressed, SDL_Event * event);


void ui_set_common_handlers(void)
{
  ui_set_handler(UIC_VOLUME_DOWN, on_volume_down);
  ui_set_handler(UIC_VOLUME_UP,   on_volume_up);
  ui_set_handler(UIC_MUTE,        on_mute);
}


static void on_volume_down(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    adjust_volume(-10);
}

static void on_volume_up(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    adjust_volume(10);
}

static void on_mute(bool pressed, SDL_Event * event DG_UNUSED)
{
  static unsigned int saved_volume;

  if(pressed == true)
    {
      if(globals.volume > 0)
        {
          saved_volume = globals.volume;
          adjust_volume(-globals.volume);
        }
      else
        adjust_volume(saved_volume);
    }
}

static void adjust_volume(int adjustment)
{
  int new_volume;

  new_volume = (int) globals.volume + adjustment;
  if(new_volume < 0)
    new_volume = 0;
  else if(new_volume > 100)
    new_volume = 100;

  globals.volume = new_volume;

  sfx_volume(sfx_get_volume());
  sfx_music_volume(sfx_get_music_volume());

  widget_new_osd_volume();

  struct widget * widget;

  widget = widget_find(widget_root(), "slider_volume");
  if(widget != NULL)
    widget_slider_set(widget, globals.volume);
}
