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

#define AI_ENGINE_NAME "diamond_hunter"

//#define AI_DIAMOND_HUNTER_DEBUG

#include "ai.h"
#include "playback.h"
#include "map.h"
#include "random.h"
#include "girl.h"

#include <assert.h>

enum PLAN
  {
    PLAN_SEEK_DIAMOND,
    PLAN_SEEK_EXIT_LEVEL,
    PLAN_EXPLODE_ENEMY
  };


/* vars */
static enum MAP_GLYPH      current_item;
static int                 fleeing;
static enum MOVE_DIRECTION fleedir;
static void *              random_state = NULL;
static enum MOVE_DIRECTION previous_thought;
static enum PLAN           current_plan;
static struct maptarget    blacklisted_target;
static int                 plan_explode_enemy_phase;
static int                 plan_explode_enemy_targets[2 * 3];

/* conf vars */
static int favor_updown;        /* 0 = no favor, -1 = up, +1 = down */
static int favor_leftright;     /* 0 = no favor, -1 = left, +1 = right */
static int random_moves;        /* percentage chance for random moves */
static int target_change_delay; /* target fixation: switch targets after most N thoughts */

/* util */
static enum MOVE_DIRECTION generate_move(struct ai * ai);
static enum MOVE_DIRECTION execute_plan_explode_enemy(struct ai * ai);
static enum MOVE_DIRECTION avoid_enemy(struct ai * ai, int xdir, int ydir);
static bool                generate_random_movement(struct ai * ai, enum MOVE_DIRECTION * direction);
static void                set_plan(enum PLAN new_plan);
static bool                find_situation(struct ai * ai, char ** situation, int width, int height, int * target);

/* AI callbacks */
static struct playback_step * get_next_move(struct ai * ai, bool game_active);
static void                   death(struct ai * ai);
static void                   victory(struct ai * ai);
static void                   diamond_collected(struct ai * ai);
static void                   cave_restart(struct ai * ai);

/* setup */
void ai_diamond_hunter(struct ai * ai)
{
  /* Initialize the random state only once.
   * We need a cleanup function and call to it in order to free this.
   */
  if(random_state == NULL)
    random_state = seed_rand_state(get_rand((1ul<<32) - 1));

  assert(random_state != NULL);

  ai_set_engine_name(ai, AI_ENGINE_NAME);
  ai->get_next_move     = get_next_move;
  ai->diamond_collected = diamond_collected;
  ai->death             = death;
  ai->victory           = victory;
  ai->cave_restart      = cave_restart;

  ai->map->player_target = NULL;
  fleeing = 0;
  fleedir = MOVE_NONE;
  previous_thought = MOVE_NONE;
  current_plan = PLAN_SEEK_DIAMOND; /* Avoid some safety checks done in set_plan(). */
  set_plan(PLAN_SEEK_DIAMOND);

  /* Configuration: */
  favor_leftright = 0;
  favor_updown = 0;
  random_moves = 0;
  target_change_delay = 1;

  float maxstrength;

  maxstrength = -2.0f;
  
  /* Increasing the number of choices where possible, increases the time to learn the optimal values. */
  for(int i = -1; i <= 1; i++)
    for(int j = -1; j <= 1; j++)
      for(int k = 0; k <= 30; k += 6)
        for(int l = 1; l <= 20; l += 2)
          {
            char address[1024];
            float strength;

            snprintf(address, sizeof address,
                     "engine=%s,favor_leftright=%d,favor_updown=%d,level=%d,map_completed,cave=%s,random_moves=%d,target_change_delay=%d",
                     AI_ENGINE_NAME,
                     i,
                     j,
                     ai->map->level,
                     ai->map->cave_name,
                     k,
                     l);
            strength = ai_brains_get(address) + 0.4f * ((float) get_rand_state(200, random_state) - 100.0f) / 100.0f;

            if(strength > maxstrength)
              {
                maxstrength = strength;
                favor_leftright     = i;
                favor_updown        = j;
                random_moves        = k;
                target_change_delay = l;
              }
          }
}


