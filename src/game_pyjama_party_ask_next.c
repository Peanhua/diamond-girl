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
#include "game.h"
#include "ui.h"
#include "widget.h"
#include "widget_factory.h"
#include <assert.h>


static struct widget * window = NULL;

static void gppan_on_exit(bool pressed, SDL_Event * event);
static void gppan_on_button_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void gppan_on_save_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void gppan_on_quit_clicked(struct widget * this, enum WIDGET_BUTTON button);

static void ppn_on_exit(bool pressed, SDL_Event * event);
static void ppn_on_play_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void ppn_on_save_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void ppn_on_quit_clicked(struct widget * this, enum WIDGET_BUTTON button);

static void pyjama_party_next(struct gamedata * gamedata);

// "The girl is dead."
// "Level completed."
void game_pyjama_party_ask_next(struct gamedata * gamedata)
{
  window = NULL;
  if(gamedata->pyjama_party_girls_passed < gamedata->girls)
    {
      if(gamedata->pyjama_party_control == PARTYCONTROLLER_PLAYER || gamedata->pyjama_party_control == PARTYCONTROLLER_GIRL)
        { /* Ask the player how to continue the party. */
          struct widget * obj;
          struct widget_factory_data wfd[] =
            {
              { WFDT_ON_RELEASE, "on_button_clicked", gppan_on_button_clicked },
              { WFDT_ON_RELEASE, "on_save_clicked",   gppan_on_save_clicked   },
              { WFDT_ON_RELEASE, "on_quit_clicked",   gppan_on_quit_clicked   },
              { WFDT_SIZEOF_,    NULL,                NULL                    }
            };

          if(gamedata->ai != NULL)
            gamedata->ai = ai_free(gamedata->ai);

          window = widget_factory(wfd, NULL, "game_pyjama_party_ask_next");
          widget_center_on_parent(window);
          widget_set_pointer(window, "gamedata", gamedata);
          
          obj = widget_find(window, "gppan_yes");
          widget_set_ulong(obj, "party", PARTYCONTROLLER_PLAYER);

          obj = widget_find(window, "gppan_no");
          widget_set_ulong(obj, "party", PARTYCONTROLLER_GIRL);

          obj = widget_find(window, "gppan_partyrun");
          widget_set_ulong(obj, "party", PARTYCONTROLLER_PARTY);
      
          ui_push_handlers();
          ui_set_navigation_handlers();
          ui_set_handler(UIC_EXIT,   gppan_on_exit);
          ui_set_handler(UIC_CANCEL, gppan_on_exit);
        }
      else
        { /* Party on non-stop until the next level. */
          pyjama_party_next(gamedata);
        }
    }
  else
    { /* All girls has passed the level, next level. */
      pyjama_party_next(gamedata);
    }
}


static void gppan_on_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  assert(window != NULL);
  if(pressed == true)
    {
      struct gamedata * gamedata;

      gamedata = widget_get_pointer(window, "gamedata");
      ui_pop_handlers();
      gamedata->quit = true;
      window = widget_delete(window);
    }
}

static void gppan_on_button_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  enum PARTYCONTROLLER control;
  struct gamedata * gamedata;

  assert(window != NULL);
  control = widget_get_ulong(this, "party");
  gamedata = widget_get_pointer(window, "gamedata");

  ui_pop_handlers();
  window = widget_delete(window);

  game_set_pyjama_party_control(control);
  pyjama_party_next(gamedata);
}

static void gppan_on_save_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  assert(window != NULL);
  struct gamedata * gamedata;

  gamedata = widget_get_pointer(window, "gamedata");
  ui_pop_handlers();
  gamedata->quit = true;
  window = widget_delete(window);
}

static void gppan_on_quit_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  assert(window != NULL);
  struct gamedata * gamedata;

  gamedata = widget_get_pointer(window, "gamedata");
  ui_pop_handlers();
  gamedata->girls = 0;
  gamedata->quit = true;
  window = widget_delete(window);
}


static void pyjama_party_next(struct gamedata * gamedata)
{
  assert(window == NULL);

  if(gamedata->pyjama_party_girls_passed < gamedata->girls)
    { /* Next girl for the party. */
      gamedata->reset_map();
    }
  else if(gamedata->girls > 0)
    { /* Next pyjama party level. */
      struct widget_factory_data wfd[] =
        {
          { WFDT_ON_RELEASE, "on_play_clicked", ppn_on_play_clicked },
          { WFDT_ON_RELEASE, "on_save_clicked", ppn_on_save_clicked },
          { WFDT_ON_RELEASE, "on_quit_clicked", ppn_on_quit_clicked },
          { WFDT_SIZEOF_,    NULL,              NULL                }
        };

      game_set_pyjama_party_control(PARTYCONTROLLER_PLAYER);

      window = widget_factory(wfd, NULL, "game_pyjama_party_level_completed");
      widget_center_on_parent(window);
      widget_set_pointer(window, "gamedata", gamedata);

      ui_push_handlers();
      ui_set_navigation_handlers();
      ui_set_handler(UIC_EXIT,   ppn_on_exit);
      ui_set_handler(UIC_CANCEL, ppn_on_exit);
    }
}


static void ppn_on_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  assert(window != NULL);
  if(pressed == true)
    {
      struct gamedata * gamedata;

      gamedata = widget_get_pointer(window, "gamedata");
      ui_pop_handlers();
      gamedata->current_level++;
      gamedata->quit = true;
      window = widget_delete(window);
    }
}

static void ppn_on_play_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{ /* Proceed to the next level. */
  assert(window != NULL);
  struct gamedata * gamedata;

  gamedata = widget_get_pointer(window, "gamedata");
  ui_pop_handlers();
  gamedata->current_level++;
  gamedata->init_map();
  window = widget_delete(window);
}

static void ppn_on_save_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  assert(window != NULL);
  struct gamedata * gamedata;

  gamedata = widget_get_pointer(window, "gamedata");
  ui_pop_handlers();
  gamedata->current_level++;
  gamedata->quit = true;
  window = widget_delete(window);
}

static void ppn_on_quit_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  assert(window != NULL);
  struct gamedata * gamedata;

  gamedata = widget_get_pointer(window, "gamedata");
  ui_pop_handlers();
  gamedata->girls = 0;
  gamedata->quit = true;
  window = widget_delete(window);
}
