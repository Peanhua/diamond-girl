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

#include "ai.h"
#include "cave.h"
#include "diamond_girl.h"
#include "font.h"
#include "game.h"
#include "gfx.h"
#include "gfx_glyph.h"
#include "gfx_image.h"
#include "gfxbuf.h"
#include "girl.h"
#include "globals.h"
#include "image.h"
#include "map.h"
#include "playback.h"
#include "random.h"
#include "sfx.h"
#include "texture.h"
#include "themes.h"
#include "title.h"
#include "traits.h"
#include "ui.h"
#include "widget.h"
#include "widget_factory.h"

#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include <assert.h>
#include <libintl.h>
#include <stdlib.h>



static void set_ingame_handlers(void);
static void on_ui_draw(bool pressed, SDL_Event * event);
static void init_map(void);
static void reset_map(void);
static void toggle_pause(struct map * map, bool onoff);
static void set_playing_speed(int new_speed);
static void game_pause(void);
static void game_unpause(void);
static struct girl * find_current_girl(void);

/* UI input handlers, generic */
static void on_game_exit(bool pressed, SDL_Event * event);
static void on_pause_game(bool pressed, SDL_Event * event);
/* User UI input handlers */
static void on_user_cancel(bool pressed, SDL_Event * event);
static void on_user_manipulate(bool pressed, SDL_Event * event);
static void on_user_left(bool pressed, SDL_Event * event);
static void on_user_right(bool pressed, SDL_Event * event);
static void on_user_up(bool pressed, SDL_Event * event);
static void on_user_down(bool pressed, SDL_Event * event);
static void on_user_show_help(bool pressed, SDL_Event * event);
static void on_user_show_legend(bool pressed, SDL_Event * event);
static void on_user_toggle_fullscreen(bool pressed, SDL_Event * event);
static void on_time_control(bool pressed, SDL_Event * event);
/* AI UI input handlers */
static void on_toggle_playback_controls(bool pressed, SDL_Event * event);
static void on_playback_controls_unload(struct widget * this);
static void on_ai_slower(bool pressed, SDL_Event * event);
static void on_ai_faster(bool pressed, SDL_Event * event);
static void on_ai_hilite_target(bool pressed, SDL_Event * event);
static void on_ai_save_playback(bool pressed, SDL_Event * event);

/* Determine the girls move direction from down pressed controls. Called when a control is released. */
static void set_girl_direction_from_event(SDL_Event * event);


static int player_commits_suicide;
static int game_speed_modifier;
static int girl_chat_chance;



static bool controls[MOVE_NONE];
static int  controls_pressure[MOVE_NONE];

static struct widget * playback_controls;
static struct widget * pause_widgets; /* Temporary widgets during pause. */
static struct widget * help_window;


static int glyph_set_prior_pause;

static bool time_control_used;
static int  time_control_timer;

#ifdef WITH_OPENGL
static struct gfxbuf * scorebuf;
#endif
static bool scoreboard_is_dirty;

static struct
{
  char header[32];
  int  width;
  int  number1;
  int  number2;
} scoreboard[5] =
  {
    { "Level",      90, 0, -1 },
    { "Girls:",     90, 0, -1 },
    { "Score:",    340, 0, -1 },
    { "Diamonds:", 140, 0,  0 },
    { "Time:",     140, 0,  0 }
  };

static struct gamedata * gamedata;


struct gamedata * game(struct cave * cave, int level, bool iron_girl_mode, bool return_after_one_level, struct ai * computer_player)
{
  ALfloat saved_listener_position[3];
  struct theme * theme;
  float saved_sfx_volume;
  float saved_music_volume;
  struct widget * root;

  scoreboard_is_dirty = true;

  /* Overwrite the placeholder texts with locale specific texts: */
  snprintf(scoreboard[0].header, sizeof scoreboard[0].header, gettext("Level:"));
  snprintf(scoreboard[1].header, sizeof scoreboard[1].header, gettext("Girls:"));
  snprintf(scoreboard[2].header, sizeof scoreboard[2].header, gettext("Score:"));
  snprintf(scoreboard[3].header, sizeof scoreboard[3].header, gettext("Diamonds:"));
  snprintf(scoreboard[4].header, sizeof scoreboard[4].header, gettext("Time:"));

  alGetListenerfv(AL_POSITION, saved_listener_position);
  saved_sfx_volume   = sfx_get_volume();
  saved_music_volume = sfx_get_music_volume();

  playback_controls = NULL;
  pause_widgets     = NULL;
  help_window       = NULL;

  game_show_text(NULL);

  { /* Setup the game specific variables: */
    gamedata = gamedata_new(cave, iron_girl_mode, traits_get(cave->game_mode));
    gamedata->init_map                  = init_map;
    gamedata->reset_map                 = reset_map;
    gamedata->need_to_clear_colorbuffer = 1;

    if(computer_player == NULL && return_after_one_level == false && cave->savegame.exists && iron_girl_mode == false)
      {
        gamedata->current_level  = cave->savegame.game_level;
        game_speed_modifier      = cave->savegame.game_speed_modifier;
        gamedata->starting_girls = cave->savegame.starting_girls;
        gamedata->girls          = cave->savegame.girls;
        gamedata->score          = cave->savegame.score;
        gamedata->diamond_score  = cave->savegame.diamond_score;
        gamedata->diamonds       = cave->savegame.diamonds;
        gamedata->playback       = cave->savegame.playback;

        if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
          {
            for(int i = 0; i < gamedata->starting_girls; i++)
              globals.pyjama_party_girls[i]->mob->alive = cave->savegame.pyjama_party_girls[i];
          }
      }
    else
      {
        gamedata->current_level = level;
        game_speed_modifier = 0;

        if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
          {
            gamedata->girls = globals.pyjama_party_girls_size;

            for(int i = 0; i < gamedata->girls; i++)
              globals.pyjama_party_girls[i]->mob->alive = true;
          }
        else
          {
            if(gamedata->iron_girl_mode == true && (gamedata->traits & TRAIT_IRON_GIRL))
              gamedata->girls = 1; /* Iron Girl mode */
            else
              gamedata->girls = 3;
          }

        gamedata->starting_girls = gamedata->girls;
      }
  }

