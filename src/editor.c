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

#include "cave.h"
#include "diamond_girl.h"
#include "font.h"
#include "game.h"
#include "gfx.h"
#include "gfx_glyph.h"
#include "gfx_material.h"
#include "gfxbuf.h"
#include "globals.h"
#include "map.h"
#include "sfx.h"
#include "ui.h"
#include "widget.h"
#include "widget_factory.h"

#include <assert.h>
#include <stdlib.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_framerate.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <libintl.h>

enum UIOBJ
  {
    UIOBJ_NONE,
    UIOBJ_CAVE,
    UIOBJ_WIDTH,
    UIOBJ_HEIGHT,
    UIOBJ_TIME,
    UIOBJ_GAMESPEED,
    UIOBJ_AMEBATIME,
    UIOBJ_DIAMONDSNEEDED,
    UIOBJ_DIAMONDSCORE,
    UIOBJ_TIMESCORE,
    UIOBJ_IS_INTERMISSION,
    UIOBJ_SIZEOF_
  };

static int editor_quit;
static struct map * map;
static struct widget * uiobj_glyphs;
static struct widget * uiobj_map;
static struct widget * uiobj[UIOBJ_SIZEOF_];

static enum MAP_GLYPH glyphs[] = 
  {
    MAP_EMPTY,
    MAP_SAND,
    MAP_BOULDER,
    MAP_DIAMOND,
    MAP_ENEMY1,
    MAP_ENEMY2,
    MAP_BRICK,
    MAP_BRICK_EXPANDING,
    MAP_BRICK_MORPHER,
    MAP_BRICK_UNBREAKABLE,
    MAP_AMEBA,
    MAP_SLIME,
    MAP_PLAYER,
    MAP_EXIT_LEVEL,
    MAP_SIZEOF_
  };

static int glyphs_count;
static int selected_glyph;

static bool map_is_dirty;

static void apply_changes(void);
static void init_map(const char * cave, int level);
static void change_map(int new_level);
static void change_map_width(int width_change);
static void change_map_height(int height_change);
static void map_update(void);
static void revert_map(void);
static void play_test(void);
static bool setup_ui(void);


static void button_change_activate(struct widget * this, enum WIDGET_BUTTON button);
static void button_change_release(struct widget * this, enum WIDGET_BUTTON button);

static void button_level_activate(struct widget * this, enum WIDGET_BUTTON button);
static void button_quit_activate(struct widget * this, enum WIDGET_BUTTON button);
static void button_play_activate(struct widget * this, enum WIDGET_BUTTON button);
static void button_revert_activate(struct widget * this, enum WIDGET_BUTTON button);
static void button_save_activate(struct widget * this, enum WIDGET_BUTTON button);
static void button_glyphs(struct widget * this, enum WIDGET_BUTTON button, int x, int y);
static void button_glyphs_draw(struct widget * this);
static void button_map_on_mouse_move(struct widget * this, int x, int y);
static void button_map_activate(struct widget * this, enum WIDGET_BUTTON button, int x, int y);
static void button_map_release(struct widget * this, enum WIDGET_BUTTON button);
static void button_is_intermission_release(struct widget * this, enum WIDGET_BUTTON button);
static void button_cave_release(struct widget * this, enum WIDGET_BUTTON button);


static void button_level_draw(struct widget * this);
static void button_width_draw(struct widget * this);
static void button_height_draw(struct widget * this);
static void button_time_draw(struct widget * this);
static void button_gamespeed_draw(struct widget * this);
static void button_amebatime_draw(struct widget * this);
static void button_diamondsneeded_draw(struct widget * this);
static void button_diamondscore_draw(struct widget * this);
static void button_timescore_draw(struct widget * this);
static void button_save_draw(struct widget * this);

