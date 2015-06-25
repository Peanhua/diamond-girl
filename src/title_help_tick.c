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
#include "title.h"
#include "sfx.h"
#include "widget.h"
#include "map.h"
#include "girl.h"

void title_help_tick(struct widget * map_widget)
{
  struct map * map;
  int help_screen;

  map = widget_get_pointer(map_widget, "map");
  help_screen = widget_get_ulong(map_widget, "help_screen");

  /* Add the girl to the background scene */
  if(help_screen == 2 && map != NULL && map->girl->mob->alive == false)
    {
      int pos;
      
      map->girl->mob->alive = true;
      
      map->girl->mob->x = widget_absolute_x(map_widget) / 24;
      map->girl->mob->y = widget_absolute_y(map_widget) / 24;
      map->girl->mob->direction = MOVE_NONE;
      pos = map->girl->mob->x + map->girl->mob->y * map->width;
      map->data[pos] = MAP_PLAYER;
      map->move_directions[pos] = MOVE_NONE;
    }
  

  /* Adjust music volume */
  if(help_screen < 4)
    {
      float curvol;
      
      curvol = sfx_get_music_volume();
      if(curvol < 1.0f)
        {
          curvol += 0.001f;
          if(curvol > 1.0f)
            curvol = 1.0f;
          sfx_music_volume(curvol);
        }
    }
  else if(help_screen > 8)
    {
      float curvol;

      curvol = sfx_get_music_volume();
      if(curvol > 0.0f)
        {
          curvol -= 0.003f;
          if(curvol < 0.0f)
            curvol = 0.0f;
          sfx_music_volume(curvol);
        }
    }
}