  gamedata->ai = computer_player;
  if(gamedata->ai != NULL)
    gamedata->ai->quit = false;

  theme = theme_get(THEME_SPECIAL_DAY);
  if(theme != NULL)
    if(theme->enabled == true)
      if(!strcmp(theme->name, "easter"))
        snprintf(scoreboard[3].header, sizeof scoreboard[3].header, "Eggs:");

  if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    {
      game_set_pyjama_party_control(PARTYCONTROLLER_PLAYER);
      strcpy(scoreboard[1].header, gettext("Girl:"));
    }
  else
    {
      scoreboard[1].number2 = -1;
      strcpy(scoreboard[1].header, gettext("Girls:"));
    }

  trait_t        saved_traits_active;
  trait_t        saved_traits_available;
  uint64_t       saved_traits_score;

  saved_traits_active    = traits_get_active();
  saved_traits_available = traits_get_available();
  saved_traits_score     = traits_get_score();


  ui_push_handlers();

  ui_initialize();
  root = widget_new_root(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  sfx_volume(1.0f);
  sfx_music_volume(0.5f);

#ifdef WITH_OPENGL
  scorebuf = NULL;
  if(globals.opengl)
    {
      scorebuf = gfxbuf_new(GFXBUF_DYNAMIC_2D, GL_QUADS, GFXBUF_TEXTURE | GFXBUF_BLENDING);
      assert(scorebuf != NULL);
      gfxbuf_resize(scorebuf, 256 * 4);
    }
#endif

  gamedata->exit_after_one_level = return_after_one_level;

  if(computer_player == NULL && (gamedata->cave->game_mode == GAME_MODE_ADVENTURE || gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY))
    girl_chat_chance = get_rand(100);
  else
    girl_chat_chance = 0;

  gamedata->init_map();

  if(cave->savegame.exists && iron_girl_mode == false && cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    {
      map_copy(gamedata->map, cave->savegame.map);

      /* Initialize the map start animation from the new map data. */
      gamedata->reset_map();

      /* The number of girls passed was set to 0 in init_map(), so we load it from the saved game here. */
      gamedata->pyjama_party_girls_passed = cave->savegame.pyjama_party_girls_passed;
    }

  SDL_initFramerate(&gamedata->framerate_manager);
  set_playing_speed(1);

  set_ingame_handlers();

  while(gamedata->quit == false)
    {
      /* Player controls */
      ui_process_events(false);

      if(gamedata->ai != NULL)
        { /* AI controls */
          if(!gamedata->quit && gamedata->ai->quit == false)
            {
              if(gamedata->map->girl->mob->alive == false)
                { /* Girl controlled by AI has died. */
                  /* In non-ai mode the function game_event() does the if&else as below, after the user presses enter.
                   * This stuff should be moved to the player death function, where we can do additional tweaks to the after-life -game.
                   * For example in non-ai mode, fade to red and slowly drip blood from top?
                   */
                  /* Pause for some time so that the watcher can see the death scene better. */
                  if(gamedata->girl_death_delay_timer > 0)
                    {
                      gamedata->girl_death_delay_timer -= 1;
                    }
                  else
                    {
                      if(gamedata->girls > 0)
                        {
                          if(gamedata->map->is_intermission)
                            gamedata->current_level++;
                          gamedata->init_map();
                        }
                      else
                        gamedata->quit = true;
                    }
                }
              
              if(gamedata->map->girl->mob->alive == true && gamedata->map->game_paused == false)
                {
                  struct playback_step * step;
                  
                  step = gamedata->ai->get_next_move(gamedata->ai, gamedata->map->level_start_anim_on ? false : true);
                  if(step != NULL)
                    {
                      gamedata->map->girl->mob->direction = step->direction;
                      gamedata->player_shift_move         = step->manipulate;
                      player_commits_suicide    = step->commit_suicide == true ? 1 : 0;
                    }
                }
            }
          else /* ai wants to quit, so do we then, too */
            gamedata->quit = 1;
        }

      if(gamedata->playback != NULL && gamedata->map->girl->mob->alive == true && gamedata->map->game_paused == false)
        { /* Take a snapshot of time. */
          struct playback * pb;
          struct playback_step * step;

          pb = gamedata->playback;
          while(pb->next != NULL)
            pb = pb->next;

          step = playback_append_step(pb);
          assert(step != NULL);
          if(step != NULL)
            {
#ifdef DEBUG_PLAYBACK
              step->timestamp       = gamedata->map->game_time;
              step->timestamp_timer = gamedata->map->game_timer;
#endif
              step->position[0]     = gamedata->map->girl->mob->x;
              step->position[1]     = gamedata->map->girl->mob->y;
              step->direction       = gamedata->map->girl->mob->direction;
              step->manipulate      = gamedata->player_shift_move;
              step->commit_suicide  = player_commits_suicide ? true : false;
            }
          else /* The recording would be garbled after this point anyway, so we delete the whole playback upon error. */
            gamedata->playback = playback_free(gamedata->playback);
        }

      if(gamedata->map->girl->mob->alive == true && player_commits_suicide && gamedata->map->game_paused == false)
        {
          gamedata->map->girl->mob->armour = 0;
          gamedata->map->player_death(gamedata, true);
          player_commits_suicide = 0;
        }

      if(gamedata->map->game_paused == false)
        {
          int finespeed;

          finespeed = 24 / 6;
          if(gamedata->ai != NULL && gamedata->playing_speed == 5)
            finespeed = 24 / 1;

          map_scroll_to_girl(gamedata->map, finespeed);
        }

      map_physics_tick(gamedata->map, gamedata);

      map_animations_tick(gamedata->map);

      if(!gamedata->map->level_start_anim_on)
	if(gamedata->map->girl->mob->alive == true && gamedata->map->game_paused == false)
	  {
	    gamedata->map->game_time--;

            if(time_control_timer > 0)
              {
                time_control_timer--;
                if(time_control_timer == 0)
                  set_playing_speed(1);
              }

	    if((gamedata->map->game_time % gamedata->map->frames_per_second) == 0)
              {
                int seconds_left;

                seconds_left = gamedata->map->game_time / gamedata->map->frames_per_second;
                if(seconds_left <= 10 && gamedata->map->game_time / gamedata->map->frames_per_second > 0)
                  {
                    sfx_pitch(1.0f + (float) (11 - seconds_left) / 10.0f);
                    if(gamedata->map->fast_forwarding == false)
                      sfx_emit(SFX_TIME, gamedata->map->girl->mob->x, gamedata->map->girl->mob->y);

                    if(globals.opengl)
                      {
                        double mood;

                        mood = 1.0 - (double) (10 - seconds_left) / 20.0;
                        assert(mood <= 1.0);
                        gamedata->map->display_colour[0] = mood * 255.0f;
                        gamedata->map->display_colour[1] = mood * 255.0f;
                        gamedata->map->display_colour[2] = mood * 255.0f;
                      }
                  }
              }
              
	    if(gamedata->map->game_time <= 0)
	      {
		gamedata->map->player_death(gamedata, false);
                gamedata->map->flash_screen = false;
#ifdef PROFILING
                gamedata->quit = true;
#endif
	      }
	  }

      if(gamedata->map->fast_forwarding == false)
        {
          ui_draw(gamedata->need_to_clear_colorbuffer ? true : false);
          if(gamedata->need_to_clear_colorbuffer > 0)
            if(gamedata->map->background_type == 0)
              gamedata->need_to_clear_colorbuffer--;
        }

#ifndef PROFILING
      if(gamedata->playing_speed > 0)
        if(gamedata->map == NULL || gamedata->map->fast_forwarding == false)
          SDL_framerateDelay(&gamedata->framerate_manager);
#endif
    }

  if(iron_girl_mode == false)
    if(gamedata->exit_after_one_level == false)
      { /* Only alter currently saved game when not in iron girl mode, and not in editor. */
        if(gamedata->girls > 0)
          { /* Save the game. */
            cave->savegame.exists              = true;
            cave->savegame.game_level          = gamedata->current_level;
            cave->savegame.game_speed_modifier = game_speed_modifier;
            cave->savegame.starting_girls      = gamedata->starting_girls;
            cave->savegame.girls               = gamedata->girls;
            cave->savegame.score               = gamedata->score;
            cave->savegame.diamond_score       = gamedata->diamond_score;
            cave->savegame.diamonds            = gamedata->diamonds;
            if(gamedata->playback != NULL)
              cave->savegame.playback          = playback_copy(gamedata->playback);
            else
              cave->savegame.playback          = NULL;
            if(cave->game_mode == GAME_MODE_PYJAMA_PARTY)
              {
                cave->savegame.pyjama_party_girls = malloc(sizeof(bool) * gamedata->starting_girls);
                assert(cave->savegame.pyjama_party_girls != NULL);
                for(int i = 0; i < gamedata->starting_girls; i++)
                  cave->savegame.pyjama_party_girls[i] = globals.pyjama_party_girls[i]->mob->alive;
                cave->savegame.pyjama_party_girls_passed = gamedata->pyjama_party_girls_passed;
                cave->savegame.map = map_copy(NULL, gamedata->map);
              }
            else
              cave->savegame.pyjama_party_girls = NULL;
          }
        else
          { /* Don't save the game. */
            cave->savegame.exists = false;
          }
      }

  if(gamedata->map->fast_forwarding == false)
    sfx_stop();

  sfx_volume(saved_sfx_volume);
  sfx_music_volume(saved_music_volume);
  sfx_set_listener_position(saved_listener_position[0], saved_listener_position[1]);

  if(gamedata->map != NULL)
    gamedata->map = map_free(gamedata->map);

  ui_pop_handlers();

  if(gamedata->ai != NULL)
    {
      traits_set(saved_traits_active, saved_traits_available, saved_traits_score);
    }

#ifdef WITH_OPENGL
  if(scorebuf != NULL)
    scorebuf = gfxbuf_free(scorebuf);
#endif

  if(playback_controls != NULL)
    playback_controls = widget_delete(playback_controls);

  game_show_text(NULL);

  root = widget_delete(root);

  /* Don't return trait info of the last party member. */
  if(gamedata->cave != NULL && gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    gamedata->traits = 0;

  return gamedata;
}


static void on_ui_draw(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(gamedata->map == NULL)
    return;

  if(pressed == true)
    {
      if(gamedata->map->flash_screen == false)
        {
          struct image * score_background_image;

          gfx_gl_log("%s:%s: score background", __FILE__, __FUNCTION__);
          if(globals.opengl_max_texture_size >= 800)
            score_background_image = gfx_image(GFX_IMAGE_SCORE_BACKGROUND);
          else
            score_background_image = gfx_image(GFX_IMAGE_SCORE_BACKGROUND_256);
          assert(score_background_image != NULL);
          if(score_background_image != NULL)
            gfx2d_draw_image_scaled(0, 0, score_background_image, 800, 45);

          gfx_gl_log("%s:%s: scores", __FILE__, __FUNCTION__);

          if(globals.opengl)
            {
#ifdef WITH_OPENGL
              if(scoreboard_is_dirty                              ||
                 scoreboard[0].number1 != gamedata->current_level ||
                 scoreboard[1].number1 != gamedata->girls         ||
                 scoreboard[2].number1 != (int) gamedata->score   ||
                 scoreboard[3].number1 != gamedata->map->diamonds            ||
                 scoreboard[3].number2 != gamedata->map->diamonds_needed     ||
                 scoreboard[4].number1 != gamedata->map->game_time / gamedata->map->frames_per_second ||
                 scoreboard[4].number2 != gamedata->map->time)
                { /* Refresh */
                  scoreboard_is_dirty = false;
                  scorebuf->vertices = 0;

                  scoreboard[0].number1 = gamedata->current_level;
                  if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
                    {
                      scoreboard[1].number1 = gamedata->pyjama_party_girls_passed + 1;
                      scoreboard[1].number2 = gamedata->girls;
                    }
                  else
                    scoreboard[1].number1 = gamedata->girls;
                  scoreboard[2].number1 = gamedata->score;
                  scoreboard[3].number1 = gamedata->map->diamonds;
                  scoreboard[3].number2 = gamedata->map->diamonds_needed;
                  scoreboard[4].number1 = gamedata->map->game_time / gamedata->map->frames_per_second;
                  scoreboard[4].number2 = gamedata->map->time;

                  int x, hy, ny;
                  bool irongirlmode;

                  if(gamedata->iron_girl_mode == true && (gamedata->traits & TRAIT_IRON_GIRL))
                    irongirlmode = true;
                  else
                    irongirlmode = false;
                  x = 0;
                  hy = (45 - font_height() * 2) / 2;
                  ny = hy + font_height();

                  for(int i = 0; i < 5; i++)
                    {
                      if(irongirlmode == false || i != 1)
                        {
                          char tbuf[256];
                          int tx;
                          
                          tx = x + 8 + (scoreboard[i].width - 8 * 2 - font_width(scoreboard[i].header)) / 2;
                          font_render(tx, hy, scoreboard[i].header, scorebuf);
                          
                          if(scoreboard[i].number2 == -1)
                            snprintf(tbuf, sizeof tbuf, "%d", scoreboard[i].number1);
                          else
                            snprintf(tbuf, sizeof tbuf, "%d/%d", scoreboard[i].number1, scoreboard[i].number2);
                  
                          tx = x + 8 + (scoreboard[i].width - 8 * 2 - font_width(tbuf)) / 2;
                          font_render(tx, ny, tbuf, scorebuf);
                        }

                      x += scoreboard[i].width;
                    }
                  gfxbuf_update(scorebuf, 0, scorebuf->vertices);
                }

              struct font * font;

              gfx_colour_white();
              font = font_get();
              gfxgl_bind_texture(font->image->texture);
              gfxbuf_draw_init(scorebuf);
              gfxbuf_draw(scorebuf);
#endif
            }
#ifdef WITH_NONOPENGL
          else
            {
              int x, hy, ny;

              scoreboard[0].number1 = gamedata->current_level;
              if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
                {
                  scoreboard[1].number1 = gamedata->pyjama_party_girls_passed + 1;
                  scoreboard[1].number2 = gamedata->girls;
                }
              else
                scoreboard[1].number1 = gamedata->girls;
              scoreboard[2].number1 = gamedata->score;
              scoreboard[3].number1 = gamedata->map->diamonds;
              scoreboard[3].number2 = gamedata->map->diamonds_needed;
              scoreboard[4].number1 = gamedata->map->game_time / gamedata->map->frames_per_second;
              scoreboard[4].number2 = gamedata->map->time;
              
              x = 0;
              hy = (45 - font_height() * 2) / 2;
              ny = hy + font_height();

              for(int i = 0; i < 5; i++)
                {
                  char tbuf[256];
                  int tx;

                  tx = x + 8 + (scoreboard[i].width - 8 * 2 - font_width(scoreboard[i].header)) / 2;
                  font_write(tx, hy, scoreboard[i].header);

                  if(scoreboard[i].number2 == -1)
                    snprintf(tbuf, sizeof tbuf, "%d", scoreboard[i].number1);
                  else
                    snprintf(tbuf, sizeof tbuf, "%d/%d", scoreboard[i].number1, scoreboard[i].number2);

                  tx = x + 8 + (scoreboard[i].width - 8 * 2 - font_width(tbuf)) / 2;

                  font_write(tx, ny, tbuf);

                  x += scoreboard[i].width;
                }
            }
#endif

          int top_x, top_y;
          int width, height;

          gfx_gl_log("%s:%s: map", __FILE__, __FUNCTION__);
          top_x = 24 + 4;
          top_y = 45 + 24;

          width = 31;
          height = 21;

          if(gamedata->map->background_type == 1 || gamedata->map->background_type == 2)
            {
              gfx_3d(true);
              draw_title_starfield();
              gfx_2d();
            }
          draw_map(gamedata->map,
                   gamedata->cave->game_mode,
                   gamedata->traits,
                   top_x, top_y,
                   width, height);
        }
    }
  else
    {
      if(gamedata->map->flash_screen == true)
        {
          SDL_Rect r;
          
          gamedata->map->flash_screen = false;
          r.x = 0;
          r.y = 0;
          r.w = SCREEN_WIDTH;
          r.h = SCREEN_HEIGHT;
          gfx_draw_rectangle(&r, 0xff, 0xff, 0xff, 0xff);
#ifdef WITH_OPENGL
          gamedata->need_to_clear_colorbuffer = 2;
#endif
        }

      if(gamedata->map->extra_life_anim > 0)
        gamedata->map->extra_life_anim--;
    }
}


static void on_user_manipulate(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(gamedata->map->game_paused == false)
    {
      if(gamedata->map->girl->mob->alive == true)
        {
          if(pressed == true)
            gamedata->player_shift_move = true;
          else
            gamedata->player_shift_move = false;
        }
      else
        {
          if(pressed == true)
            {
              assert(gamedata->girls == 0);
              gamedata->quit = true;
            }
        }
    }
}

static void set_girl_direction_from_event(SDL_Event * event)
{ /* Determine the direction from down pressed controls. */
  if(event->type == SDL_JOYAXISMOTION && globals.joysticks_analog[event->jaxis.which] == true)
    { /* analog mode */
      const int threshold = 32768 / 2;

      if(controls_pressure[MOVE_LEFT] >  threshold &&
         controls_pressure[MOVE_LEFT] >= controls_pressure[MOVE_RIGHT] &&
         controls_pressure[MOVE_LEFT] >= controls_pressure[MOVE_UP] &&
         controls_pressure[MOVE_LEFT] >= controls_pressure[MOVE_DOWN])
        gamedata->map->girl->mob->direction = MOVE_LEFT;
      else if(controls_pressure[MOVE_RIGHT] >  threshold &&
              controls_pressure[MOVE_RIGHT] >= controls_pressure[MOVE_LEFT] &&
              controls_pressure[MOVE_RIGHT] >= controls_pressure[MOVE_UP] &&
              controls_pressure[MOVE_RIGHT] >= controls_pressure[MOVE_DOWN])
        gamedata->map->girl->mob->direction = MOVE_RIGHT;
      else if(controls_pressure[MOVE_UP] >  threshold &&
              controls_pressure[MOVE_UP] >= controls_pressure[MOVE_LEFT] &&
              controls_pressure[MOVE_UP] >= controls_pressure[MOVE_RIGHT] &&
              controls_pressure[MOVE_UP] >= controls_pressure[MOVE_DOWN])
        gamedata->map->girl->mob->direction = MOVE_UP;
      else if(controls_pressure[MOVE_DOWN] >  threshold &&
              controls_pressure[MOVE_DOWN] >= controls_pressure[MOVE_LEFT] &&
              controls_pressure[MOVE_DOWN] >= controls_pressure[MOVE_RIGHT] &&
              controls_pressure[MOVE_DOWN] >= controls_pressure[MOVE_UP])
        gamedata->map->girl->mob->direction = MOVE_DOWN;
      else
        gamedata->map->girl->mob->direction = MOVE_NONE;
    }
  else
    { /* digital mode */
      if(controls[MOVE_LEFT] == true)
        gamedata->map->girl->mob->direction = MOVE_LEFT;
      else if(controls[MOVE_RIGHT] == true)
        gamedata->map->girl->mob->direction = MOVE_RIGHT;
      else if(controls[MOVE_UP] == true)
        gamedata->map->girl->mob->direction = MOVE_UP;
      else if(controls[MOVE_DOWN] == true)
        gamedata->map->girl->mob->direction = MOVE_DOWN;
      else
        gamedata->map->girl->mob->direction = MOVE_NONE;
    }
}


static void on_user_up(bool pressed, SDL_Event * event)
{
  if(gamedata->map->game_paused == false && gamedata->map->girl->mob->alive == true)
    {
      if(event->type == SDL_JOYAXISMOTION && globals.joysticks_analog[event->jaxis.which] == true)
        {
          controls_pressure[MOVE_UP] = abs(event->jaxis.value);
          set_girl_direction_from_event(event);
        }
      else
        {
          if(pressed == true)
            {
              controls[MOVE_UP] = true;
              gamedata->map->girl->mob->direction = MOVE_UP;
            }
          else
            {
              controls[MOVE_UP] = false;
              if(gamedata->map->girl->mob->direction == MOVE_UP)
                set_girl_direction_from_event(event);
            }
        }
    }
}


static void on_user_down(bool pressed, SDL_Event * event)
{
  if(gamedata->map->game_paused == false && gamedata->map->girl->mob->alive == true)
    {
      if(event->type == SDL_JOYAXISMOTION && globals.joysticks_analog[event->jaxis.which] == true)
        {
          controls_pressure[MOVE_DOWN] = abs(event->jaxis.value);
          set_girl_direction_from_event(event);
        }
      else
        {
          if(pressed == true)
            {
              controls[MOVE_DOWN] = true;
              gamedata->map->girl->mob->direction = MOVE_DOWN;
            }
          else
            {
              controls[MOVE_DOWN] = false;
              if(gamedata->map->girl->mob->direction == MOVE_DOWN)
                set_girl_direction_from_event(event);
            }
        }
    }
}


static void on_user_left(bool pressed, SDL_Event * event)
{
  if(gamedata->map->game_paused == false && gamedata->map->girl->mob->alive == true)
    {
      if(event->type == SDL_JOYAXISMOTION && globals.joysticks_analog[event->jaxis.which] == true)
        {
          controls_pressure[MOVE_LEFT] = abs(event->jaxis.value);
          set_girl_direction_from_event(event);
        }
      else
        {
          if(pressed == true)
            {
              controls[MOVE_LEFT] = true;
              gamedata->map->girl->mob->direction = MOVE_LEFT;
            }
          else
            {
              controls[MOVE_LEFT] = false;
              if(gamedata->map->girl->mob->direction == MOVE_LEFT)
                set_girl_direction_from_event(event);
            }
        }
    }
}


static void on_user_right(bool pressed, SDL_Event * event)
{
  if(gamedata->map->game_paused == false && gamedata->map->girl->mob->alive == true)
    {
      if(event->type == SDL_JOYAXISMOTION && globals.joysticks_analog[event->jaxis.which] == true)
        {
          controls_pressure[MOVE_RIGHT] = abs(event->jaxis.value);
          set_girl_direction_from_event(event);
        }
      else
        {
          if(pressed == true)
            {
              controls[MOVE_RIGHT] = true;
              gamedata->map->girl->mob->direction = MOVE_RIGHT;
            }
          else
            {
              controls[MOVE_RIGHT] = false;
              if(gamedata->map->girl->mob->direction == MOVE_RIGHT)
                set_girl_direction_from_event(event);
            }
        }
    }
}


static void on_user_show_help(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      game_pause();

      if(help_window != NULL)
        help_window = widget_delete(help_window);

      help_window = widget_new_game_help_window();
    }
}


static void on_user_show_legend(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      game_pause();

      if(help_window != NULL)
        help_window = widget_delete(help_window);

      help_window = widget_new_frame(widget_root(), 0, 0, 1, 1);
      if(help_window != NULL)
        {
          struct widget * legend;
          int padding;

          padding = 16;

          legend = widget_new_legend(help_window, padding, padding);

          widget_set_width(help_window, widget_width(legend) + padding);
          widget_set_height(help_window, widget_width(legend) + padding);
          widget_center(help_window);
        }
    }
}


static void on_user_toggle_fullscreen(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      if(globals.fullscreen)
        globals.fullscreen = 0;
      else
        globals.fullscreen = 1;
      gfx_cleanup();
      gfx_initialize();
    }
}