static enum MOVE_DIRECTION avoid_enemy(struct ai * ai, int xdir, int ydir)
{
  enum MOVE_DIRECTION rv;

  rv = MOVE_NONE;

  if(fleeing)
    {
      if(ai_is_passable_direction(ai->map, fleedir, true, true) == true)
        {
          fleeing--;
          rv = fleedir;
        }
#ifdef AI_DIAMOND_HUNTER_DEBUG
      else
        printf("-FLEEFAIL1");
#endif
    }
  else
    {
      int x, y;

      x = ai->map->girl->mob->x + xdir;
      y = ai->map->girl->mob->y + ydir;
      if(x >= 0 && x < ai->map->width && y >= 0 && y < ai->map->height)
        if(ai->map->data[x + y * ai->map->width] == MAP_ENEMY1)
          for(int iy = -1; rv == MOVE_NONE && iy <= 1; iy++)
            for(int ix = -1; rv == MOVE_NONE && ix <= 1; ix++)
              if((iy == 0 && ix != 0) || (iy != 0 && ix == 0))
                if(ix != xdir || iy != ydir)
                  {
                    if(ai_is_passable(ai->map, ai->map->girl->mob->x, ai->map->girl->mob->x, ix, iy, true, true, true) == true)
                      {
                        if(xdir < 0)
                          rv = MOVE_RIGHT;
                        else if(xdir > 0)
                          rv = MOVE_LEFT;
                        else if(ydir < 0)
                          rv = MOVE_DOWN;
                        else if(ydir > 0)
                          rv = MOVE_UP;
                      }
#ifdef AI_DIAMOND_HUNTER_DEBUG
                    else
                      printf("-FLEEFAIL2");
#endif
                  }

      if(rv != MOVE_NONE)
        {
          fleeing = ai->map->game_speed;
          fleedir = rv;
        }
    }

  return rv;
}



static enum MOVE_DIRECTION generate_move(struct ai * ai)
{ /* Figure out next direction of movement. */
  enum MOVE_DIRECTION rv;
  uint8_t possible_moves;

#ifdef AI_DIAMOND_HUNTER_DEBUG
  printf("%s:", __FUNCTION__);
#endif

  if(blacklisted_target.x != -1)
    {
      blacklisted_target.age++;
      if(blacklisted_target.age >= 5)
        blacklisted_target.x = -1;
    }

  possible_moves = ai_get_possible_moves(ai);
  rv = MOVE_NONE;