void map_editor(struct cave * cave, int level)
{
  FPSmanager framerate_manager;

  ui_state_push();
  
  globals.ai_hilite_current_target = true;

  glyphs_count = 0;
  for(int i = 0; glyphs[i] != MAP_SIZEOF_; i++)
    glyphs_count++;

  gfx_frame0();

  if(setup_ui())
    {
      int k_leftright, k_updown;
      int cursor_move_delay;

      SDL_initFramerate(&framerate_manager);
      SDL_setFramerate(&framerate_manager, 75);

      assert(cave != NULL);

      map = NULL;
      selected_glyph = 0;

      k_leftright = 0;
      k_updown = 0;
      cursor_move_delay = 0;

      init_map(cave->name, level);

      editor_quit = 0;
      while(!editor_quit)
        {
          SDL_Event event;

          while(SDL_PollEvent(&event))
            {
              switch(event.type)
                {
                case SDL_QUIT:
                  editor_quit = 1;
                  break;
                case SDL_KEYDOWN:
                  switch(event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                      editor_quit = 1;
                      break;
                    case SDLK_LEFT:
                      k_leftright = -1;
                      cursor_move_delay = 0;
                      break;
                    case SDLK_RIGHT:
                      k_leftright = 1;
                      cursor_move_delay = 0;
                      break;
                    case SDLK_UP:
                      k_updown = -1;
                      cursor_move_delay = 0;
                      break;
                    case SDLK_DOWN:
                      k_updown = 1;
                      cursor_move_delay = 0;
                      break;
                    case SDLK_PLUS:
                      change_map(map->level + 1);
                      break;
                    case SDLK_MINUS:
                      if(map->level > 1)
                        change_map(map->level - 1);
                      break;
                    case SDLK_COMMA:
                      if(selected_glyph > 0)
                        {
                          selected_glyph--;
                        }
                      break;
                    case SDLK_PERIOD:
                      if(glyphs[selected_glyph + 1] != MAP_SIZEOF_)
                        {
                          selected_glyph++;
                        }
                      break;
                    case SDLK_w:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_WIDTH], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_WIDTH], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_WIDTH], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_WIDTH], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_h:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_HEIGHT], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_HEIGHT], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_HEIGHT], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_HEIGHT], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_SPACE:
                      map_update();
                      widget_set_long(uiobj_map, "UPDATE", 1);
                      break;
                    case SDLK_t:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_TIME], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_TIME], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_TIME], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_TIME], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_a:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_AMEBATIME], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_AMEBATIME], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_AMEBATIME], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_AMEBATIME], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_c:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_TIMESCORE], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_TIMESCORE], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_TIMESCORE], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_TIMESCORE], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_d:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_e:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_GAMESPEED], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_GAMESPEED], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_GAMESPEED], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_GAMESPEED], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_s:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          widget_set_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE", 1);
                          widget_set_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE_TIMER", 0);
                        }
                      else
                        {
                          widget_set_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE", -1);
                          widget_set_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE_TIMER", 0);
                        }
                      break;
                    case SDLK_v:
                      if(globals.read_only == false)
                        if(map_is_dirty)
                          {
                            map_save(map);
                            map_is_dirty = 0;
                          }
                      break;
                    case SDLK_p:
                      play_test();
                      break;
                    case SDLK_r:
                      if(event.key.keysym.mod & KMOD_SHIFT)
                        {
                          revert_map();
                        }
                      break;
                    default:
                      break;
                    }
                  break;
                case SDL_KEYUP:
                  switch(event.key.keysym.sym)
                    {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                      k_leftright = 0;
                      break;
                    case SDLK_UP:
                    case SDLK_DOWN:
                      k_updown = 0;
                      break;
                    case SDLK_SPACE:
                      widget_set_long(uiobj_map, "UPDATE", 0);
                      break;
                    case SDLK_w:
                      widget_set_long(uiobj[UIOBJ_WIDTH], "CHANGE", 0);
                      break;
                    case SDLK_h:
                      widget_set_long(uiobj[UIOBJ_HEIGHT], "CHANGE", 0);
                      break;
                    case SDLK_t:
                      widget_set_long(uiobj[UIOBJ_TIME], "CHANGE", 0);
                      break;
                    case SDLK_a:
                      widget_set_long(uiobj[UIOBJ_AMEBATIME], "CHANGE", 0);
                      break;
                    case SDLK_c:
                      widget_set_long(uiobj[UIOBJ_TIMESCORE], "CHANGE", 0);
                      break;
                    case SDLK_d:
                      widget_set_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE", 0);
                      break;
                    case SDLK_e:
                      widget_set_long(uiobj[UIOBJ_GAMESPEED], "CHANGE", 0);
                      break;
                    case SDLK_s:
                      widget_set_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE", 0);
                      break;
                    default:
                      break;
                    }
                  break;
                case SDL_MOUSEMOTION:
                  ui_process_mouse_move(event.motion.x, event.motion.y);
                  break;
                case SDL_MOUSEBUTTONDOWN:
                  {
                    enum WIDGET_BUTTON b;

                    if(event.button.button == SDL_BUTTON_LEFT)
                      b = WIDGET_BUTTON_LEFT;
                    else if(event.button.button == SDL_BUTTON_MIDDLE)
                      b = WIDGET_BUTTON_MIDDLE;
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                      b = WIDGET_BUTTON_RIGHT;
                    else
                      b = WIDGET_BUTTON_LEFT;
                      
                    ui_process_mouse_button(event.button.x, event.button.y, b, 0);
                  }
                  break;
                case SDL_MOUSEBUTTONUP:
                  {
                    enum WIDGET_BUTTON b;

                    if(event.button.button == SDL_BUTTON_LEFT)
                      b = WIDGET_BUTTON_LEFT;
                    else if(event.button.button == SDL_BUTTON_MIDDLE)
                      b = WIDGET_BUTTON_MIDDLE;
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                      b = WIDGET_BUTTON_RIGHT;
                    else
                      b = WIDGET_BUTTON_LEFT;
                      
                    ui_process_mouse_button(event.button.x, event.button.y, b, 1);
                  }
                  break;
                }
            }

          if(cursor_move_delay == 0)
            {
              cursor_move_delay = 7;

              if(k_leftright == -1)
                {
                  if(map->player_target->x > 0)
                    {
                      map->player_target->x--;
                      if(widget_get_long(uiobj_map, "UPDATE"))
                        map_update();
                    }
                }
              else if(k_leftright == 1)
                {
                  if(map->player_target->x + 1 < map->width)
                    {
                      map->player_target->x++;
                      if(widget_get_long(uiobj_map, "UPDATE"))
                        map_update();
                    }
                }

              if(k_updown == -1)
                {
                  if(map->player_target->y > 0)
                    {
                      map->player_target->y--;
                      if(widget_get_long(uiobj_map, "UPDATE"))
                        map_update();
                    }
                }
              else if(k_updown == 1)
                {
                  if(map->player_target->y + 1 < map->height)
                    {
                      map->player_target->y++;
                      if(widget_get_long(uiobj_map, "UPDATE"))
                        map_update();
                    }
                }
            }
          else
            {
              cursor_move_delay--;
            }


          apply_changes();


          ui_draw(true);

          SDL_framerateDelay(&framerate_manager);
        }

      if(map != NULL)
        map = map_free(map);
    }

  widget_delete(widget_root());
  ui_state_pop();
  
  globals.ai_hilite_current_target = false;
}