static void init_map(void)
{
  sfx_stop();
  
  if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    gamedata->pyjama_party_girls_passed = 0;

  if(gamedata->map != NULL)
    gamedata->map = map_free(gamedata->map);

  gamedata->map = map_get(gamedata->cave->name, gamedata->current_level);
  if(gamedata->map == NULL)
    { /* after last map, cycle to first */
      if(gamedata->ai != NULL && gamedata->ai->cave_restart != NULL)
        gamedata->ai->cave_restart(gamedata->ai);
      
      gamedata->current_level = 1;

      game_speed_modifier--;
      if(game_speed_modifier < -99)
        game_speed_modifier = -99;
      
      gamedata->map = map_get(gamedata->cave->name, gamedata->current_level);
      assert(gamedata->map != NULL);
    }

  struct playback * new_pb;

  new_pb = playback_new();
  assert(new_pb != NULL);
  
  new_pb->game_mode      = gamedata->cave->game_mode;
  new_pb->iron_girl_mode = gamedata->iron_girl_mode;
  new_pb->traits         = gamedata->traits;
  new_pb->cave           = strdup(gamedata->cave->name);
  new_pb->level          = gamedata->current_level;

  new_pb->map_hash = map_calculate_hash(gamedata->map);

  if(gamedata->playback == NULL)
    {
      gamedata->playback = new_pb;
    }
  else
    {
      struct playback * tmp;
      
      tmp = gamedata->playback;
      while(tmp->next != NULL)
        tmp = tmp->next;
      
      tmp->next = new_pb;
    }

  if(gamedata->map != NULL)
    {
      bool ameba;

      ameba = false;
      for(int i = 0; ameba == false && i < gamedata->map->width * gamedata->map->height; i++)
        if(gamedata->map->data[i] == MAP_AMEBA)
          ameba = true;

      if(ameba == true)
        {
          if(gamedata->map->fast_forwarding == false)
            sfx_emit(SFX_AMEBA, 0, 0);
          gamedata->map->ameba_blocked_timer = gamedata->map->frames_per_second * 4;
        }

      gamedata->map->diamonds = 0;
      gamedata->map->exit_open = false;
      gamedata->map->morpher_is_on = 0;
      gamedata->map->morpher_end_time = 0;

      { /* Setup slime random values. */
        void * rs;
            
        rs = seed_rand_state(gamedata->map->width + gamedata->map->height * 42);
        for(int y = 0; y < gamedata->map->height; y++)
          for(int x = 0; x < gamedata->map->width; x++)
            if(gamedata->map->data[x + y * gamedata->map->width] == MAP_SLIME)
              gamedata->map->move_directions[x + y * gamedata->map->width] = get_rand_state(40, rs);
        free(rs);
      }

      if(gamedata->traits & TRAIT_CHAOS)
        {
          void * rs;
          unsigned int illegal;

          illegal = gamedata->map->width * gamedata->map->height;

          for(int i = 0; i < 2 * gamedata->map->width * gamedata->map->height; i += 2)
            gamedata->map->order[i] = illegal;

          rs = seed_rand_state(gamedata->map->width + gamedata->map->height * 42);
          for(int y = 0; y < gamedata->map->height; y++)
            for(int x = 0; x < gamedata->map->width; x++)
              {
                int n;

                do
                  {
                    n = get_rand_state(gamedata->map->width * gamedata->map->height, rs);
                  } while(gamedata->map->order[n * 2 + 0] != illegal);
                gamedata->map->order[n * 2 + 0] = x;
                gamedata->map->order[n * 2 + 1] = y;
              }
          free(rs);
        }

      struct theme * theme;
          
      theme = theme_get(THEME_SPECIAL_DAY);
      if(theme != NULL)
        if(!strcmp(theme->name, "new-year"))
          gamedata->map->background_type = 1;
    }

  reset_map();
}







