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

#include "cave.h"
#include "game.h"
#include "map.h"
#include "girl.h"
#include "random.h"
#include "sfx.h"
#include "ai.h"
#include "traits.h"
#include <assert.h>

bool game_player_movement(struct map * map, struct gamedata * gamedata)
{
  int new_x, new_y;
  int8_t offx, offy;
  bool rv;

  assert(gamedata->map == map);
  rv = true;
  if(map->girl->mob->direction != MOVE_NONE)
    {
      map->data[map->girl->mob->x + map->girl->mob->y * map->width] = MAP_EMPTY;
    }

  new_x = map->girl->mob->x;
  new_y = map->girl->mob->y;
  
  offx = 0;
  offy = 0;

  switch(map->girl->mob->direction)
    {
    case MOVE_NONE:
      break;
    case MOVE_UP:
      if(map->girl->mob->y > 0)
        {
          new_y--;
          offy = 23;
        }
      break;
    case MOVE_DOWN:
      if(map->girl->mob->y < map->height - 1)
        {
          new_y++;
          offy = -23;
        }
      break;
    case MOVE_LEFT:
      if(map->girl->mob->x > 0)
        {
          new_x--;
          offx = 23;
        }
      break;
    case MOVE_RIGHT:
      if(map->girl->mob->x < map->width - 1)
        {
          new_x++;
          offx = -23;
        }
      break;
    }

  if(map->girl->mob->direction != MOVE_NONE)
    {
      bool pushing;

      pushing = false;
      switch(map->data[new_x + new_y * map->width])
	{
	case MAP_EXIT_LEVEL:
          sfx_emit(SFX_ENTER_EXIT, new_x, new_y);
          map->girl->mob->moved_over_glyph = map->data[new_x + new_y * map->width];
          game_move_girl(map, new_x, new_y);
	  map->player_next_level(gamedata);
          rv = false;
          map = NULL;
          gamedata = NULL;
	  break;
	case MAP_DIAMOND_FALLING:
          if(map->girl->mob->direction == MOVE_UP)
            {
              map->girl->mob->armour = 0;
              map->player_death(gamedata, true);
            }
          /* fall-thru */
	case MAP_DIAMOND:
          if(map->girl->mob->alive == true)
            {
              map->diamonds++;
              if(gamedata != NULL)
                {
                  gamedata->diamonds++;
                  if(gamedata->pyjama_party_girl != NULL)
                    {
                      gamedata->pyjama_party_girl->diamonds++;
 
                      if(gamedata->pyjama_party_girl->possible_traits != 0)
                        if(gamedata->pyjama_party_girl->diamonds >= gamedata->pyjama_party_girl->diamonds_to_trait)
                          { /* Enough diamonds for next trait, pick a random one using map->ameba_random_state for predictable results. */
                            trait_t traits[TRAIT_SIZEOF_];
                            int traitcount;
                            trait_t trait;
                            
                            traitcount = 0;
                            for(trait_t i = 0; i < TRAIT_SIZEOF_; i++)
                              if(gamedata->pyjama_party_girl->possible_traits & (1<<i))
                                {
                                  traits[traitcount] = 1<<i;
                                  traitcount++;
                                }
                            assert(traitcount > 0);
                            trait = traits[get_rand_state(traitcount, gamedata->map->ameba_random_state)];
                            gamedata->pyjama_party_girl->traits |= trait;
                            gamedata->pyjama_party_girl->possible_traits &= ~trait;
                            gamedata->map->girl->traits = gamedata->pyjama_party_girl->traits;
                            gamedata->traits = gamedata->pyjama_party_girl->traits;

                            /* Next trait costs twice as much. */
                            gamedata->pyjama_party_girl->diamonds_to_trait *= 2;
                          }
                    }
                  game_add_score(map->diamond_score);
                }
              if(map->fast_forwarding == false)
                sfx_emit(SFX_DIAMOND_COLLECT, new_x, new_y);
              if(map->exit_open == false && map->diamonds >= map->diamonds_needed)
                {
                  map->exit_open = true;
                  map->data[map->exit_x + map->exit_y * map->width] = MAP_EXIT_LEVEL;
                  if(map->fast_forwarding == false)
                    sfx_emit(SFX_SMALL_EXPLOSION, new_x, new_y);
                  map->flash_screen = true;
                }

              if(gamedata != NULL && gamedata->player_shift_move)
                {
                  map->data[new_x + new_y * map->width] = MAP_EMPTY;
                  map->processed[new_x + new_y * map->width] = 1;
                }
              else
                {
                  map->girl->mob->moved_over_glyph = MAP_DIAMOND;
                  game_move_girl(map, new_x, new_y);
                  map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 0] = offx;
                  map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 1] = offy;
                }

              if(gamedata != NULL && gamedata->ai != NULL)
                if(gamedata->ai->diamond_collected != NULL)
                  gamedata->ai->diamond_collected(gamedata->ai);
            }
	  break;
	case MAP_EMPTY:
	case MAP_SAND:
	  if(map->data[new_x + new_y * map->width] == MAP_EMPTY)
            {
              if(map->fast_forwarding == false)
                sfx_emit(SFX_MOVE_EMPTY, new_x, new_y);
            }
	  else if(map->data[new_x + new_y * map->width] == MAP_SAND)
            {
              if(map->fast_forwarding == false)
                sfx_emit(SFX_MOVE_SAND, new_x, new_y);
            }

	  if(gamedata->player_shift_move)
	    {
	      map->data[new_x + new_y * map->width] = MAP_EMPTY;
	      map->processed[new_x + new_y * map->width] = 1;
	    }
	  else
	    {
              map->girl->mob->moved_over_glyph = map->data[new_x + new_y * map->width];
              game_move_girl(map, new_x, new_y);
              map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 0] = offx;
              map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 1] = offy;
	    }
	  break;
	case MAP_BOULDER:
	case MAP_BOULDER_FALLING:
	  if(map->girl->mob->direction   == MOVE_LEFT  ||
             map->girl->mob->direction   == MOVE_RIGHT ||
             ( map->girl->mob->direction == MOVE_UP && (gamedata->traits & TRAIT_DIAMOND_PUSH) ))
	    {
	      int behind_x, behind_y;
	      int pushing_needed;

	      behind_x = new_x + (new_x - map->girl->mob->x);
	      behind_y = new_y + (new_y - map->girl->mob->y);
              if(behind_x >= 0 && behind_x < map->width && behind_y >= 0 && behind_y < map->height)
                {
                  pushing = true;
                  gamedata->push_counter++;

                  if(gamedata->traits & TRAIT_POWER_PUSH)
                    gamedata->push_counter++;

                  pushing_needed = 3;
                  if(map->girl->mob->direction == MOVE_UP)
                    pushing_needed *= 2;

                  if(map->data[behind_x + behind_y * map->width] == MAP_EMPTY)
                    if(gamedata->push_counter > pushing_needed)
                      { /* do the push */
                        if(map->fast_forwarding == false)
                          sfx_emit(SFX_BOULDER_MOVE, new_x, new_y);
                        map->data[behind_x + behind_y * map->width] = MAP_BOULDER;
                        map->move_offsets[2 * (behind_x + behind_y * map->width) + 0] = offx;
                        map->move_offsets[2 * (behind_x + behind_y * map->width) + 1] = offy;
                        map->processed[behind_x + behind_y * map->width] = 1;
                        if(gamedata->player_shift_move)
                          {
                            map->data[new_x + new_y * map->width] = MAP_EMPTY;
                            map->processed[new_x + new_y * map->width] = 1;
                          }
                        else
                          {
                            map->girl->mob->moved_over_glyph = MAP_EMPTY;
                            game_move_girl(map, new_x, new_y);
                            map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 0] = offx;
                            map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 1] = offy;
                          }
                        gamedata->push_counter = 1; /* The next push will be a bit faster. */
                      }
                }
	    }
	  break;
        case MAP_AMEBA:
          if(map->ameba_blocked == 1)
            if(gamedata->traits & TRAIT_POWER_PUSH)
              if(map->ameba_blocked_timer > 4)
                map->ameba_blocked_timer -= 3;
          break;
	}

      if(gamedata != NULL)
        if(pushing == false)
          gamedata->push_counter = 0;
      
      if(map != NULL)
	if(map->data[map->girl->mob->x + map->girl->mob->y * map->width] != MAP_EXIT_LEVEL)
	  map->data[map->girl->mob->x + map->girl->mob->y * map->width] = MAP_PLAYER;
    }

  return rv;
}