static void apply_changes(void)
{
  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_TIME], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_TIME], "CHANGE_TIMER");

        if(timer == 0)
          {
            if(map->time + change >= 0 && map->time + change <= 999)
              {
                map->time += change;
              }
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_TIME], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_WIDTH], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_WIDTH], "CHANGE_TIMER");

        if(timer == 0)
          {
            change_map_width(change);
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_WIDTH], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_HEIGHT], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_HEIGHT], "CHANGE_TIMER");

        if(timer == 0)
          {
            change_map_height(change);
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_HEIGHT], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_GAMESPEED], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_GAMESPEED], "CHANGE_TIMER");

        if(timer == 0)
          {
            if(map->game_speed + change >= 1 && map->game_speed + change <= 20)
              {
                map->game_speed += change;
              }
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_GAMESPEED], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_AMEBATIME], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_AMEBATIME], "CHANGE_TIMER");

        if(timer == 0)
          {
            if(map->ameba_time + change >= 0 && map->ameba_time + change <= 999)
              {
                map->ameba_time += change;
              }
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_AMEBATIME], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE_TIMER");

        if(timer == 0)
          {
            if(map->diamonds_needed + change >= 0 && map->diamonds_needed + change <= 999)
              {
                map->diamonds_needed += change;
              }
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_DIAMONDSNEEDED], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE_TIMER");

        if(timer == 0)
          {
            if(map->diamond_score + change >= 0 && map->diamond_score + change <= 999)
              {
                map->diamond_score += change;
              }
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_DIAMONDSCORE], "CHANGE_TIMER", timer);
      }
  }

  {
    long change;

    change = widget_get_long(uiobj[UIOBJ_TIMESCORE], "CHANGE");
    if(change)
      {
        long timer;

        timer = widget_get_long(uiobj[UIOBJ_TIMESCORE], "CHANGE_TIMER");

        if(timer == 0)
          {
            if(map->time_score + change >= 0 && map->time_score + change <= 999)
              {
                map->time_score += change;
              }
            timer = 7;
          }
        else
          {
            timer--;
          }
                
        widget_set_long(uiobj[UIOBJ_TIMESCORE], "CHANGE_TIMER", timer);
      }
  }
}