void game_add_score(int score)
{
  bool iron;

  /* Iron girl mode doubles the score. */
  iron = false;
  if(gamedata->iron_girl_mode == true && (gamedata->traits & TRAIT_IRON_GIRL))
    {
      iron = true;
      score *= 2;
    }

  /* Double score if she's greedy, and the extra life anim is on. */
  if(gamedata->traits & TRAIT_GREEDY)
    if(gamedata->map->extra_life_anim > 0)
      score *= 2;

  if(iron == false && gamedata->cave->game_mode != GAME_MODE_PYJAMA_PARTY)
    { /* Extra girl */
      int elifescore;

      elifescore = 200;
      if(gamedata->score / elifescore < (gamedata->score + score) / elifescore)
        {
          gamedata->girls++;
          gamedata->map->extra_life_anim = gamedata->map->frames_per_second * 5;
        }
    }

  gamedata->score += score;
}

static void toggle_pause(struct map * map DG_NDEBUG_UNUSED, bool onoff)
{
  assert(map->game_paused != onoff);
  if(onoff == true)
    { /* Pause the game. */
      game_pause();
    }
  else
    { /* Un-pause the game. */
      game_unpause();
    }
}

static void on_pause_game(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    toggle_pause(gamedata->map, gamedata->map->game_paused == true ? false : true);
}