  if(possible_moves)
    {
      /* Avoid enemies */
      if(rv == MOVE_NONE)
        rv = avoid_enemy(ai, -1,  0);
      if(rv == MOVE_NONE)
        rv = avoid_enemy(ai,  1,  0);
      if(rv == MOVE_NONE)
        rv = avoid_enemy(ai,  0, -1);
      if(rv == MOVE_NONE)
        rv = avoid_enemy(ai,  0,  1);

#ifdef AI_DIAMOND_HUNTER_DEBUG
      if(rv != MOVE_NONE)
        printf("-flee");
#endif
      
      if(rv == MOVE_NONE)
        if(current_plan != PLAN_EXPLODE_ENEMY) /* No random movements during this plan. */
          if((int) get_rand_state(100, random_state) < random_moves)
            generate_random_movement(ai, &rv); /* Note that we ignore MOVE_NONE from random generator */

      if(rv == MOVE_NONE)
        { /* Move towards target, which is the "closest" of type current_item.
           * First determine if we need to obtain a new target.
           */
          bool obtain_new_target;
          
          obtain_new_target = false;

          if(ai->map->player_target != NULL)
            {
              ai->map->player_target->age++;
              
              if(ai->map->data[ai->map->player_target->x + ai->map->player_target->y * ai->map->width] != current_item)
                {
                  obtain_new_target = true; /* The target is no longer present */
                  ai->map->player_target = maptarget_free(ai->map->player_target);
                }

              if(current_plan == PLAN_SEEK_DIAMOND)
                if(obtain_new_target == false)
                  if(ai->map->player_target->age >= target_change_delay)
                    {
                      obtain_new_target = true; /* The target is old enough */
                      if(ai->map->player_target->age >= target_change_delay + ai->map->player_target->original_distance * 5)
                        { /* The target is 5 times as old as it should take us from reaching there.
                           * Try something different next time.
                           */
                          blacklisted_target.x   = ai->map->player_target->x;
                          blacklisted_target.y   = ai->map->player_target->y;
                          blacklisted_target.age = 0;
                        }
                    }
            }
          else
            obtain_new_target = true;
          
          
          if(obtain_new_target == true)
            { /* Obtain new target */
              struct maptarget * t;
              
              t = ai_get_next_target(ai->map, current_item, &blacklisted_target, favor_updown, favor_leftright);
              if(t != NULL)
                {
#ifdef AI_DIAMOND_HUNTER_DEBUG
                  printf("-newtarget[%d,%d]", t->x, t->y);
#endif
                  if(ai->map->player_target == NULL || (t->x != ai->map->player_target->x || t->y != ai->map->player_target->y) )
                    { /* Don't replace unless changed, so that the age reflects the real age of this particular target. */
                      maptarget_free(ai->map->player_target);
                      ai->map->player_target = t;
                    }
                  if(current_plan == PLAN_EXPLODE_ENEMY) /* Don't switch from PLAN_SEEK_LEVEL_EXIT ... */
                    set_plan(PLAN_SEEK_DIAMOND);
                }
            }

          if(ai->map->player_target == NULL)
            {
#ifdef AI_DIAMOND_HUNTER_DEBUG
              printf("-NOTARGET");
#endif
              if(ai->map->girl->mob->armour > 0)
                {
                  if(current_plan != PLAN_EXPLODE_ENEMY)
                    {
                      char * needle1[3] =
                        {
                          "  B", /* B = boulder or diamond */
                          "210", /* 1 = target1, 2 = target2, 3 = target3 */
                          "  E", /* E = enemy to be exploded into diamonds, must be trapped */
                        };
                      char * needle2[3] =
                        {
                          "B  ", /* B = boulder or diamond */
                          "012", /* 1 = target1, 2 = target2, 3 = target3 */
                          "E  ", /* E = enemy to be exploded into diamonds, must be trapped */
                        };

                      if(find_situation(ai, needle1, 3, 3, plan_explode_enemy_targets) == true ||
                         find_situation(ai, needle2, 3, 3, plan_explode_enemy_targets) == true)
                        {
#ifdef AI_DIAMOND_HUNTER_DEBUG
                          printf("-PLAN_EXPLODE_ENEMY");
#endif
                          set_plan(PLAN_EXPLODE_ENEMY);
                        }
#ifdef AI_DIAMOND_HUNTER_DEBUG
                      else
                        printf("-explode_enemy_situation_not_found");
#endif
                    }
                  else
                    {
#ifdef AI_DIAMOND_HUNTER_DEBUG
                      printf("-EXEC_PLAN_EXPLODE_ENEMY");
#endif
                      rv = execute_plan_explode_enemy(ai);
                    }
                }
            }

          
          if(ai->map->player_target != NULL)
            { /* Move towards the target */
              struct mappath * p;
              
#ifdef AI_DIAMOND_HUNTER_DEBUG
              printf("-diamond");
#endif
              if(ai->map->player_target->path == NULL)
                ai->map->player_target->path = map_find_path(ai->map,
                                                             ai->map->girl->mob->x,         ai->map->girl->mob->y,
                                                             ai->map->player_target->x, ai->map->player_target->y,
                                                             ai_is_passable_mfp_cb);
              //assert(ai->map->player_target->path != NULL); /* this does happen */
              if(ai->map->player_target->path != NULL)
                {
                  int pathlen;

                  pathlen = 0;
                  p = ai->map->player_target->path;
                  while(p->parent != NULL && p->parent->parent != NULL)
                    {
                      p = p->parent;
                      pathlen++;
                    }
                  
                  if(ai->map->girl->mob->x < p->x)
                    rv = MOVE_RIGHT;
                  else if(ai->map->girl->mob->x > p->x)
                    rv = MOVE_LEFT;
                  else if(ai->map->girl->mob->y < p->y)
                    rv = MOVE_DOWN;
                  else /*if(ai->map->girl->mob->y > p->y)*/
                    rv = MOVE_UP;
                  
                  ai->map->player_target->path = map_free_path(ai->map->player_target->path);
                }
              else
                { /* Path finding fails, time for plan B to reach our target.
                   * If we're hunting for diamonds, we blacklist this one for a moment, and try a different one.
                   */
                  if(current_plan == PLAN_SEEK_DIAMOND)
                    {
                      blacklisted_target.x   = ai->map->player_target->x;
                      blacklisted_target.y   = ai->map->player_target->x;
                      blacklisted_target.age = 0;
                    }
#ifdef AI_DIAMOND_HUNTER_DEBUG
                  printf("-NOPATH");
#endif
                }
            }
        }


      if(rv == MOVE_NONE)
        if(ai->map->girl->mob->y > 0)
          { /* Avoid falling objects above head */
            enum MAP_GLYPH above;
            
            above = ai->map->data[ai->map->girl->mob->x + (ai->map->girl->mob->y - 1) * ai->map->width];
            if(above == MAP_DIAMOND_FALLING || above == MAP_BOULDER_FALLING)
              {
                bool okdirs[2];
                
                okdirs[0] = okdirs[1] = false;
                if(possible_moves & (1 << MOVE_LEFT)  && ai_is_passable_direction(ai->map, MOVE_LEFT,  false, true))
                  okdirs[0] = true;
                if(possible_moves & (1 << MOVE_RIGHT) && ai_is_passable_direction(ai->map, MOVE_RIGHT, false, true))
                  okdirs[1] = true;

                if(okdirs[0] == true && okdirs[1] == true)
                  { /* both possible, pick a random one */
                    if(get_rand_state(2, random_state) == 0)
                      rv = MOVE_LEFT;
                    else
                      rv = MOVE_RIGHT;
                  }
                else if(okdirs[0] == true)
                  rv = MOVE_LEFT;
                else if(okdirs[1] == true)
                  rv = MOVE_RIGHT;

#ifdef AI_DIAMOND_HUNTER_DEBUG
                if(rv != MOVE_NONE)
                  printf("-head");
#endif
              }
          }
      
      if(rv == MOVE_NONE)
        if(ai->map->girl->mob->armour > 0)
          { /* Try one more time, this time don't be so cautious. */
            enum MOVE_DIRECTION alldirs[4] = { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
            enum MOVE_DIRECTION dirs[4];
            int count;

            count = 0;
            for(int i = 0; rv == MOVE_NONE && i < 4; i++)
              if(ai_is_passable_direction(ai->map, alldirs[i], false, false))
                dirs[count++] = alldirs[i];
            
            if(count > 0)
              {
                int n;

                n = get_rand_state(count, random_state);
                rv = dirs[n];
#ifdef AI_DIAMOND_HUNTER_DEBUG
                printf("-all");
#endif
              }
          }
}

#ifdef AI_DIAMOND_HUNTER_DEBUG
  if(rv == MOVE_LEFT && ai->map->data[ai->map->girl->mob->x - 1 + ai->map->girl->mob->y * ai->map->width] == MAP_BOULDER)
    printf("-pushingboulderleft");
  if(rv == MOVE_RIGHT && ai->map->data[ai->map->girl->mob->x + 1 + ai->map->girl->mob->y * ai->map->width] == MAP_BOULDER)
    printf("-pushingboulderright");

  printf(":%d\n", rv);
#endif

  return rv;
}

static enum MOVE_DIRECTION execute_plan_explode_enemy(struct ai * ai)
{
  enum MOVE_DIRECTION rv;

#ifdef AI_DIAMOND_HUNTER_DEBUG
  printf("%s: phase=%d", __FUNCTION__, plan_explode_enemy_phase);
#endif
  rv = MOVE_NONE;