static void init_map(const char * cave, int level)
{
  struct map * oldmap;

  oldmap = map;

  if(cave == NULL)
    cave = oldmap->cave_name;

  widget_set_string(uiobj[UIOBJ_CAVE], "text", "%s '%-.8s'", gettext("Cave:"), cave);
  
  map = map_load(cave, level);
  if(map != NULL)
    {
      map->data[map->start_x + map->start_y * map->width] = MAP_PLAYER;
      map->data[map->exit_x + map->exit_y * map->width] = MAP_EXIT_LEVEL;
    }
  else
    {
      map = map_new(40, 20);
      map->cave_name = strdup(cave);
      map->level     = level;
    }

  map->in_editor = true;

  map->player_target = malloc(sizeof *map->player_target);
  assert(map->player_target != NULL);
  map->player_target->x = 0;
  map->player_target->y = 0;
  map->player_target->age = 0;
  map->player_target->path = NULL;

  map_is_dirty = 0;

  widget_set_map_pointer(uiobj_map, "map", map);
  widget_set_long(uiobj[UIOBJ_IS_INTERMISSION], "checked", map->is_intermission);

  if(oldmap != NULL)
    oldmap = map_free(oldmap);
}

static void change_map(int new_level)
{
  if(new_level != map->level)
    init_map(NULL, new_level);
}




static void button_level_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("Level[+-]: %-3d"), (int) map->level);
  widget_draw(this);
}

static void button_level_activate(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      if(map->level > 1)
        {
          change_map(map->level - 1);
        }
    }
  else if(button == WIDGET_BUTTON_RIGHT)
    {
      change_map(map->level + 1);
    }
}

static void button_width_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("[W]idth: %-3d"), (int) map->width);
  widget_draw(this);
}

static void button_height_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("[H]eight: %-3d"), (int) map->height);
  widget_draw(this);
}

static void button_time_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("[T]ime: %-4d"), (int) map->time);
  widget_draw(this);
}

static void button_change_activate(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      widget_set_long(this, "CHANGE", -1);
      widget_set_long(this, "CHANGE_TIMER", 0);
      map_is_dirty = 1;
    }
  else if(button == WIDGET_BUTTON_RIGHT)
    {
      widget_set_long(this, "CHANGE", 1);
      widget_set_long(this, "CHANGE_TIMER", 0);
      map_is_dirty = 1;
    }
}

static void button_change_release(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT || button == WIDGET_BUTTON_RIGHT)
    {
      widget_set_long(this, "CHANGE", 0);
      map_is_dirty = 1;
    }
}

static void button_gamespeed_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("GameSp[e]ed: %-2d"), (int) map->game_speed);
  widget_draw(this);
}

static void button_amebatime_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("[A]mebaTime: %-4d"), (int) map->ameba_time);
  widget_draw(this);
}

static void button_diamondsneeded_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("[D]iamondsNeeded: %-4d"), (int) map->diamonds_needed);
  widget_draw(this);
}

static void button_diamondscore_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("Diamond[S]core: %-4d"), (int) map->diamond_score);
  widget_draw(this);
}

static void button_timescore_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("TimeS[c]ore: %-4d"), (int) map->time_score);
  widget_draw(this);
}

static void change_map_width(int width_change)
{
  if(width_change == -1)
    {
      if(map->width > 10)
        {
          unsigned char * old;
          int old_w;
                            
          old = map->data;
          old_w = map->width;
          map->width--;

          map->data = malloc(map->width * map->height);
          assert(map->data != NULL);

          for(int y = 0; y < map->height; y++)
            for(int x = 0; x < map->width; x++)
              map->data[x + y * map->width] = old[x + y * old_w];
          free(old);

          map_is_dirty = 1;
        }
    }
  else if(width_change == 1)
    {
      if(map->width < 200)
        {
          unsigned char * old;
          int old_w;

          old   = map->data;
          old_w = map->width;
          map->width++;

          map->data = malloc(map->width * map->height);
          assert(map->data != NULL);

          for(int y = 0; y < map->height; y++)
            for(int x = 0; x < map->width; x++)
              if(x < map->width - 1)
                map->data[x + y * map->width] = old[x + y * old_w];
              else
                map->data[x + y * map->width] = MAP_SAND;
          free(old);

          map_is_dirty = 1;
        }
    }
}

static void change_map_height(int height_change)
{
  if(map->height + height_change >= 10 && map->height + height_change <= 200)
    {
      unsigned char * new;
      
      new = realloc(map->data, map->width * (map->height + height_change));
      assert(new != NULL);
      
      map->data = new;
      map->height += height_change;
      
      if(height_change > 0)
        for(int x = 0; x < map->width; x++)
          map->data[x + (map->height - 1) * map->width] = MAP_SAND;
      
      map_is_dirty = 1;
    }
}

