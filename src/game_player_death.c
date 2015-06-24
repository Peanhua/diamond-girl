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

#include "ai.h"
#include "cave.h"
#include "game.h"
#include "gfx.h"
#include "girl.h"
#include "globals.h"
#include "map.h"
#include "random.h"
#include "sfx.h"
#include "traits.h"
#include "widget.h"
#include "widget_factory.h"
#include <assert.h>
#include <libintl.h>

static struct widget * window = NULL;

static void on_quit(bool pressed, SDL_Event * event);
static void on_window_unload(struct widget * this);
static void on_play_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_save_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quit_clicked(struct widget * this, enum WIDGET_BUTTON button);


void game_player_death(struct gamedata * gamedata, bool sounds)
{
  if(gamedata->map->level_start_anim_on)
    map_level_start_animation_stop(gamedata->map);

  if(gamedata->cave->game_mode == GAME_MODE_CLASSIC || gamedata->map->girl->mob->armour == 0)
    {
      gamedata->girl_death_delay_timer = gamedata->map->frames_per_second * 3;
      if(sounds)
        {
          sfx_pitch(1.0f);
          sfx_emit(SFX_EXPLOSION, gamedata->map->girl->mob->x, gamedata->map->girl->mob->y);
          sfx_emit(SFX_EXPLOSION, gamedata->map->girl->mob->x, gamedata->map->girl->mob->y);
        }

      gamedata->map->flash_screen     = true;
      gamedata->map->girl->mob->alive = false;
      gamedata->map->fast_forwarding  = false;
      if(globals.opengl)
        {
          gamedata->map->display_colour[0] = 0xff;
          gamedata->map->display_colour[1] = 0xff;
          gamedata->map->display_colour[2] = 0xff;
          gamedata->map->display_colour[3] = 0xff;
        }

      if(gamedata->map->is_intermission == false)
        {
          gamedata->girls--;

          if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
            {
              if(gamedata->pyjama_party_girl != NULL)
                gamedata->pyjama_party_girl->mob->alive = false;
              game_set_pyjama_party_control(PARTYCONTROLLER_PLAYER);
            }

          if(gamedata->traits & TRAIT_DYNAMITE)
            {
              for(int y = -4; y <= 4; y++)
                for(int x = -4; x <= 4; x++)
                  {
                    enum MAP_GLYPH g;
                    int d;
                  
                    d = abs(x) + abs(y);
                    if(d < 5)
                      g = MAP_DIAMOND;
                    else if(d < 7)
                      g = MAP_EMPTY;
                    else
                      g = MAP_ILLEGAL;

                    if(g != MAP_ILLEGAL)
                      if(x + gamedata->map->girl->mob->x >= 0 && x + gamedata->map->girl->mob->x < gamedata->map->width &&
                         y + gamedata->map->girl->mob->y >= 0 && y + gamedata->map->girl->mob->y < gamedata->map->height)
                        gamedata->map->data[x + gamedata->map->girl->mob->x + (y + gamedata->map->girl->mob->y) * gamedata->map->width] = g;
                  }
            }
          else
            {
              for(int y = -1; y <= 1; y++)
                for(int x = -1; x <= 1; x++)
                  if(x + gamedata->map->girl->mob->x >= 0 && x + gamedata->map->girl->mob->x < gamedata->map->width &&
                     y + gamedata->map->girl->mob->y >= 0 && y + gamedata->map->girl->mob->y < gamedata->map->height)
                    gamedata->map->data[x + gamedata->map->girl->mob->x + (y + gamedata->map->girl->mob->y) * gamedata->map->width] = MAP_DIAMOND;
            }

          if(gamedata->ai != NULL)
            if(gamedata->ai->quit == false)
              if(gamedata->ai->death != NULL)
                gamedata->ai->death(gamedata->ai);

          int score;

          score = 0;
          if(!gamedata->exit_after_one_level)
            if(gamedata->ai == NULL)
              if(gamedata->cave->game_mode == GAME_MODE_CLASSIC || gamedata->cave->game_mode == GAME_MODE_ADVENTURE)
                { /* Add some trait score */
                  score = gamedata->map->diamonds / 10;
                  if(gamedata->traits & TRAIT_GREEDY)
                    score *= 2;

                  /* Double the score in iron girl mode. */
                  if(gamedata->iron_girl_mode == true && (gamedata->traits & TRAIT_IRON_GIRL))
                    score *= 2;

                  if(score > 0)
                    {
                      traits_add_score(score);
                      gamedata->diamond_score += score;
                    }
                }
      
          char msg[256];

          if(gamedata->girls > 0)
            {
              if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
                game_pyjama_party_ask_next(gamedata);
              else if(gamedata->exit_after_one_level == true)
                {
                  gamedata->quit = true;
                }
              else if(gamedata->ai == NULL)
                {
                  char scorestring[128];

                  snprintf(scorestring, sizeof scorestring, "%d", score);

                  struct widget_factory_data wfd[] =
                    {
                      { WFDT_CAVE,       "cave",             gamedata->cave   },
                      { WFDT_MAP,        "map",              gamedata->map    },
                      { WFDT_STRING,     "score",            scorestring      },
                      { WFDT_ON_RELEASE, "on_play_clicked",  on_play_clicked  },
                      { WFDT_ON_RELEASE, "on_save_clicked",  on_save_clicked  },
                      { WFDT_ON_RELEASE, "on_quit_clicked",  on_quit_clicked  },
                      { WFDT_ON_UNLOAD,  "on_window_unload", on_window_unload },
                      { WFDT_SIZEOF_,    NULL,               NULL             }
                    };
                  window = widget_factory(wfd, NULL, "game_player_death");
                  widget_center_on_parent(window);
                  widget_set_y(window, 100);
                  widget_set_pointer(window, "gamedata", gamedata);

                  ui_push_handlers();
                  ui_set_navigation_handlers();
                  ui_set_common_handlers();
                  ui_set_handler(UIC_EXIT,   on_quit);
                  ui_set_handler(UIC_CANCEL, on_quit);
                }
              else
                {
                  snprintf(msg, sizeof msg,
                           gettext("The girl is dead.\n"
                                   "Diamond score: %d"),
                           score);
                  game_show_text(msg);
                }
            }
          else
            {
              snprintf(msg, sizeof msg,
                       gettext("    Game Over!\n"
                               "Diamond score: %d"),
                       score);
              game_show_text(msg);
            }
        }
    }
  else
    {
      char * chat = NULL;
      switch(get_rand(3))
        {
        case 0: chat = gettext("Ouch!");  break;
        case 1: chat = gettext("Yikes!"); break;
        case 2: chat = gettext("Eww..");  break;
        default: assert(false);
        }
      girl_chat(gamedata->map->girl, chat);
      gamedata->map->girl->mob->armour--;
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

      gamedata = widget_get_pointer(window, "gamedata");
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

      gamedata = widget_get_pointer(window, "gamedata");
      gamedata->quit = true;
      widget_delete(window);
    }
}

static void on_quit_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct gamedata * gamedata;

      gamedata = widget_get_pointer(window, "gamedata");
      gamedata->girls = 0;
      gamedata->quit  = true;
      widget_delete(window);
    }
}
