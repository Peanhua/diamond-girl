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

#define AI_ENGINE_NAME "pyjama_party"

#include "ai.h"
#include "map.h"
#include "random.h"
#include "playback.h"
#include "girl.h"

#include <assert.h>


static struct playback_step * get_next_move(struct ai * ai, bool game_active);
static void                   diamond_collected(struct ai * ai);
static void                   death(struct ai * ai);
static void                   victory(struct ai * ai);
static void                   cave_restart(struct ai * ai);
static struct maptarget * new_target(struct ai * ai, int x, int y);
static struct playback_step * ai_move_towards_target(struct ai * ai);

#ifdef DEBUG_PLAYBACK
static void check_step(struct ai * ai, struct playback_step * step);
#endif

void ai_pyjama_party(struct ai * ai)
{
  ai_set_engine_name(ai, AI_ENGINE_NAME);
  ai->use_planning      = false;
  ai->get_next_move     = get_next_move;
  ai->diamond_collected = diamond_collected;
  ai->death             = death;
  ai->victory           = victory;
  ai->cave_restart      = cave_restart;
}


static struct playback_step * get_next_move(struct ai * ai, bool game_active)
{
  struct playback_step * step;

  if(game_active)
    {
      if(ai->map->player_target == NULL)
        {
          /* Greedy girls try to pick diamonds first. */
          if(ai->map->girl->traits & TRAIT_GREEDY)
            ai->map->player_target = ai_get_next_target(ai->map, MAP_DIAMOND, NULL, 0, 0);

          /* Move to the exit. */
          if(ai->map->player_target == NULL)
            if(ai->map->exit_open)
              ai->map->player_target = new_target(ai, ai->map->exit_x, ai->map->exit_y);

          /* Find the closest accessible diamond. */
          if(ai->map->player_target == NULL)
            ai->map->player_target = ai_get_next_target(ai->map, MAP_DIAMOND, NULL, 0, 0);
        }
      else
        { /* Clear target if the girl has reached it's destination. */
          if(ai->map->player_target->x == ai->map->girl->mob->x && ai->map->player_target->y == ai->map->girl->mob->y)
            ai->map->player_target = maptarget_free(ai->map->player_target);
        }
    }

  

  step = ai_move_towards_target(ai);
  if(step == NULL)
    {
      step = malloc(sizeof(struct playback_step));
      assert(step != NULL);
      step->direction      = MOVE_NONE;
      step->manipulate     = false;
      step->commit_suicide = false;
    }

  return step;
}


static struct maptarget * new_target(struct ai * ai DG_UNUSED, int x, int y)
{
  struct maptarget * target;
  
  target = malloc(sizeof(struct maptarget));
  assert(target != NULL);
  if(target != NULL)
    {
      struct mappath * path;
      int pathlen;
      
      path = NULL;
      pathlen = 0;
      if(path != NULL)
        {
          struct mappath * p;

          p = path;
          while(p->parent != NULL && p->parent->parent != NULL)
            {
              p = p->parent;
              pathlen++;
            }
        }
      target->x = x;
      target->y = y;
      target->original_distance = pathlen;
      target->age  = 0;
      target->path = path;
    }
  
  return target;
}


static struct playback_step * ai_move_towards_target(struct ai * ai)
{ /* Move towards the target */
  struct playback_step * step;

  step = NULL;
  if(ai->map->player_target != NULL)
    {
      if(ai->map->player_target->path == NULL)
        ai->map->player_target->path = map_find_path(ai->map,
                                                     ai->map->girl->mob->x,     ai->map->girl->mob->y,
                                                     ai->map->player_target->x, ai->map->player_target->y,
                                                     ai_is_passable_mfp_cb);
      if(ai->map->player_target->path != NULL)
        {
          int pathlen;
          struct mappath * p;

          pathlen = 0;
          p = ai->map->player_target->path;
          while(p->parent != NULL && p->parent->parent != NULL)
            {
              p = p->parent;
              pathlen++;
            }
              
          step = malloc(sizeof(struct playback_step));
          assert(step != NULL);
          step->direction      = MOVE_NONE;
          step->manipulate     = false;
          step->commit_suicide = false;

          if(ai->map->girl->mob->x < p->x)
            step->direction = MOVE_RIGHT;
          else if(ai->map->girl->mob->x > p->x)
            step->direction = MOVE_LEFT;
          else if(ai->map->girl->mob->y < p->y)
            step->direction = MOVE_DOWN;
          else /*if(ai->map->girl->mob->y > p->y)*/
            step->direction = MOVE_UP;

          ai->map->player_target->path = map_free_path(ai->map->player_target->path);
        }
    }

  return step;
}


static void diamond_collected(struct ai * ai DG_UNUSED)
{
}

static void death(struct ai * ai DG_UNUSED)
{
}


static void victory(struct ai * ai DG_UNUSED)
{
}


static void cave_restart(struct ai * ai DG_UNUSED)
{
}