static void button_glyphs(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED, int x, int y DG_UNUSED)
{
  int sel;

  sel = (x - 4) / 30;
  if(sel >= 0 && sel < glyphs_count)
    {
      selected_glyph = sel;
    }
}

#ifdef WITH_OPENGL
struct gfxbuf * glyphsbuf = NULL;
#endif

static void button_glyphs_draw(struct widget * this)
{
  SDL_Rect dr;
  int gcount;

  widget_draw(this);

  dr.x = widget_x(this) + 4;
  dr.y = widget_y(this) + 4;
  dr.w = 24;
  dr.h = 24;

#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      if(glyphsbuf == NULL)
        {
          glyphsbuf = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, GFXBUF_TEXTURE);
          assert(glyphsbuf != NULL);
          gfxbuf_resize(glyphsbuf, MAP_SIZEOF_ * 4);
          glyphsbuf->material = gfx_material_default();

          glyphsbuf->vertices = 0;
          for(int i = 0; glyphs[i] != MAP_SIZEOF_; i++)
            {
              if(glyphs[i] == MAP_PLAYER)
                gfx_glyph_render_frame(glyphsbuf, i * 30, 0, MAP_PLAYER_ARMOUR2, MOVE_NONE, 0);
              else if(glyphs[i] == MAP_AMEBA || glyphs[i] == MAP_SLIME)
                gfx_glyph_render_frame(glyphsbuf, i * 30, 0, glyphs[i], MOVE_UP, 0);
              else
                gfx_glyph_render_frame(glyphsbuf, i * 30, 0, glyphs[i], MOVE_NONE, 0);
            }
          gfxbuf_update(glyphsbuf, 0, glyphsbuf->vertices);
        }
    }
#endif

  gcount = 0;
  for(int i = 0; glyphs[i] != MAP_SIZEOF_; i++, gcount++)
    {
      if(selected_glyph == i)
        {
          uint8_t r, g, b;

          r = 0xff;
          g = 0xff;
          b = 0xff;

          gfx_draw_hline(dr.x - 1  + i * 30, dr.y - 1,  25, r, g, b, 0xff);
          gfx_draw_hline(dr.x - 1  + i * 30, dr.y + 24, 25, r, g, b, 0xff);
          gfx_draw_vline(dr.x - 1  + i * 30, dr.y - 1,  25, r, g, b, 0xff);
          gfx_draw_vline(dr.x + 24 + i * 30, dr.y - 1,  25, r, g, b, 0xff);

          gfx_draw_hline(dr.x - 2  + i * 30, dr.y - 2,  27, r, g, b, 0xff);
          gfx_draw_hline(dr.x - 2  + i * 30, dr.y + 25, 27, r, g, b, 0xff);
          gfx_draw_vline(dr.x - 2  + i * 30, dr.y - 2,  27, r, g, b, 0xff);
          gfx_draw_vline(dr.x + 25 + i * 30, dr.y - 2,  27, r, g, b, 0xff);
        }

#ifdef WITH_NONOPENGL
      if(!globals.opengl)
        {
          if(glyphs[i] == MAP_EMPTY)
            gfx_draw_glyph_frame(dr.x + i * 30, dr.y, MAP_EMPTY, MOVE_NONE, 0);
          else if(glyphs[i] == MAP_PLAYER)
            gfx_draw_glyph_frame(dr.x + i * 30, dr.y, MAP_PLAYER_ARMOUR2, MOVE_NONE, 0);
          else if(glyphs[i] == MAP_AMEBA || glyphs[i] == MAP_SLIME)
            gfx_draw_glyph_frame(dr.x + i * 30, dr.y, glyphs[i], MOVE_UP, 0);
          else
            gfx_draw_glyph_frame(dr.x + i * 30, dr.y, glyphs[i], MOVE_NONE, 0);
        }
#endif
    }

#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      gfx_set_current_glyph_set(0);

      gfxbuf_draw_init(glyphsbuf);
      gfxbuf_draw_at(glyphsbuf, dr.x, dr.y);
    }
