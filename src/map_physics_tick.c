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
#include "cave.h"
#include "sfx.h"
#include "map.h"
#include "random.h"
#include "girl.h"
#include "traits.h"
#include "game.h"

#include <assert.h>

static void process_map(struct map * map, struct gamedata * gamedata);
static void enemy_death(struct map * map, struct gamedata * gamedata, enum MAP_GLYPH fill, int x, int y);
static void check_ameba(struct map * map, int amebax, int amebay);
static void expand_ameba(struct map * map, int x, int y);

void map_physics_tick(struct map * map, struct gamedata * gamedata)
{
  assert(map != NULL);
  assert(gamedata == NULL || gamedata->map == map);

  if(map->level_start_anim_on)
    {
      map_level_start_animation_tick(map);
    }
  else
    {
      if(map->game_paused == false)
        {
          map->game_timer++;
          if(map->game_timer >= map->game_speed)
            {
              map->game_timer = 0;

              /* The girl is special, she moves first. */
              if(map->girl->mob->alive == true)
                {
                  if(map->player_movement != NULL)
                    {
                      if(map->player_movement(map, gamedata) == false)
                        map = NULL;
                    }
                }
              else if(map->is_intermission == true)
                { /* The girl has died, too bad, but if this is intermission level, then she will get to the next level.
                   * Our copy of map may no longer be valid after player_next_level(), so it's set to NULL.
                   */
                  if(map->player_next_level != NULL)
                    {
                      map->player_next_level(gamedata);
                      map = NULL;
                    }
                }
      
              /* Then process the rest of the map. */
              if(map != NULL)
                process_map(map, gamedata);
            }
        }
    }
}

