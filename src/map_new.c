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

#include "gfxbuf.h"
#include "gfx_material.h"
#include "girl.h"
#include "map.h"
#include "random.h"
#include <assert.h>
#include <stdlib.h>
#include <errno.h>


struct map * map_new(int width, int height)
{
  struct map * map;

  assert(width  >= 4);
  assert(height >= 4);

  map = malloc(sizeof *map);
  assert(map != NULL);
  if(map != NULL)
    {
      map->cave_name       = NULL;
      map->level           = 0;
      map->is_intermission = false;
      map->width           = width;
      map->height          = height;
      map->start_x         = 1;
      map->start_y         = 1;
      map->exit_x          = 2;
      map->exit_y          = 1;
      map->diamonds        = 0;
      map->diamonds_needed = 0;
      map->diamond_score   = 0;
      map->time_score      = 0;
      map->data = malloc(map->width * map->height);
      assert(map->data != NULL);

      for(int y = 0; y < map->height; y++)
        for(int x = 0; x < map->width; x++)
          map->data[x + y * map->width] = MAP_SAND;

      for(int x = 0; x < map->width; x++)
	{
	  map->data[x] = MAP_BRICK_UNBREAKABLE;
	  map->data[x + (map->height - 1) * map->width] = MAP_BRICK_UNBREAKABLE;
	}
      for(int y = 0; y < map->height; y++)
	{
	  map->data[y * map->width] = MAP_BRICK_UNBREAKABLE;
	  map->data[map->width - 1 + y * map->width] = MAP_BRICK_UNBREAKABLE;
	}
      map->data[map->start_x + map->start_y * map->width] = MAP_PLAYER;
      map->data[map->exit_x + map->exit_y * map->width] = MAP_BRICK;

      map->processed = malloc(map->width * map->height);
      assert(map->processed != NULL);

      map->move_directions = malloc(map->width * map->height * sizeof(enum MOVE_DIRECTION));
      assert(map->move_directions != NULL);
      for(int y = 0; y < map->height; y++)
        for(int x = 0; x < map->width; x++)
          map->move_directions[x + y * map->width] = MOVE_NONE;

      map->move_offsets = malloc(2 * map->width * map->height * sizeof(int8_t));
      assert(map->move_offsets != NULL);
      for(int y = 0; y < map->height; y++)
        for(int x = 0; x < map->width; x++)
          {
            map->move_offsets[(x + y * map->width) * 2 + 0] = 0;
            map->move_offsets[(x + y * map->width) * 2 + 1] = 0;
          }

      {
        int i;

        map->order = malloc(2 * map->width * map->height * sizeof(unsigned int));
        assert(map->order != NULL);
        i = 0;
        for(int y = 0; y < map->height; y++)
          for(int x = 0; x < map->width; x++)
            {
              map->order[i++] = x;
              map->order[i++] = y;
            }
      }

      map->time         = 0;
      map->ameba_time   = 0;
      map->ameba_random_state = seed_rand_state(1);
      map->game_speed   = 3;
      map->game_paused  = false;
      map->borders      = false;

#ifdef WITH_OPENGL
      map->drawbuf = gfxbuf_new(GFXBUF_DYNAMIC_2D, GL_QUADS, GFXBUF_BLENDING | GFXBUF_TEXTURE);
      if(map->drawbuf != NULL)
        {
          gfxbuf_resize(map->drawbuf, 40 * 40 * 4);
          map->drawbuf->material = gfx_material_new();
        }
      map->borderbuf    = NULL;
#endif

      map->player_target       = NULL;
      map->morpher_is_on       = 0;
      map->morpher_end_time    = 0;
      map->game_time           = 0;
      map->game_timer          = 0;
      map->frames_per_second   = 0;
      map->ameba_blocked       = 0;
      map->ameba_blocked_timer = 0;
      map->anim_timer          = 0;
      map->extra_life_anim     = 0;
      map->exit_open           = false;

      map->level_start_anim_on     = 0;
      map->level_start_anim_player = 0;
      map->level_start_anim_map    = NULL;


      map->background_type = 0;
      map->glyph_set       = 0;
      map->flash_screen    = false;
      map->tilt            = 0;

      map->map_x           = 0;
      map->map_fine_x      = 0;
      map->map_y           = 0;
      map->map_fine_y      = 0;
      
      map->in_editor       = false;
      map->fast_forwarding = false;

      map->player_movement   = NULL;
      map->player_death      = NULL;
      map->player_next_level = NULL;
      map->toggle_pause      = NULL;

      map->girl = girl_new();

      if(map->data              == NULL ||
         map->processed         == NULL ||
         map->move_directions   == NULL ||
#ifdef WITH_OPENGL
         map->drawbuf           == NULL ||
         map->drawbuf->material == NULL ||
#endif
         map->girl              == NULL)
        {
          free(map);
          map = NULL;
        }
    }

  assert(map != NULL);

  return map;
}