#endif

  {
    const char * name;
    char buf[64];

    switch(glyphs[selected_glyph])
      {
      case MAP_EMPTY:             name = gettext("empty");             break;
      case MAP_SAND:              name = gettext("sand");              break;
      case MAP_PLAYER:            name = gettext("start location");    break;
      case MAP_BOULDER:           name = gettext("boulder");           break;
      case MAP_DIAMOND:           name = gettext("diamond");           break;
      case MAP_ENEMY1:            name = gettext("enemy1");            break;
      case MAP_ENEMY2:            name = gettext("enemy2");            break;
      case MAP_EXIT_LEVEL:        name = gettext("exit location");     break;
      case MAP_BRICK:             name = gettext("brick");             break;
      case MAP_BRICK_EXPANDING:   name = gettext("expanding brick");   break;
      case MAP_BRICK_MORPHER:     name = gettext("morphing brick");    break;
      case MAP_BRICK_UNBREAKABLE: name = gettext("unbreakable brick"); break;
      case MAP_AMEBA:             name = gettext("ameba");             break;
      case MAP_SLIME:             name = gettext("slime");             break;
      default:
        name = NULL;
      }

    snprintf(buf, sizeof buf, gettext("Selected[,.]: %s"), name);
    font_write(dr.x + gcount * 30, dr.y, buf);
  }
}


static void button_quit_activate(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  editor_quit = 1;
}

static void button_play_activate(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  play_test();
}


static void button_save_draw(struct widget * this)
{
  widget_set_string(this, "text", gettext("%sSa[v]e"), map_is_dirty ? "*" : "");
  widget_draw(this);
}

static void button_revert_activate(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  revert_map();
}

static void button_save_activate(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  if(globals.read_only == false)
    if(map != NULL)
      {
        map_save(map);
        map_is_dirty = 0;
      }
}

static void button_map_on_mouse_move(struct widget * this, int x, int y)
{
  int mx, my;

  mx = map->map_x + x / 24;
  my = map->map_y + y / 24;
  if(mx >= map->width)
    mx = map->width - 1;
  if(my >= map->height)
    my = map->height - 1;
  if(mx != map->player_target->x || my != map->player_target->y)
    {
      map->player_target->x = mx;
      map->player_target->y = my;
      if(widget_get_long(this, "UPDATE"))
        map_update();
    }
}

static void button_map_activate(struct widget * this, enum WIDGET_BUTTON button, int x, int y)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      widget_set_long(this, "UPDATE", 1);
      map_update();
    }
  else if(button == WIDGET_BUTTON_RIGHT)
    {
      int map_width, map_height;
      
      map_width = widget_width(uiobj_map) / 24;
      map_height = widget_height(uiobj_map) / 24;

      map->map_x = map->player_target->x - map_width / 2;
      map->map_y = map->player_target->y - map_height / 2;
      if(map->map_x > map->width - map_width)
        map->map_x = map->width - map_width;
      if(map->map_y > map->height - map_height)
        map->map_y = map->height - map_height;
      if(map->map_x < 0)
        map->map_x = 0;
      if(map->map_y < 0)
        map->map_y = 0;
      map->player_target->x = map->map_x + x / 24;
      map->player_target->y = map->map_y + y / 24;
    }
}

static void button_map_release(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      widget_set_long(this, "UPDATE", 0);
    }
}

static void button_is_intermission_release(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      map->is_intermission = widget_get_long(this, "checked");
      map_is_dirty = 1;
    }
}




static void map_update(void)
{
  if(map->player_target->x != map->start_x || map->player_target->y != map->start_y)
    if(map->player_target->x != map->exit_x || map->player_target->y != map->exit_y)
      {
        if(glyphs[selected_glyph] == MAP_PLAYER)
          { /* change start location */
            map->data[map->start_x + map->start_y * map->width] = MAP_EMPTY;
            map->start_x = map->player_target->x;
            map->start_y = map->player_target->y;
          }
        else if(glyphs[selected_glyph] == MAP_EXIT_LEVEL)
          { /* change exit location */
            map->data[map->exit_x + map->exit_y * map->width] = MAP_EMPTY;
            map->exit_x = map->player_target->x;
            map->exit_y = map->player_target->y;
          }
        
        if(map->data[map->player_target->x + map->player_target->y * map->width] != glyphs[selected_glyph])
          {
            map->data[map->player_target->x + map->player_target->y * map->width] = glyphs[selected_glyph];
          }

        map_is_dirty = 1;
      }
}

static void revert_map(void)
{
  if(map != NULL)
    init_map(NULL, map->level);
}

