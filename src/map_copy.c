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

#include "map.h"
#include "random.h"
#include <assert.h>

/* Note that not everything are copied:
 *  level start animation stuff
 *  girl
 *  processed
 *  move_offsets
 *  drawbuf
 *  borderbuf
 */

struct map * map_copy(struct map * dst, struct map * src)
{
  assert(src != NULL);

  if(dst == NULL)
    dst = map_get(src->cave_name, src->level);

  assert(dst != NULL);
  assert(!strcmp(dst->cave_name, src->cave_name));
  assert(dst->level == src->level);

  dst->is_intermission     = src->is_intermission;
  dst->start_x             = src->start_x;
  dst->start_y             = src->start_y;
  dst->diamonds            = src->diamonds;
  dst->diamonds_needed     = src->diamonds_needed;
  dst->diamond_score       = src->diamond_score;
  dst->time_score          = src->time_score;
  dst->time                = src->time;
  dst->ameba_time          = src->ameba_time;
  dst->game_speed          = src->game_speed;
  dst->game_paused         = src->game_paused;
  dst->borders             = src->borders;
  dst->morpher_is_on       = src->morpher_is_on;
  dst->morpher_end_time    = src->morpher_end_time;
  dst->game_time           = src->game_time;
  dst->game_timer          = src->game_timer;
  dst->frames_per_second   = src->frames_per_second;
  dst->ameba_blocked       = src->ameba_blocked;
  dst->ameba_blocked_timer = src->ameba_blocked_timer;
  dst->ameba_random_state  = rand_state_duplicate(src->ameba_random_state);
  dst->anim_timer          = src->anim_timer;
  dst->extra_life_anim     = src->extra_life_anim;
  dst->exit_open           = src->exit_open;
  dst->background_type     = src->background_type;
  dst->glyph_set           = src->glyph_set;
  dst->flash_screen        = src->flash_screen;
  dst->tilt                = src->tilt;
  dst->map_x               = src->map_x;
  dst->map_y               = src->map_y;
  dst->map_fine_x          = src->map_fine_x;
  dst->map_fine_y          = src->map_fine_y;
  dst->in_editor           = src->in_editor;
  dst->fast_forwarding     = src->fast_forwarding;
  dst->player_movement     = src->player_movement;
  dst->player_death        = src->player_death;
  dst->player_next_level   = src->player_next_level;
  dst->toggle_pause        = src->toggle_pause;

  for(int i = 0; i < 4; i++)
    dst->display_colour[i] = src->display_colour[i];

  for(int y = 0; y < src->height; y++)
    for(int x = 0; x < src->width; x++)
      {
        dst->data[x + y * src->width]            = src->data[x + y * src->width];
        dst->move_directions[x + y * src->width] = src->move_directions[x + y * src->width];
        dst->order[(x + y * src->width) * 2 + 0] = src->order[(x + y * src->width) * 2 + 0];
        dst->order[(x + y * src->width) * 2 + 1] = src->order[(x + y * src->width) * 2 + 1];
      }

  return dst;
}
