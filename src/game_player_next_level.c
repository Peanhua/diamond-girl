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

#include "ai.h"
#include "cave.h"
#include "game.h"
#include "gfx.h"
#include "gfx_glyph.h"
#include "girl.h"
#include "map.h"
#include "quest.h"
#include "sfx.h"
#include "treasure.h"
#include "treasureinfo.h"
#include "ui.h"
#include "widget.h"
#include "widget_factory.h"
#include <libintl.h>

static struct widget * window = NULL;

static void on_quit(bool pressed, SDL_Event * event);
static void on_window_unload(struct widget * this);
static void on_play_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_save_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quit_clicked(struct widget * this, enum WIDGET_BUTTON button);


void game_player_next_level(struct gamedata * gamedata)
{
  int time_left;

  time_left = gamedata->map->game_time / gamedata->map->frames_per_second;
  if(gamedata->map->game_time % gamedata->map->frames_per_second > 0)
    time_left++;
  
  gamedata->map->game_paused = true;
  gamedata->map->fast_forwarding = false;

  if(gamedata->ai != NULL)
    if(gamedata->ai->quit == false)
      if(gamedata->ai->victory != NULL)
        gamedata->ai->victory(gamedata->ai);
  
  if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    {
      game_add_score(gamedata->map->time_score * (gamedata->map->game_time / gamedata->map->frames_per_second));
    }
  else
    {
      float pitch, pitch_step;
      int do_sfx;

      game_show_text(gettext("Level completed!"));

      pitch      = 1.0f;
      pitch_step = 0.5f / (float) (gamedata->map->game_time / gamedata->map->frames_per_second);
      do_sfx = 0;

      ui_destroy_pending_events();
      while(gamedata->map->game_time > 0)
        {
          gfx_next_frame();
          map_scroll_to_girl(gamedata->map, 24 / 6);
          gamedata->map->game_time -= gamedata->map->frames_per_second;
          game_add_score(gamedata->map->time_score);

          if(gamedata->map->game_time > 3 * gamedata->map->frames_per_second)
            pitch -= pitch_step;
          else
            {
              pitch += (1.0f - pitch) / 3.0f;
              do_sfx = 0;
            }
          if(do_sfx == 0)
            {
              sfx_pitch(pitch);
              sfx_emit(SFX_TIME, gamedata->map->girl->mob->x, gamedata->map->girl->mob->y);
              do_sfx = 1;
            }
          else
            do_sfx--;

          if(gamedata->map->game_time < 0)
            gamedata->map->game_time = 0;

          if(gamedata->map->fast_forwarding == false)
            {
              ui_draw(gamedata->need_to_clear_colorbuffer ? true : false);
              if(gamedata->need_to_clear_colorbuffer > 0)
                if(gamedata->map->background_type == 0)
                  gamedata->need_to_clear_colorbuffer--;
            }

          if(gamedata->ai != NULL)
            {
              SDL_Event e;
              
              while(SDL_PollEvent(&e))
                if(e.type == SDL_QUIT || e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN)
                  { /* Add remaining score and end the loop. */
                    game_add_score(gamedata->map->time_score * (gamedata->map->game_time / gamedata->map->frames_per_second));
                    gamedata->map->game_time = 0;
                  }
            }

          if(gamedata->playing_speed > 0)
            if(gamedata->map->fast_forwarding == false)
              SDL_framerateDelay(&gamedata->framerate_manager);
        }
    }

  ui_draw(gamedata->need_to_clear_colorbuffer ? true : false);
  if(gamedata->need_to_clear_colorbuffer > 0)
    if(gamedata->map->background_type == 0)
      gamedata->need_to_clear_colorbuffer--;

  if(gamedata->exit_after_one_level)
    {
      gamedata->quit = true;
    }
  else
    {
      if(gamedata->ai == NULL)
        if(gamedata->cave->game_mode == GAME_MODE_CLASSIC || gamedata->cave->game_mode == GAME_MODE_ADVENTURE)
          {
            char t_score[32];
            char t_score_diamonds[32];
            char t_score_time[32];
            char t_score_greedy[32];
            char t_diamond_score[32];
            char t_diamond_score_alive[32];
            char t_diamond_score_diamonds[32];
            char t_diamond_score_greedy[32];
            char t_irongirl[32];

            { /* Add some trait score and build UI strings */
              int tr_score;
              int scoremult;

              strcpy(t_score_greedy,           "-");
              strcpy(t_diamond_score_alive,    "-");
              strcpy(t_diamond_score_diamonds, "-");
              strcpy(t_diamond_score_greedy,   "-");
              strcpy(t_irongirl,               "-");
              scoremult = 1;
              
              if(gamedata->cave->editable == true)
                {
                  tr_score = 0;
                }
              else
                {
                  tr_score = gamedata->map->diamonds;
                  snprintf(t_diamond_score_diamonds, sizeof t_diamond_score_diamonds, "%d", (int) gamedata->map->diamonds);

                  if(gamedata->map->girl->mob->alive == true) /* Also intermission maps needs to be completed successfully for full score. */
                    {
                      tr_score += 10;
                      snprintf(t_diamond_score_alive, sizeof t_diamond_score_alive, "%d", 10);
                    }

                  if(gamedata->traits & TRAIT_GREEDY)
                    {
                      tr_score *= 2;
                      strcpy(t_diamond_score_greedy, "x2");
                      snprintf(t_score_greedy, sizeof t_score_greedy, "%d", gamedata->current_greedy_score);
                    }

                  /* Double the score in iron girl mode. */
                  if(gamedata->iron_girl_mode == true && (gamedata->traits & TRAIT_IRON_GIRL))
                    {
                      tr_score *= 2;
                      strcpy(t_irongirl, "x2");
                      scoremult *= 2;
                    }
                }
              
              if(tr_score > 0)
                {
                  traits_add_score(tr_score);
                  gamedata->diamond_score += tr_score;
                }

              
              snprintf(t_score_diamonds, sizeof t_score_diamonds, "%d x %d", gamedata->map->diamonds, gamedata->map->diamond_score);
              snprintf(t_score_time, sizeof t_score_time, "%d x %d", time_left, gamedata->map->time_score);
              snprintf(t_score, sizeof t_score, "%d",
                       scoremult * ( gamedata->map->diamonds * gamedata->map->diamond_score +
                                     time_left * gamedata->map->time_score +
                                     gamedata->current_greedy_score));
              snprintf(t_diamond_score, sizeof t_diamond_score, "%d", tr_score);
            }


            struct widget_factory_data wfd[] =
              {
                { WFDT_CAVE,       "cave",                   gamedata->cave           },
                { WFDT_MAP,        "map",                    gamedata->map            },
                { WFDT_STRING,     "score_total",            t_score                  },
                { WFDT_STRING,     "score_diamonds",         t_score_diamonds         },
                { WFDT_STRING,     "score_time",             t_score_time             },
                { WFDT_STRING,     "score_greedy",           t_score_greedy           },
                { WFDT_STRING,     "diamond_score",          t_diamond_score          },
                { WFDT_STRING,     "diamond_score_alive",    t_diamond_score_alive    },
                { WFDT_STRING,     "diamond_score_diamonds", t_diamond_score_diamonds },
                { WFDT_STRING,     "diamond_score_greedy",   t_diamond_score_greedy   },
                { WFDT_STRING,     "irongirl",               t_irongirl               },
                { WFDT_ON_RELEASE, "on_play_clicked",        on_play_clicked          },
                { WFDT_ON_RELEASE, "on_save_clicked",        on_save_clicked          },
                { WFDT_ON_RELEASE, "on_quit_clicked",        on_quit_clicked          },
                { WFDT_ON_UNLOAD,  "on_window_unload",       on_window_unload         },
                { WFDT_SIZEOF_,    NULL,                     NULL                     }
              };
            window = widget_factory(wfd, NULL, "game_player_next_level");
            widget_center(window);
            widget_set_gamedata_pointer(window, "gamedata", gamedata);

            {
              struct widget * w;

              w = widget_find(window, "other");
              if(w != NULL)
                if(gamedata->treasure != NULL)
                  if(gamedata->treasure->collected == true)
                    if(gamedata->treasure_placed_this_level == true)
                      {
                        widget_set_string(w, "text", gettext("Found: %s"), treasure_type_name(gamedata->treasure->item->type));
                        w = NULL;
                      }
              if(w != NULL)
                widget_delete(w);
            }
            
            ui_push_handlers();
            ui_set_navigation_handlers();
            ui_set_common_handlers();
            ui_set_handler(UIC_EXIT,   on_quit);
            ui_set_handler(UIC_CANCEL, on_quit);
            ui_destroy_pending_events();

            

            /* Some levels make certain traits available. */
            trait_t curt, levt, newt;
              
            curt = traits_get_available();
            levt = traits_level_gives(gamedata->cave, gamedata->current_level, true);
            newt = levt & (~curt);
            if(newt != 0)
              {
                traits_make_available(newt);
                widget_new_traits_available_window(newt);
              }

            /* Show the scores and bonuses stuff */
            game_end_of_level_bonuses(window);
          }

      gamedata->map->girl->mob->alive  = false;
      gamedata->map->player_movement   = NULL;
      gamedata->map->player_death      = NULL;
      gamedata->map->player_next_level = NULL;
      gamedata->map->toggle_pause      = NULL;

      if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
        {
          gamedata->pyjama_party_girls_passed++;
          game_pyjama_party_ask_next(gamedata);
        }
      
      if(gamedata->cave->game_mode != GAME_MODE_PYJAMA_PARTY && gamedata->ai != NULL)
        {
          gamedata->current_level++;
          gamedata->init_map();
        }
    }
}



static void on_quit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    on_quit_clicked(NULL, WIDGET_BUTTON_LEFT);
}

static void on_window_unload(struct widget * this DG_UNUSED)
{
  window = NULL;
  ui_pop_handlers();
}

static void on_play_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct gamedata * gamedata;

      gamedata = widget_get_gamedata_pointer(window, "gamedata");
      gamedata->current_level++;
      if(gamedata->init_map != NULL)
        gamedata->init_map();
      widget_delete(window);
    }
}

static void on_save_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct gamedata * gamedata;

      gamedata = widget_get_gamedata_pointer(window, "gamedata");
      gamedata->current_level++;
      gamedata->quit = true;
      widget_delete(window);
    }
}

static void on_quit_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct gamedata * gamedata;

      gamedata = widget_get_gamedata_pointer(window, "gamedata");
      gamedata->current_level++;
      gamedata->girls = 0;
      gamedata->quit  = true;
      widget_delete(window);
    }
}