static void on_game_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    gamedata->quit = 1;
}

static void on_user_cancel(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(gamedata->map->game_paused == false)
    if(pressed == true)
      {
        if(gamedata->map->girl->mob->alive == true)
          {
            player_commits_suicide = 1;
            if(gamedata->ai != NULL)
              gamedata->ai = ai_free(gamedata->ai);
          }
        else
          gamedata->quit = true;
      }
}

static void on_playback_controls_unload(struct widget * this DG_UNUSED)
{
  playback_controls = NULL;
}

static void on_toggle_playback_controls(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      if(playback_controls == NULL)
        {
          assert(gamedata->ai != NULL);
          playback_controls = widget_new_playback_controls(NULL,
                                                           SCREEN_WIDTH - 20 - (10 + 42 * 4 + 10),
                                                           SCREEN_HEIGHT - 20 - (24 + 40 + 10),
                                                           gamedata->ai->playback_title);
          if(playback_controls != NULL)
            {
              widget_set_on_unload(playback_controls, on_playback_controls_unload);
              widget_set_pointer(playback_controls, "map", gamedata->map);
            }
        }
      else
        playback_controls = widget_delete(playback_controls);
    }
}

static void on_ai_slower(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    set_playing_speed(gamedata->playing_speed + 1);
}

static void on_ai_faster(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    set_playing_speed(gamedata->playing_speed - 1);
}