static void play_test(void)
{
  if(map != NULL)
    {
      struct gamedata * gr;
      struct widget * root;
      struct cave * cave;

      root = widget_root();
      widget_set_root(NULL);

      clear_area(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      cave = cave_get(globals.title_game_mode, map->cave_name);
      gr = game(cave, map->level, false, true, NULL, NULL);
      sfx_stop();
      clear_area(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      gfx_frame0();

      widget_set_root(root);

      assert(gr != NULL);
      assert(gr->map != NULL);
      if(gr != NULL)
        {
          if(gr->map != NULL)
            {
              int diamonds, enemy1, enemy2;

              diamonds = 0;
              enemy1 = 0;
              enemy2 = 0;
              for(int y = 0; y < gr->map->height; y++)
                for(int x = 0; x < gr->map->width; x++)
                  switch(gr->map->data[x + y * gr->map->width])
                    {
                    case MAP_DIAMOND: diamonds++; break;
                    case MAP_ENEMY1:  enemy1++;   break;
                    case MAP_ENEMY2:  enemy2++;   break;
                    }

              char t_score[32];
              char t_dc[32];
              char t_dl[32];
              char t_e1l[32];
              char t_e2l[32];
              char t_tl[32];

              snprintf(t_score, sizeof t_score, "%d", (int) gr->score);
              snprintf(t_dc,    sizeof t_dc,    "%d", (int) gr->diamonds);
              snprintf(t_dl,    sizeof t_dl,    "%d", diamonds);
              snprintf(t_e1l,   sizeof t_e1l,   "%d", enemy1);
              snprintf(t_e2l,   sizeof t_e2l,   "%d", enemy2);
              snprintf(t_tl,    sizeof t_tl,    "%d", (int) (gr->map->game_time / gr->map->frames_per_second));

              struct widget_factory_data wfd[] =
                {
                  { WFDT_STRING,  "score",              t_score },
                  { WFDT_STRING,  "diamonds_collected", t_dc    },
                  { WFDT_STRING,  "diamonds_left",      t_dl    },
                  { WFDT_STRING,  "enemy1_left",        t_e1l   },
                  { WFDT_STRING,  "enemy2_left",        t_e2l   },
                  { WFDT_STRING,  "time_left",          t_tl    },
                  { WFDT_SIZEOF_, NULL,                 NULL    }
              };
              struct widget * stats;
              
              stats = widget_factory(wfd, NULL, "playtesting_results");
              widget_new_message_dialog("Playtesting results", stats, NULL, NULL);

              gr->map = map_free(gr->map);
            }
          gr = gamedata_free(gr);
        }
    }
}


static bool setup_ui(void)
{
  bool rv;

  rv = false;
  ui_clear_handlers();
  if(ui_initialize())
    {
      struct widget * root;
      int y;
      
      y = 0;

      root = widget_new_root(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      assert(root != NULL);
      if(root != NULL)
        {
          struct
          {
            enum UIOBJ id;
            int x, y;
            ui_func_on_draw_t on_draw;
            ui_func_on_activate_t on_activate;
            ui_func_on_activate_t on_release;
            char * text;
          } default_buttons[] =
              {
                { UIOBJ_CAVE,              0, 0, NULL,                       NULL,                   button_cave_release,   ""                       },
                { UIOBJ_NONE,            150, 0, button_level_draw,          button_level_activate,  NULL,                  ""                       },
                { UIOBJ_WIDTH,           285, 0, button_width_draw,          button_change_activate, button_change_release, ""                       },
                { UIOBJ_HEIGHT,          405, 0, button_height_draw,         button_change_activate, button_change_release, ""                       },
                { UIOBJ_TIME,            535, 0, button_time_draw,           button_change_activate, button_change_release, ""                       },
                { UIOBJ_GAMESPEED,       650, 0, button_gamespeed_draw,      button_change_activate, button_change_release, ""                       },
                { UIOBJ_AMEBATIME,         0, 1, button_amebatime_draw,      button_change_activate, button_change_release, ""                       },
                { UIOBJ_DIAMONDSNEEDED,  180, 1, button_diamondsneeded_draw, button_change_activate, button_change_release, ""                       },
                { UIOBJ_DIAMONDSCORE,    400, 1, button_diamondscore_draw,   button_change_activate, button_change_release, ""                       },
                { UIOBJ_TIMESCORE,       605, 1, button_timescore_draw,      button_change_activate, button_change_release, ""                       },
                { UIOBJ_NONE,              0, 2, NULL,                       button_quit_activate,   NULL,                  gettext("[Q]uit")        },
                { UIOBJ_NONE,            100, 2, NULL,                       button_play_activate,   NULL,                  gettext("[P]lay")        },
                { UIOBJ_NONE,            200, 2, NULL,                       button_revert_activate, NULL,                  gettext("[R]evert Map")  },
                { UIOBJ_NONE,            350, 2, button_save_draw,           button_save_activate,   NULL,                  ""                       },
                { UIOBJ_NONE,            500, 2, NULL,                       NULL,                   NULL,                  gettext("Intermission:") },
                { UIOBJ_IS_INTERMISSION, 630, 2, NULL,                       NULL,          button_is_intermission_release, ""                       },
                { UIOBJ_NONE,             -1,-1, NULL,                       NULL,                   NULL,                  NULL                     }
              };
          int button_height;

          button_height = 0;
          for(int i = 0; default_buttons[i].x >= 0; i++)
            if(default_buttons[i].text != NULL)
              {
                struct widget * obj;
                int next_x;
                
                if(default_buttons[i + 1].x > 0)
                  next_x = default_buttons[i + 1].x;
                else
                  next_x = SCREEN_WIDTH - 1;

                if(default_buttons[i].id == UIOBJ_IS_INTERMISSION)
                  obj = widget_new_checkbox(root, default_buttons[i].x, default_buttons[i].y * button_height, map != NULL ? map->is_intermission : false);
                else if(i == 14)
                  obj = widget_new_text(root, default_buttons[i].x, default_buttons[i].y * button_height, default_buttons[i].text);
                else
                  obj = widget_new_button(root, default_buttons[i].x, default_buttons[i].y * button_height, default_buttons[i].text);
                assert(obj != NULL);

                if(default_buttons[i].id == UIOBJ_IS_INTERMISSION)
                  ;
                else
                  widget_set_width(obj, next_x - default_buttons[i].x - 4);

                if(button_height < widget_height(obj) + 2)
                  button_height = widget_height(obj) + 2;
              
                if(default_buttons[i].id != UIOBJ_NONE && default_buttons[i].id != UIOBJ_SIZEOF_)
                  uiobj[default_buttons[i].id] = obj;
                
                if(default_buttons[i].on_draw != NULL)
                  widget_set_on_draw(obj, default_buttons[i].on_draw);
                
                if(default_buttons[i].on_activate != NULL)
                  widget_set_on_activate(obj, default_buttons[i].on_activate);
                
                if(default_buttons[i].on_release != NULL)
                  widget_set_on_release(obj, default_buttons[i].on_release);
                
                if(i == 0)

                widget_delete_flags(obj, WF_ALIGN_CENTER);

                
                if(y < default_buttons[i].y * button_height)
                  y = default_buttons[i].y * button_height;
              }

          y += font_height() + 5;

          {
            int h;

            y += 10;
            if(font_height() > 26)
              h = font_height();
            else
              h = 26;
            h += 6;
            uiobj_glyphs = widget_new_button(root, 0, y, NULL);
            assert(uiobj_glyphs != NULL);
            widget_set_width(uiobj_glyphs, SCREEN_WIDTH);
            widget_set_height(uiobj_glyphs, h);
            widget_set_on_draw(uiobj_glyphs, button_glyphs_draw);
            widget_set_on_activate_at(uiobj_glyphs, button_glyphs);
            y += h;
          }
          y += 2;

          uiobj_map = widget_new_map(root, 0, y, SCREEN_WIDTH, (SCREEN_HEIGHT - y), NULL, globals.title_game_mode);
          assert(uiobj_map != NULL);
          widget_set_on_mouse_move(uiobj_map, button_map_on_mouse_move);
          widget_set_on_activate_at(uiobj_map, button_map_activate);
          widget_set_on_release(uiobj_map, button_map_release);

          rv = true;
        }
    }
  return rv;
}


static void button_cave_release(struct widget * this, enum WIDGET_BUTTON button)
{
  if(this != NULL)
    {
      int direction;
      
      if(button == WIDGET_BUTTON_LEFT)
        direction = -1;
      else
        direction = 1;


      char ** caves;

      caves = get_cave_names(true);
      if(caves != NULL)
        {
          int pos;

          pos = -1;
          for(int i = 0; pos == -1 && caves[i] != NULL; i++)
            if(!strcmp(caves[i], map->cave_name))
              pos = i;

          int newpos;

          newpos = -1;
          for(int i = 1; newpos == -1 && pos + i * direction >= 0 && caves[pos + i * direction] != NULL; i += direction)
            {
              struct cave * cave;

              cave = cave_get(GAME_MODE_CLASSIC, caves[pos + i * direction]);
              if(cave->editable == true)
                newpos = pos + i * direction;
            }
          
          if(newpos >= 0)
            init_map(caves[newpos], 1);
        }
      free_cave_names(caves);
    }
}
