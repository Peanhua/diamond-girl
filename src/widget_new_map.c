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

#include "widget.h"
#include "gfx.h"
#include "game.h"
#include <assert.h>

static void draw(struct widget * this);

struct widget * widget_new_map(struct widget * parent, int x, int y, int width, int height, struct map * map, enum GAME_MODE game_mode)
{
  struct widget * obj;

  if(parent == NULL)
    parent = widget_root();

  if(parent == NULL)
    obj = widget_new_root(x, y, width, height);
  else
    obj = widget_new_child(parent, x, y, width, height);
  assert(obj != NULL);
  if(obj != NULL)
    {
      widget_set_on_draw(obj, draw);
      widget_set_ulong(obj, "type", WT_MAP);
      widget_set_flags(obj, 0);
      widget_set_map_pointer(obj, "map", map);
      widget_set_ulong(obj, "game_mode", game_mode);
      widget_set_enabled(obj, true);
    }

  return obj;
}


static void draw(struct widget * this)
{
  if(widget_enabled(this) == true)
    {
      struct map * map;

      map = widget_get_map_pointer(this, "map");
      if(map != NULL)
        {
          enum GAME_MODE game_mode;
          trait_t traits;
          int map_width, map_height;

          game_mode = widget_get_ulong(this, "game_mode");
          traits = traits_get(game_mode);
          map_width  = widget_width(this)  / 24;
          map_height = widget_height(this) / 24;
          
          draw_map(map, game_mode, traits,
                   widget_absolute_x(this), widget_absolute_y(this),
                   map_width, map_height);
        }
    }
}