static void on_time_control(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    if(time_control_used == false)
      {
        time_control_used  = true;
        time_control_timer = gamedata->map->frames_per_second * 5;
        set_playing_speed(3);
      }
}

static void on_ai_hilite_target(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      globals.ai_hilite_current_target = globals.ai_hilite_current_target == true ? false : true;
#ifndef NDEBUG
      printf("ai_hilite_current_target=%s\n", globals.ai_hilite_current_target == true ? "on" : "off");
#endif
    }
}

static void on_ai_save_playback(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      char fn[1024];

      snprintf(fn, sizeof fn, "/tmp/playback-%ld.dgp", (long) time(NULL));
      printf("Saving playback: %s", fn);
      playback_save(fn, gamedata->playback);
      printf("\n");
    }
}


static void set_playing_speed(int new_speed)
{
  if(new_speed >= 0 && new_speed <= 5)
    {
      gamedata->playing_speed = new_speed;
      if(gamedata->playing_speed > 0)
        {
          if(gamedata->playing_speed < 5)
            SDL_setFramerate(&gamedata->framerate_manager, gamedata->map->frames_per_second / gamedata->playing_speed);
          else
            SDL_setFramerate(&gamedata->framerate_manager, 1);
        }
    }
}




static void game_pause(void)
{
  if(gamedata->map->game_paused == false)
    {
      glyph_set_prior_pause = gamedata->map->glyph_set;
      gamedata->map->glyph_set        = 1;
      gamedata->map->game_paused      = true;

      ui_push_handlers();
      ui_set_handler(UIC_EXIT,           on_pause_game);
      ui_set_handler(UIC_CANCEL,         on_pause_game);
      ui_set_handler(UIC_MANIPULATE,     on_pause_game);
      ui_set_handler(UIC_ALT_MANIPULATE, on_pause_game);


      /* Setup invisible widgets over the traits on the border for tooltips. */
      int top_x, top_y;
  
      top_x = 24 + 4;
      top_y = 45 + 24;
  
      trait_t traits[] =
        {
          TRAIT_KEY,
          TRAIT_RIBBON,
          TRAIT_GREEDY,
          TRAIT_TIME_CONTROL,
          TRAIT_POWER_PUSH,
          TRAIT_DIAMOND_PUSH,
          TRAIT_RECYCLER,
          TRAIT_STARS1,
          TRAIT_STARS2,
          TRAIT_STARS3,
          TRAIT_CHAOS,
          TRAIT_DYNAMITE,
          TRAIT_ALL
        };
  
      pause_widgets = widget_new_child(widget_root(), 0, 0, 0, 0);

      struct widget * w;
      char * name;

      name = NULL;
      switch(gamedata->cave->game_mode)
        {
        case GAME_MODE_CLASSIC:
          name = gettext("Game mode: Classic");
          break;
        case GAME_MODE_ADVENTURE:
          name = gettext("Game mode: Adventure");
          break;
        case GAME_MODE_PYJAMA_PARTY:
          name = gettext("Game mode: Pyjama Party");
          break;
        }
      if(name != NULL)
        {
          w = widget_new_frame(pause_widgets, top_x - 24, top_y - 24 + 2, 20, 20);
          widget_add_flags(w, WF_FOCUSABLE);
          widget_set_on_draw(w, NULL);
          widget_set_tooltip(w, name);
        }

      if(globals.iron_girl_mode)
        {
          w = widget_new_frame(pause_widgets, top_x - 24, top_y + 2, 20, 20);
          widget_add_flags(w, WF_FOCUSABLE);
          widget_set_on_draw(w, NULL);
          widget_set_tooltip(w, gettext("Iron Girl"));
        }
  
      for(int i = 0; traits[i] != TRAIT_ALL; i++)
        if(gamedata->traits & traits[i])
          {
            w = widget_new_frame(pause_widgets, top_x, top_y - 24 + 2, 20, 20);
            top_x += 24;
            widget_add_flags(w, WF_FOCUSABLE);
            widget_set_on_draw(w, NULL);
            name = trait_get_name(traits[i]);
            assert(name != NULL);
            if(name != NULL)
              widget_set_tooltip(w, name);
          }
    }
}

