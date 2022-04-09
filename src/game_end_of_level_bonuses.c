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

#include "game.h"
#include "girl.h"
#include "map.h"
#include "sfx.h"
#include "widget.h"

static int timeout_phase;


static void timeout(struct widget * this);


void game_end_of_level_bonuses(struct widget * window)
{
  timeout_phase = 0;
  widget_set_on_timeout(window, timeout, 0);
}


static void timeout(struct widget * this)
{
  struct
  {
    int    delay;
    char * widget_id;
  } program[] =
      {
        {  0, NULL                     }, /* NOP, so we don't have to access program outside (in set_timeout()) */
        { 15, "score_diamonds"         },
        { 15, "score_time"             },
        { 15, "score_greedy"           },
        { 15, "score_irongirl"         },
        { 20, "score_total"            },
        { 30, "diamond_score_alive"    },
        { 15, "diamond_score_diamonds" },
        { 15, "diamond_score_greedy"   },
        { 15, "diamond_score_irongirl" },
        { 20, "diamond_score"          },
        { 30, "other"                  },
        { -1, NULL                     }
      };

  bool dodelay;

  dodelay = true;
  if(program[timeout_phase].widget_id != NULL)
    {
      struct widget * w;

      w = widget_find(this, program[timeout_phase].widget_id);
      if(w != NULL)
        {
          struct gamedata * gamedata;
          
          gamedata = widget_get_gamedata_pointer(this, "gamedata");
          if(gamedata != NULL)
            {
              sfx_emit(SFX_DIAMOND_COLLECT, gamedata->map->girl->mob->x, gamedata->map->girl->mob->y);
              sfx_emit(SFX_NOISELESS_EXPLOSION, gamedata->map->girl->mob->x, gamedata->map->girl->mob->y);
            }

          widget_delete_flags(w, WF_HIDDEN);
        }
      else
        dodelay = false;
    }

  timeout_phase++;

  if(program[timeout_phase].delay >= 0)
    widget_set_on_timeout(this, timeout, dodelay == true ? program[timeout_phase].delay : 0);
}