static void process_map(struct map * map, struct gamedata * gamedata)
{
  assert(map != NULL);

  memset(map->processed, 0, map->width * map->height);

  map->ameba_blocked = 0;
  for(int order = 0; order < map->width * map->height; order++)
    {
      int x, y;

      x = map->order[order * 2 + 0];
      y = map->order[order * 2 + 1];
      if(!map->processed[x + y * map->width])
	{
	  int check_for_player_kill;

	  map->processed[x + y * map->width] = 1;
	  check_for_player_kill = 0;
	  switch((enum MAP_GLYPH) map->data[x + y * map->width])
	    {
	    case MAP_EMPTY:
	    case MAP_BORDER:
	    case MAP_ILLEGAL:
	    case MAP_SAND:
	    case MAP_SMOKE:
	    case MAP_EXIT_LEVEL:
	    case MAP_EXTRA_LIFE_ANIM:
	    case MAP_BRICK:
	    case MAP_BRICK_UNBREAKABLE:
	    case MAP_BRICK_MORPHER:
	    case MAP_PLAYER_ARMOUR0:
	    case MAP_PLAYER_ARMOUR1:
	    case MAP_PLAYER_ARMOUR2:
            case MAP_TREASURE:
	      break;
	    case MAP_BRICK_EXPANDING:
	      if(x > 0 && map->data[x - 1 + y * map->width] == MAP_EMPTY)
		{
		  map->data[x - 1 + y * map->width]      = MAP_BRICK_EXPANDING;
		  map->processed[x - 1 + y * map->width] = 1;
                  if(map->fast_forwarding == false)
                    sfx_emit(SFX_BOULDER_MOVE, x - 1, y);
		}
	      else if(x + 1 < map->width && map->data[x + 1 + y * map->width] == MAP_EMPTY)
		{
		  map->data[x + 1 + y * map->width]      = MAP_BRICK_EXPANDING;
		  map->processed[x + 1 + y * map->width] = 1;
                  if(map->fast_forwarding == false)
                    sfx_emit(SFX_BOULDER_MOVE, x + 1, y);
		}
	      break;
	    case MAP_SMOKE2:
	      map->data[x + y * map->width] = MAP_SMOKE1;
	      break;
	    case MAP_SMOKE1:
	      map->data[x + y * map->width] = MAP_EMPTY;
	      break;
	    case MAP_ENEMY1:
	      {
		enum MOVE_DIRECTION current_direction;
		int nx, ny, do_move;
		
		check_for_player_kill = 1;

		current_direction = map->move_directions[x + y * map->width];

		/* try to turn left */
		switch(current_direction)
		  {
		  case MOVE_NONE:
		    break;
		  case MOVE_UP:
		    current_direction = MOVE_LEFT;
		    break;
		  case MOVE_RIGHT:
		    current_direction = MOVE_UP;
		    break;
		  case MOVE_DOWN:
		    current_direction = MOVE_RIGHT;
		    break;
		  case MOVE_LEFT:
		    current_direction = MOVE_DOWN;
		    break;
		  }

		nx = x;
		ny = y;
		do_move = 0;
		for(int i = 0; i < 2 && !do_move; i++)
		  switch(current_direction)
		    {
		    case MOVE_NONE:
		      current_direction = MOVE_UP;
		      i = 2;
		      break;
		    case MOVE_UP:
		      if(y > 0 && map->data[x + (y - 1) * map->width] == MAP_EMPTY)
			{
			  ny--;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_RIGHT;
		      break;
		    case MOVE_RIGHT:
		      if(x < map->width - 1 && map->data[x + 1 + y * map->width] == MAP_EMPTY)
			{
			  nx++;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_DOWN;
		      break;
		    case MOVE_DOWN:
		      if(y < map->height - 1 && map->data[x + (y + 1) * map->width] == MAP_EMPTY)
			{
			  ny++;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_LEFT;
		      break;
		    case MOVE_LEFT:
		      if(x > 0 && map->data[x - 1 + y * map->width] == MAP_EMPTY)
			{
			  nx--;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_UP;
		      break;
		    }

		map->data[x + y * map->width] = MAP_EMPTY;
		map->data[nx + ny * map->width]            = MAP_ENEMY1;
		map->processed[nx + ny * map->width]       = 1;
		map->move_directions[nx + ny * map->width] = current_direction;
                map->move_offsets[2 * (nx + ny * map->width) + 0] = (x - nx) * 23;
                map->move_offsets[2 * (nx + ny * map->width) + 1] = (y - ny) * 23;
	      }
	      break;
	    case MAP_ENEMY2:
	      {
		enum MOVE_DIRECTION current_direction;
		int nx, ny, do_move;

		check_for_player_kill = 1;

		current_direction = map->move_directions[x + y * map->width];

		/* try to turn right */
		switch(current_direction)
		  {
		  case MOVE_NONE:
		    current_direction = MOVE_DOWN;
		    break;
		  case MOVE_UP:
		    current_direction = MOVE_RIGHT;
		    break;
		  case MOVE_RIGHT:
		    current_direction = MOVE_DOWN;
		    break;
		  case MOVE_DOWN:
		    current_direction = MOVE_LEFT;
		    break;
		  case MOVE_LEFT:
		    current_direction = MOVE_UP;
		    break;
		  }

		nx = x;
		ny = y;
		do_move = 0;
		for(int i = 0; i < 2 && !do_move; i++)
		  switch(current_direction)
		    {
		    case MOVE_NONE:
		      current_direction = MOVE_UP;
		      break;
		    case MOVE_UP:
		      if(y > 0 && map->data[x + (y - 1) * map->width] == MAP_EMPTY)
			{
			  ny--;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_LEFT;
		      break;
		    case MOVE_RIGHT:
		      if(x < map->width - 1 && map->data[x + 1 + y * map->width] == MAP_EMPTY)
			{
			  nx++;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_UP;
		      break;
		    case MOVE_DOWN:
		      if(y < map->height - 1 && map->data[x + (y + 1) * map->width] == MAP_EMPTY)
			{
			  ny++;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_RIGHT;
		      break;
		    case MOVE_LEFT:
		      if(x > 0 && map->data[x - 1 + y * map->width] == MAP_EMPTY)
			{
			  nx--;
			  do_move = 1;
			}
		      else
			current_direction = MOVE_DOWN;
		      break;
		    }

		map->data[x + y * map->width] = MAP_EMPTY;
		map->data[nx + ny * map->width]            = MAP_ENEMY2;
		map->processed[nx + ny * map->width]       = 1;
		map->move_directions[nx + ny * map->width] = current_direction;
                map->move_offsets[2 * (nx + ny * map->width) + 0] = (x - nx) * 23;
                map->move_offsets[2 * (nx + ny * map->width) + 1] = (y - ny) * 23;
	      }
	      break;
	    case MAP_BOULDER:
	    case MAP_DIAMOND:
	      if(y < map->height - 1)
		{
		  int fall;
		  char type;

		  type = map->data[x + y * map->width];

		  fall = 0;
		  if(map->data[x + (y + 1) * map->width] == MAP_EMPTY)
		    fall = map->width;
		  else if(map->data[x + (y + 1) * map->width] == MAP_BOULDER ||
                          map->data[x + (y + 1) * map->width] == MAP_DIAMOND ||
			  map->data[x + (y + 1) * map->width] == MAP_BRICK)
		    {
		      if(x > 0                   && map->data[x - 1 + y * map->width] == MAP_EMPTY && map->data[x - 1 + (y + 1) * map->width] == MAP_EMPTY)
			fall = -1;
		      else if(x < map->width - 1 && map->data[x + 1 + y * map->width] == MAP_EMPTY && map->data[x + 1 + (y + 1) * map->width] == MAP_EMPTY)
			fall = 1;
		    }
                  else if(y < map->height - 2 && map->data[x + (y + 1) * map->width] == MAP_SLIME)
                    {
                      if((int) map->move_directions[x + (y + 1) * map->width] >= 40) // Typecasted to int because move_directions is enum MOVE_DIRECTION.
                        {
                          map->move_directions[x + (y + 1) * map->width] = 0;
                          if(map->data[x + (y + 2) * map->width] == MAP_EMPTY)
                            {
                              map->data[x + y * map->width] = MAP_EMPTY;
                              map->data[x + (y + 2) * map->width]            = type;
                              map->processed[x + (y + 2) * map->width]       = 1;
                              map->move_directions[x + (y + 2) * map->width] = MOVE_NONE;
                            }
                        }
                      map->move_directions[x + (y + 1) * map->width] += 1;
                    }

		  if(fall)
		    {
		      int bx, by;

		      bx = x;
		      by = y;
		      if(fall == 1 || fall == -1)
			{
			  map->data[x + y * map->width] = MAP_EMPTY;
			  bx += fall;
                          map->move_offsets[2 * (bx + by * map->width) + 0] = -fall * 23;
                          map->move_offsets[2 * (bx + by * map->width) + 1] = 0;
			}
		      if(type == MAP_BOULDER)
                        map->data[bx + by * map->width] = MAP_BOULDER_FALLING;
		      else if(type == MAP_DIAMOND)
			map->data[bx + by * map->width] = MAP_DIAMOND_FALLING;
		      else
			{
			  assert(0);
			}
		      map->processed[bx + by * map->width]       = 1;
                      map->move_directions[bx + by * map->width] = MOVE_NONE;
		    }
		}
	      break;
	    case MAP_BOULDER_FALLING:
	    case MAP_DIAMOND_FALLING:
	      {
		int cont_falling;

		cont_falling = 0;
		if(y < map->height - 1)
		  {
		    switch(map->data[x + (y + 1) * map->width])
		      {
		      case MAP_EMPTY:
			cont_falling = 1;
			break;
		      case MAP_PLAYER:
			map->girl->mob->armour = 0;
                        if(map->player_death != NULL)
                          map->player_death(gamedata, 1);
			break;
		      case MAP_ENEMY1:
			enemy_death(map, gamedata, MAP_SMOKE2, x, y + 1);
			break;
		      case MAP_ENEMY2:
			enemy_death(map, gamedata, MAP_DIAMOND, x, y + 1);
			break;
		      case MAP_BRICK_MORPHER:
			if(map->morpher_is_on == 0)
			  {
			    map->morpher_is_on = 1;
			    map->morpher_end_time = map->game_time - 30 * map->frames_per_second;
			  }
			if(map->morpher_is_on == 1)
			  {
			    if(y < map->height - 2)
			      if(map->data[x + (y + 2) * map->width] == MAP_EMPTY)
				{
				  enum MAP_GLYPH g;
				  
				  if(map->data[x + y * map->width] == MAP_DIAMOND_FALLING)
				    g = MAP_BOULDER;
				  else
				    g = MAP_DIAMOND;

				  map->data[x + (y + 2) * map->width]            = g;
				  map->processed[x + (y + 2) * map->width]       = 1;
                                  map->move_directions[x + (y + 2) * map->width] = MOVE_NONE;
                                  if(map->fast_forwarding == false)
                                    sfx_emit(SFX_DIAMOND_COLLECT, x, y + 2);
				}
			    map->data[x + y * map->width] = MAP_EMPTY;
			  }
			break;
		      }
		  }

		{
		  char type;

		  type = map->data[x + y * map->width];

		  if(cont_falling)
		    {
		      map->data[x + (y + 1) * map->width]            = type;
		      map->processed[x + (y + 1) * map->width]       = 1;
                      map->move_directions[x + (y + 1) * map->width] = MOVE_NONE;
		      map->data[x + y * map->width] = MAP_EMPTY;
                      map->move_offsets[2 * (x + (y + 1) * map->width) + 0] =   0;
                      map->move_offsets[2 * (x + (y + 1) * map->width) + 1] = -23;
		    }
		  else
		    {
		      if(type == MAP_BOULDER_FALLING)
			{
                          int dist;

                          dist = abs(map->girl->mob->x - x) + abs(map->girl->mob->y - y);
                          if(dist < 10)
                            dist = 0;
                          else if(dist < 20)
                            dist = 1;
                          else
                            dist = 2;

			  map->data[x + y * map->width]            = MAP_BOULDER;
			  map->processed[x + y * map->width]       = 1;
                          if(map->fast_forwarding == false)
                            sfx_emit(SFX_BOULDER_FALL, x, y);

                          map->tilt += 3 - dist;
                          if(map->tilt > 12)
                            map->tilt = 12;
			}
		      else if(type == MAP_DIAMOND_FALLING)
			{
			  map->data[x + y * map->width]            = MAP_DIAMOND;
			  map->processed[x + y * map->width]       = 1;
                          map->move_directions[x + y * map->width] = MOVE_NONE;
                          if(map->fast_forwarding == false)
                            sfx_emit(SFX_DIAMOND_FALL, x, y);
			}
		    }
		}
	      }
	      break;
	    case MAP_PLAYER:
	      break;
	    case MAP_AMEBA:
	      {
		int dx, dy;

		dx = -1;
		dy = -1;
		if(x > 0 && (map->data[x - 1 + y * map->width] == MAP_ENEMY1 || map->data[x - 1 + y * map->width] == MAP_ENEMY2))
		  {
		    dx = x - 1;
		    dy = y;
		  }
		else if(x < map->width - 1 && (map->data[x + 1 + y * map->width] == MAP_ENEMY1 || map->data[x + 1 + y * map->width] == MAP_ENEMY2))
		  {
		    dx = x + 1;
		    dy = y;
		  }
		else if(y > 0 && (map->data[x + (y - 1) * map->width] == MAP_ENEMY1 || map->data[x + (y - 1) * map->width] == MAP_ENEMY2))
		  {
		    dx = x;
		    dy = y - 1;
		  }
		else if(y < map->height - 1 && (map->data[x + (y + 1) * map->width] == MAP_ENEMY1 || map->data[x + (y + 1) * map->width] == MAP_ENEMY2))
		  {
		    dx = x;
		    dy = y + 1;
		  }

		if(dx != -1)
		  {
		    if(map->data[dx + dy * map->width] == MAP_ENEMY1)
		      enemy_death(map, gamedata, MAP_SMOKE2, dx, dy);
		    else if(map->data[dx + dy * map->width] == MAP_ENEMY2)
		      enemy_death(map, gamedata, MAP_DIAMOND, dx, dy);
		  }
	      }
	      if(map->data[x + y * map->width] == MAP_AMEBA)
		{
		  int r;
		  
		  r = get_rand_state(200, map->ameba_random_state);
		  if(r < 5)
		    expand_ameba(map, x - 1, y);
		  else if(r < 10)
		    expand_ameba(map, x + 1, y);
		  else if(r < 15)
		    expand_ameba(map, x, y - 1);
		  else if(r < 20)
		    expand_ameba(map, x, y + 1);

                  if(map->ameba_blocked == 0 || map->ameba_blocked == 1)
                    check_ameba(map, x, y);
		}
	      break;
            case MAP_SLIME:
              break;
            case MAP_SIZEOF_:
              break;
	    }

	  if(check_for_player_kill)
            if(map->player_death != NULL)
              {
                if(x > 0 && map->data[x - 1 + y * map->width] == MAP_PLAYER)
                  map->player_death(gamedata, 1);
                else if(x < map->width - 1 && map->data[x + 1 + y * map->width] == MAP_PLAYER)
                  map->player_death(gamedata, 1);
                else if(y > 0 && map->data[x + (y - 1) * map->width] == MAP_PLAYER)
                  map->player_death(gamedata, 1);
                else if(y < map->height - 1 && map->data[x + (y + 1) * map->width] == MAP_PLAYER)
                  map->player_death(gamedata, 1);
              }
	}
    }

  if(map->ameba_blocked == -1)
    map->ameba_blocked_timer = map->frames_per_second * 4;
  else if(map->ameba_blocked == 1 && map->ameba_blocked_timer > 0)
    map->ameba_blocked_timer--; 

  if((map->ameba_blocked == 1 && map->ameba_blocked_timer == 0) || (map->ameba_time > 0 && map->ameba_time == map->game_time / map->frames_per_second))
    { /* turn ameba into diamonds or boulders */
      enum MAP_GLYPH g;

      sfx_stop();
      map->ameba_time = 0;

      g = MAP_DIAMOND;
      for(int y = 0; y < map->height && g == MAP_DIAMOND; y++)
        for(int x = 0; x < map->width && g == MAP_DIAMOND; x++)
          if(map->data[x + y * map->width] == MAP_AMEBA)
            {
              if(x > 0 && (map->data[x - 1 + y * map->width] == MAP_EMPTY || map->data[x - 1 + y * map->width] == MAP_SAND))
                g = MAP_BOULDER;
              else if(x < map->width - 1 && (map->data[x + 1 + y * map->width] == MAP_EMPTY || map->data[x + 1 + y * map->width] == MAP_SAND))
                g = MAP_BOULDER;
              else if(y > 0 && (map->data[x + (y - 1) * map->width] == MAP_EMPTY || map->data[x + (y - 1) * map->width] == MAP_SAND))
                g = MAP_BOULDER;
              else if(y < map->height - 1 && (map->data[x + (y + 1) * map->width] == MAP_EMPTY || map->data[x + (y + 1) * map->width] == MAP_SAND))
                g = MAP_BOULDER;
            }

      for(int y = 0; y < map->height; y++)
        for(int x = 0; x < map->width; x++)
          if(map->data[x + y * map->width] == MAP_AMEBA)
            map->data[x + y * map->width] = g;
    }
  
  if(map->morpher_is_on == 1)
    if(map->game_time <= map->morpher_end_time)
      map->morpher_is_on = 2;

}


static void explode_at(struct map * map, struct gamedata * gamedata, enum MAP_GLYPH fill, int inx, int iny)
{
  if(inx >= 0 && inx < map->width && iny >= 0 && iny < map->height)
    {
      if(map->data[inx + iny * map->width] == MAP_PLAYER)
        {
          map->girl->mob->armour = 0;
          if(map->player_death != NULL)
            map->player_death(gamedata, 1);
        }
      
      if(map->data[inx + iny * map->width] != MAP_BRICK_UNBREAKABLE && map->data[inx + iny * map->width] != MAP_TREASURE)
        {	    
          map->data[inx + iny * map->width]            = fill;
          map->processed[inx + iny * map->width]       = 1;
          map->move_directions[inx + iny * map->width] = MOVE_NONE;
        }
    }
}

static void enemy_death(struct map * map, struct gamedata * gamedata, enum MAP_GLYPH fill, int x, int y)
{

  if(map->fast_forwarding == false)
    sfx_emit(SFX_EXPLOSION, x, y);

  for(int iy = -1; iy <= 1; iy++)
    for(int ix = -1; ix <= 1; ix++)
      explode_at(map, gamedata, fill, ix + x, iy + y);

  if(gamedata != NULL)
    if(gamedata->traits & TRAIT_DYNAMITE)
      {
        explode_at(map, gamedata, fill, x - 2, y);
        explode_at(map, gamedata, fill, x + 2, y);
        explode_at(map, gamedata, fill, x,     y - 2);
        explode_at(map, gamedata, fill, x,     y + 2);
      }
}

static void check_ameba(struct map * map, int amebax, int amebay)
{
  if(map->ameba_blocked == 0 || map->ameba_blocked == 1)
    {
      bool found;
      
      found = false;
      for(int yy = -1; found == false && yy <= 1; yy++)
        for(int xx = -1; found == false && xx <= 1; xx++)
          if((yy == 0 && xx != 0) || (xx == 0 && yy != 0))
            if(amebax + xx >= 0 && amebax + xx < map->width && amebay + yy >= 0 && amebay + yy < map->height)
              {
                enum MAP_GLYPH g;
                
                g = map->data[amebax + xx + (amebay + yy) * map->width];
                if(g == MAP_EMPTY || g == MAP_SAND)
                  found = true;
              }
      if(found == true)
        map->ameba_blocked = -1; /* Could expand, but did not this time. */
      else
        map->ameba_blocked = 1; /* Can not expand. */
    }
}


static void expand_ameba(struct map * map, int x, int y)
{
  if(x >= 0 && x < map->width && y >= 0 && y < map->height)
    {
      if(map->data[x + y * map->width] == MAP_EMPTY ||
         map->data[x + y * map->width] == MAP_SAND)
        {
          map->data[x + y * map->width]            = MAP_AMEBA;
          map->processed[x + y * map->width]       = 1;
          map->move_directions[x + y * map->width] = MOVE_NONE;
          map->ameba_blocked = -1; /* Could expand, so this round is ok. */
        }
    }
}