static void game_unpause(void)
{
  if(gamedata->map->game_paused == true)
    {
      gamedata->map->glyph_set   = glyph_set_prior_pause;
      gamedata->map->game_paused = false;
      pause_widgets = widget_delete(pause_widgets);
      if(help_window != NULL)
        help_window = widget_delete(help_window);

      ui_pop_handlers();
    }
}





static void reset_map(void)
{
  assert(gamedata->cave != NULL);
  assert(gamedata->map != NULL);
  gamedata->map->player_target = NULL;

  gamedata->pyjama_party_girl = find_current_girl();
  if(gamedata->pyjama_party_girl != NULL)
    gamedata->traits = gamedata->pyjama_party_girl->traits;

  game_show_text(NULL);

  for(int i = 0; i < MOVE_NONE; i++)
    {
      controls[i] = false;
      controls_pressure[i] = 0;
    }

  sfx_pitch(1.0f);

  if(gamedata->map != NULL)
    {
      gamedata->map->game_paused       = false;
      gamedata->map->anim_timer        = 0;
      gamedata->map->frames_per_second = 30;
      gamedata->map->player_movement   = game_player_movement;
      gamedata->map->player_death      = game_player_death;
      gamedata->map->player_next_level = game_player_next_level;
      gamedata->map->toggle_pause      = toggle_pause;

      if(gamedata->pyjama_party_girl != NULL)
        { /* Copy data from the pyjama party girl. */
          strcpy(gamedata->map->girl->name,        gamedata->pyjama_party_girl->name);
          gamedata->map->girl->birth_time        = gamedata->pyjama_party_girl->birth_time;
          gamedata->map->girl->traits            = gamedata->pyjama_party_girl->traits;
          gamedata->map->girl->possible_traits   = gamedata->pyjama_party_girl->possible_traits;
          gamedata->map->girl->diamonds          = gamedata->pyjama_party_girl->diamonds;
          gamedata->map->girl->diamonds_to_trait = gamedata->pyjama_party_girl->diamonds_to_trait;
        }
      game_move_girl(gamedata->map, gamedata->map->start_x, gamedata->map->start_y);
      gamedata->map->girl->mob->direction         = MOVE_NONE;
      gamedata->map->girl->mob->moved_over_glyph  = MAP_EMPTY;
      gamedata->map->girl->mob->animation         = 0;
      gamedata->map->girl->mob->animation_repeats = 0;
      gamedata->map->girl->mob->chat_chance       = girl_chat_chance;
      gamedata->map->girl->mob->alive             = true;
      gamedata->map->girl->mob->armour            = 1;
      if(gamedata->traits & TRAIT_RIBBON)
        gamedata->map->girl->mob->armour          = 2;

      gamedata->player_shift_move = false;
      gfx_frame0();
      player_commits_suicide = 0;

      gamedata->push_counter = 0;
      gamedata->map->flash_screen = false;

      gamedata->map->game_time = gamedata->map->time * gamedata->map->frames_per_second;
#ifdef PROFILING
      gamedata->map->game_time = 120 * gamedata->map->frames_per_second;
#endif
      gamedata->map->borders = true;

      if(gamedata->map->game_speed + game_speed_modifier >= 1)
	gamedata->map->game_speed += game_speed_modifier;
      else
	gamedata->map->game_speed = 1;

      if(globals.opengl)
        {
          gamedata->map->display_colour[0] = 0xff;
          gamedata->map->display_colour[1] = 0xff;
          gamedata->map->display_colour[2] = 0xff;
          gamedata->map->display_colour[3] = 0xff;
        }

      if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY && gamedata->pyjama_party_control == PARTYCONTROLLER_PARTY)
        {
          gamedata->map->data[gamedata->map->start_x + gamedata->map->start_y * gamedata->map->width] = MAP_PLAYER;
        }
      else
        {
          sfx_music(MUSIC_START, 0);
          map_level_start_animation_start(gamedata->map);
        }
    }
  else
    {
      fprintf(stderr, "Failed to load map, unable to start/continue game.\n");
      gamedata->quit = true;
    }

  if(gamedata->ai != NULL)
    {
      gamedata->ai->map = gamedata->map;

      if(gamedata->ai->map != NULL && gamedata->ai->use_planning == true)
        {
          ai_plan_level(gamedata->ai, gamedata->cave->game_mode);
          if(playback_controls != NULL)
            widget_set_string(playback_controls, "title", "%s", gamedata->ai->playback_title);
        }
    }

  time_control_used = false;
  if(gamedata->ai == NULL)
    set_playing_speed(1);
  else if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    set_playing_speed(0);

  if(playback_controls != NULL)
    widget_set_pointer(playback_controls, "map", gamedata->map);
}