  if(plan_explode_enemy_phase < 3)
    {
      int planx, plany;

      planx = plan_explode_enemy_targets[plan_explode_enemy_phase * 2 + 0];
      plany = plan_explode_enemy_targets[plan_explode_enemy_phase * 2 + 1];

      if(ai->map->girl->mob->x != planx || ai->map->girl->mob->y != plany)
        {
          struct mappath * path;

#ifdef AI_DIAMOND_HUNTER_DEBUG
          printf("-goto(%d,%d)", planx, plany);
#endif
          path = map_find_path(ai->map,
                               ai->map->girl->mob->x, ai->map->girl->mob->y,
                               planx,             plany,
                               ai_is_passable_mfp_cb);
          if(path == NULL)
            path = map_find_path(ai->map,
                                 ai->map->girl->mob->x, ai->map->girl->mob->y,
                                 planx,             plany,
                                 ai_is_passable_mfp_cb_cautiousoff);
          if(path != NULL)
            {
              struct mappath * p;
              
              p = path;
              while(p->parent != NULL && p->parent->parent != NULL)
                p = p->parent;
                  
              if(ai->map->girl->mob->x < p->x)
                rv = MOVE_RIGHT;
              else if(ai->map->girl->mob->x > p->x)
                rv = MOVE_LEFT;
              else if(ai->map->girl->mob->y < p->y)
                rv = MOVE_DOWN;
              else /*if(ai->map->girl->mob->y > p->y)*/
                rv = MOVE_UP;

#ifdef AI_DIAMOND_HUNTER_DEBUG
              printf("-path_ok");
#endif

              if(ai->map->player_target != NULL)
                {
                  ai->map->player_target->x = planx;
                  ai->map->player_target->y = plany;
                }
            }
          else /* Bail out from this plan. */
            {
#ifdef AI_DIAMOND_HUNTER_DEBUG
              printf("-NOPATH");
#endif
              set_plan(PLAN_SEEK_DIAMOND);
            }
        }
      else
        { /* Current phase completed, proceed to next. */
          plan_explode_enemy_phase++;
#ifdef AI_DIAMOND_HUNTER_DEBUG
          printf("-NEXTPHASE(%d)", (int) plan_explode_enemy_phase);
#endif
          rv = execute_plan_explode_enemy(ai);
        }
    }
  else /* This plan has been fully executed. */
    {
#ifdef AI_DIAMOND_HUNTER_DEBUG
      printf("-COMPLETED");
#endif
      set_plan(PLAN_SEEK_DIAMOND);
    }

#ifdef AI_DIAMOND_HUNTER_DEBUG
  printf(":%d:", (int) rv);
#endif

