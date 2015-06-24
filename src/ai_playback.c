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

#define AI_ENGINE_NAME "playback"

#include "ai.h"
#include "playback.h"
#include "map.h"
#include "highscore.h"
#include "random.h"
#include "cave.h"

#include <assert.h>

/* conf vars */
static int playback_highscore;
static int playback_index;


static struct playback_step * get_next_move(struct ai * ai, bool game_active);
static void                   death(struct ai * ai);
static void                   victory(struct ai * ai);
static void                   cave_restart(struct ai * ai);

#ifdef DEBUG_PLAYBACK
static void check_step(struct ai * ai, struct playback_step * step);
#endif

bool ai_playback(struct ai * ai, enum GAME_MODE game_mode, struct playback * playback, char * playback_title)
{
  ai_set_engine_name(ai, AI_ENGINE_NAME);
  ai->playback_title = playback_title;

  playback_highscore = -1;
  playback_index = -1;

  if(playback == NULL)
    {
      struct highscore_entry ** highscores;
      size_t                    highscores_size;
      struct cave * cave;

      cave = cave_get(game_mode, ai->map->cave_name);
      highscores = highscores_get(cave->highscores, &highscores_size);
      if(highscores_size > 0)
        {
          float strength, maxstrength;
          
          maxstrength = -2.0f;
          for(size_t i = 0; i < highscores_size; i++)
            {
              struct playback * pb;
              char * pberr;

              pb = highscore_playback(highscores[i], game_mode);
              pberr = playback_check(pb);
              if(pberr == NULL)
                {
                  int pbind;
                  
                  pbind = 0;
                  while(pb != NULL)
                    {
                      if(pb->not_for_ai == false)
                        if(!strcmp(pb->cave, ai->map->cave_name) && pb->level == ai->map->level)
                          {
                            char address[1024];
                        
                            snprintf(address, sizeof address,
                                     "engine=%s,highscore=%d,index=%d,level=%d,map_completed,cave=%s",
                                     AI_ENGINE_NAME,
                                     (int) i,
                                     pbind,
                                     ai->map->level,
                                     ai->map->cave_name);
                            strength = ai_brains_get(address) + 0.4f * ((float) get_rand(200) - 100.0f) / 100.0f;
                        
                            if(strength > maxstrength)
                              {
                                maxstrength = strength;
                                playback           = pb;
                                playback_highscore = i;
                                playback_index     = pbind;
                              }
                          }
                  
                      pb = pb->next;
                      pbind++;
                    }
                }
            }
        }
    }

  bool rv;

  if(playback != NULL)
    {
      ai->playback          = playback;
      ai->playback->current_step = 0;
      ai->get_next_move     = get_next_move;
      ai->diamond_collected = NULL;
      ai->death             = death;
      ai->victory           = victory;
      ai->cave_restart      = cave_restart;
      rv = true;
    }
  else
    rv = false;

  return rv;
}

#ifdef DEBUG_PLAYBACK
static void check_step(struct ai * ai, struct playback_step * step)
{
  if(step->timestamp       != ai->map->game_time  ||
     step->timestamp_timer != ai->map->game_timer ||
     step->position[0]     != ai->map->player_x   || 
     step->position[1]     != ai->map->player_y)
    {
      printf("Step check FAIL:\n");
      printf(" current_step    = %u\n",
             (unsigned int) ai->playback->current_step);
      printf(" timestamp       = %d, map says = %d: %s\n",
             (int) step->timestamp,
             (int) ai->map->game_time,
             step->timestamp       == ai->map->game_time  ? "ok" : "mismatch");
      printf(" timestamp_timer = %d, map says = %d: %s\n",
             (int) step->timestamp_timer,
             (int) ai->map->game_timer,
             step->timestamp_timer == ai->map->game_timer ? "ok" : "mismatch");
      printf(" position[0]     = %d, map says = %d: %s\n",
             (int) step->position[0],
             (int) ai->map->player_x,
             step->position[0]     == ai->map->player_x   ? "ok" : "mismatch");
      printf(" position[1]     = %d, map says = %d: %s\n",
             (int) step->position[1],
             (int) ai->map->player_y,
             step->position[1]     == ai->map->player_y   ? "ok" : "mismatch");
    }
  assert(step->timestamp       == ai->map->game_time);
  assert(step->timestamp_timer == ai->map->game_timer);
  assert(step->position[0]     == ai->map->player_x);
  assert(step->position[1]     == ai->map->player_y);
}
#endif

static struct playback_step * get_next_move(struct ai * ai, bool game_active DG_UNUSED)
{
  struct playback_step * step;
  
  assert(ai->playback != NULL);

#ifdef DEBUG_PLAYBACK
  printf("%s(): playback=%p, current_step = %d\n", __FUNCTION__, ai->playback, ai->playback->current_step);
#endif
  step = playback_next_step(ai->playback);
  if(step != NULL)
    {
#ifdef DEBUG_PLAYBACK
      check_step(ai, step);
#endif
    }
  else
    {
#ifdef DEBUG_PLAYBACK
      printf("%s:%s(): End-Of-Steps reached\n", __FILE__, __FUNCTION__);
#endif
      ai->playback = ai->playback->next;
      if(ai->playback != NULL)
        {
          ai->playback->current_step = 0;
#ifdef DEBUG_PLAYBACK
          printf("%s:%s(): New map: %s level %d\n", __FILE__, __FUNCTION__, ai->playback->cave, ai->playback->level);
#endif

          step = playback_next_step(ai->playback);
          if(step != NULL)
            {
#ifdef DEBUG_PLAYBACK
              check_step(ai, step);
#endif
            }
        }
      else
        {
          ai->quit = true;
        }
    }

  if(step == NULL)
    {
      static struct playback_step s;

      s.direction = MOVE_NONE;
      s.manipulate = false;
      step = &s;
    }

#ifdef DEBUG_PLAYBACK
  printf("%s(): return %d:%d\n", __FUNCTION__, (int) step->direction, (int) step->manipulate);
#endif

  return step;
}


static void death(struct ai * ai)
{
  if(playback_highscore >= 0)
    {
      char address[1024];
  
      snprintf(address, sizeof address,
               "engine=%s,highscore=%d,index=%d,level=%d,map_completed,cave=%s",
               AI_ENGINE_NAME,
               playback_highscore,
               playback_index,
               ai->map->level,
               ai->map->cave_name);
      ai_brains_add(address, -1.0f);

      snprintf(address, sizeof address,
               "engine=%s,level=%d,map_completed,cave=%s",
               AI_ENGINE_NAME,
               ai->map->level,
               ai->map->cave_name);
      ai_brains_add(address, -1.0f);
    }
}


static void victory(struct ai * ai)
{
  if(playback_highscore >= 0)
    {
      char address[1024];
  
      snprintf(address, sizeof address,
               "engine=%s,highscore=%d,index=%d,level=%d,map_completed,cave=%s",
               AI_ENGINE_NAME,
               playback_highscore,
               playback_index,
               ai->map->level,
               ai->map->cave_name);
      ai_brains_add(address, 1.0f);

      snprintf(address, sizeof address,
               "engine=%s,level=%d,map_completed,cave=%s",
               AI_ENGINE_NAME,
               ai->map->level,
               ai->map->cave_name);
      ai_brains_add(address, 1.0f);
    }
}


static void cave_restart(struct ai * ai)
{
  if(ai->use_planning == true)
    ai->quit = true; /* Quit if this was AI playing and not a playback playing for the user. */
}