static void set_ingame_handlers(void)
{
  ui_clear_handlers();
  ui_set_common_handlers();
  ui_set_handler(UIC_EXIT,              on_game_exit);
  ui_set_handler(UIC_DRAW,              on_ui_draw);
  ui_set_handler(UIC_PAUSE,             on_pause_game);
  ui_set_handler(UIC_TOGGLE_FULLSCREEN, on_user_toggle_fullscreen);
  if(gamedata->ai != NULL)
    {
      ui_set_navigation_handlers();
      if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
        ui_set_handler(UIC_CANCEL,      on_user_cancel);
      else
        ui_set_handler(UIC_CANCEL,      on_game_exit);
      ui_set_handler(UIC_SLOWER,        on_ai_slower);
      ui_set_handler(UIC_FASTER,        on_ai_faster);
      ui_set_handler(UIC_HILITE_TARGET, on_ai_hilite_target);
      ui_set_handler(UIC_SAVE_PLAYBACK, on_ai_save_playback);
      if(gamedata->cave->game_mode != GAME_MODE_PYJAMA_PARTY)
        {
          ui_set_handler(UIC_SHOW_HELP, on_toggle_playback_controls);
          if(playback_controls == NULL)
            on_toggle_playback_controls(true, NULL);
        }
    }
  else
    {
      ui_set_handler(UIC_CANCEL,      on_user_cancel);
      ui_set_handler(UIC_MANIPULATE,  on_user_manipulate);
      ui_set_handler(UIC_UP,          on_user_up);
      ui_set_handler(UIC_DOWN,        on_user_down);
      ui_set_handler(UIC_LEFT,        on_user_left);
      ui_set_handler(UIC_RIGHT,       on_user_right);
      ui_set_handler(UIC_SHOW_HELP,   on_user_show_help);
      ui_set_handler(UIC_SHOW_LEGEND, on_user_show_legend);
      if(gamedata->traits & TRAIT_TIME_CONTROL)
        ui_set_handler(UIC_SLOWER, on_time_control);
      if(gamedata->cave->game_mode != GAME_MODE_PYJAMA_PARTY)
        if(playback_controls != NULL)
          on_toggle_playback_controls(true, NULL);
    }
}

void game_set_pyjama_party_control(enum PARTYCONTROLLER controller)
{
  if(gamedata->ai != NULL)
    gamedata->ai = ai_free(gamedata->ai);

  gamedata->pyjama_party_control = controller;

  if(gamedata->pyjama_party_control == PARTYCONTROLLER_GIRL || gamedata->pyjama_party_control == PARTYCONTROLLER_PARTY)
    if(gamedata->ai == NULL)
      {
        gamedata->ai = ai_new();
        ai_pyjama_party(gamedata->ai);
        gamedata->ai->playback_title = gamedata->pyjama_party_girl->name;
      }
  
  set_ingame_handlers();
}


static struct girl * find_current_girl(void)
{
  struct girl * girl;

  girl = NULL;
  if(gamedata->cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    { /* Find the current girl. */
      int n;
      
      n = 0;
      for(int i = 0; girl == NULL && i < gamedata->starting_girls; i++)
        if(globals.pyjama_party_girls[i]->mob->alive == true)
          {
            if(n == gamedata->pyjama_party_girls_passed)
              girl = globals.pyjama_party_girls[i];
            n++;
          }
      assert(girl != NULL);
    }
  return girl;
}