  return rv;
}

static struct playback_step * get_next_move(struct ai * ai, bool game_active)
{
  static struct playback_step step;
  
  playback_step_initialize(&step);

  if(game_active == true)
    {
      if(ai->map->game_timer == 0)
        { /* rethink */
#ifdef AI_DIAMOND_HUNTER_DEBUG
          printf("%s:current_plan=%d > ", __FUNCTION__, (int) current_plan);
#endif
          switch(current_plan)
            {
            case PLAN_SEEK_DIAMOND:
              step.direction = generate_move(ai);
              previous_thought = step.direction;
              break;
            case PLAN_SEEK_EXIT_LEVEL:
              step.direction = generate_move(ai);
              previous_thought = step.direction;
              break;
            case PLAN_EXPLODE_ENEMY:
              step.direction = generate_move(ai);
              previous_thought = step.direction;
              break;
            }
        }
      else
        step.direction = previous_thought;
    }

  return &step;
}


static void set_plan(enum PLAN new_plan)
{
#ifdef AI_DIAMOND_HUNTER_DEBUG
  printf("\nNEW PLAN %d\n", (int) new_plan);
#endif

  /* Abort on illegal situation: switch to seek diamond in the midst of seeking level exit */
  assert(current_plan != PLAN_SEEK_EXIT_LEVEL || new_plan != PLAN_SEEK_DIAMOND);
  
  current_plan = new_plan;
  switch(new_plan)
    {
    case PLAN_SEEK_DIAMOND:
      current_item = MAP_DIAMOND;
      break;
    case PLAN_SEEK_EXIT_LEVEL:
      current_item = MAP_EXIT_LEVEL;
      break;
    case PLAN_EXPLODE_ENEMY:
      plan_explode_enemy_phase = 0;
      break;
    }
  blacklisted_target.x = -1;
  blacklisted_target.y = -1;
}


static void score_conf(struct ai * ai, bool obtained_victory)
{
  float score, tmp;

  if(obtained_victory == true)
    { /* Victory:       +0.7 */
      /* Time left: max +0.3 */
      score = 0.7f;

      tmp = (float) ai->map->game_time / (float) (ai->map->time * ai->map->frames_per_second);
      assert(tmp <= 1.0f);
      score += tmp * 0.3f;
    }
  else
    { /* No victory:   -1.0 */
      /* Diamonds: max +0.4 */
      score = -1.0f;

      tmp = (float) ai->map->diamonds / (float) ai->map->diamonds_needed;
      if(tmp > 1.0f)
        tmp = 1.0f;
      score += tmp * 0.4f;
    }
  
  char address[1024];

  snprintf(address, sizeof address,
           "engine=%s,favor_leftright=%d,favor_updown=%d,level=%d,map_completed,cave=%s,random_moves=%d,target_change_delay=%d",
           AI_ENGINE_NAME,
           favor_leftright,
           favor_updown,
           ai->map->level,
           ai->map->cave_name,
           random_moves,
           target_change_delay);
  ai_brains_add(address, score);

  snprintf(address, sizeof address, "engine=%s,level=%d,map_completed,cave=%s", AI_ENGINE_NAME, ai->map->level, ai->map->cave_name);
  if(obtained_victory == true)
    ai_brains_add(address,  1.0f);
  else
    ai_brains_add(address, -1.0f);

}

static void death(struct ai * ai)
{ /* Memorize the used values with the knowledge of failure. */
  score_conf(ai, false);
  ai->map->player_target = maptarget_free(ai->map->player_target);
}


static void victory(struct ai * ai)
{ /* Memorize the used values with the knowledge of success. */
  score_conf(ai, true);
  ai->map->player_target = maptarget_free(ai->map->player_target);
}









static void diamond_collected(struct ai * ai)
{
  if(ai->map->player_target != NULL)
    if(ai->map->data[ai->map->player_target->x + ai->map->player_target->y * ai->map->width] != current_item)
      ai->map->player_target = maptarget_free(ai->map->player_target);

  if(current_plan == PLAN_SEEK_DIAMOND && ai->map->exit_open == true)
    set_plan(PLAN_SEEK_EXIT_LEVEL);
}


static bool generate_random_movement(struct ai * ai, enum MOVE_DIRECTION * direction)
{ /* Only when this returns true, the direction is written over. */
  enum MOVE_DIRECTION alldirs[5] = { MOVE_NONE, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN };
  enum MOVE_DIRECTION d;
  int xmod, ymod;
  bool rv;
  
  rv = false;

  d = alldirs[get_rand_state(5, random_state)];
  switch(d)
    {
    default:
    case MOVE_NONE:  xmod =  0; ymod =  0; break;
    case MOVE_UP:    xmod =  0; ymod = -1; break;
    case MOVE_LEFT:  xmod = -1; ymod =  0; break;
    case MOVE_RIGHT: xmod =  1; ymod =  0; break;
    case MOVE_DOWN:  xmod =  0; ymod =  1; break;
    }
          
  int x, y;
          
  x = ai->map->girl->mob->x + xmod;
  y = ai->map->girl->mob->y + ymod;
  if(x >= 0 && x < ai->map->width && y >= 0 && y < ai->map->height)
    if(d == MOVE_NONE || ai_is_passable(ai->map, ai->map->girl->mob->x, ai->map->girl->mob->y, xmod, ymod, true, false, true))
      {
#ifdef AI_DIAMOND_HUNTER_DEBUG
        printf("-random");
#endif
        rv = true;
        *direction = d;
      }

  return rv;
}


static void cave_restart(struct ai * ai)
{
  ai->quit = true;
}



static bool find_situation(struct ai * ai, char ** situation, int width, int height, int * target)
{
  bool found;
  int mindist;
  int tmptarget[2 * 10];
  int targetcount;

  found = false;
  mindist = -1;
  for(int i = 0; i < 2 * 10; i++)
    tmptarget[i] = -1;
  targetcount = 0;

  for(int y = 0; found == false && y < ai->map->height - 3; y++)
    for(int x = 0; found == false && x < ai->map->width; x++)
      {
        bool match;

        match = true;
        for(int ny = 0; match == true && ny < height; ny++)
          for(int nx = 0; match == true && nx < width; nx++)
            if(ny + y >= 0 && ny + y < ai->map->height && nx + x >= 0 && nx + x < ai->map->width)
              {
                enum MAP_GLYPH glyph;
                
                match = false;
                glyph = ai->map->data[nx + x + (ny + y) * ai->map->width];
                switch(situation[ny][nx])
                  {
                  case 'B': /* boulder or diamond */
                    if(glyph == MAP_BOULDER || glyph == MAP_DIAMOND)
                      match = true;
                    break;
                  case '0': /* target0, girl must be able to get there */
                  case '1': /* target1, girl must be able to get there */
                  case '2': /* target2, girl must be able to get there */
                  case '3': /* target3, girl must be able to get there */
                    {
                      struct mappath * path;
                      
                      path = map_find_path(ai->map,
                                           ai->map->girl->mob->x, ai->map->girl->mob->y,
                                           nx + x,            ny + y,
                                           ai_is_passable_mfp_cb_cautiousoff);
                      if(path != NULL)
                        {
                          int target_index;

                          match = true;
                          target_index = situation[ny][nx] - '0';
                          tmptarget[2 * target_index + 0] = nx + x;
                          tmptarget[2 * target_index + 1] = ny + y;
                          if(target_index + 1 > targetcount)
                            targetcount = target_index + 1;
                        }
                    }
                    break;
                  case 'E': /* enemy2, must be trapped (not yet implemented) */
                    if(glyph == MAP_ENEMY2)
                      match = true;
                    break;
                  default:
                    match = true;
                    break;
                  }

              }
            else
              match = false;

        if(match == true)
          {
            int dist;
            
            dist = abs(ai->map->girl->mob->x - x) + abs(ai->map->girl->mob->y - y);
            if(mindist == -1 || dist < mindist)
              {
                mindist = dist;
                found = true;
                for(int i = 0; i < targetcount; i++)
                  {
                    target[i * 2 + 0] = tmptarget[i * 2 + 0];
                    target[i * 2 + 1] = tmptarget[i * 2 + 1];
                  }
              }
          }
      }

  return found;
}
