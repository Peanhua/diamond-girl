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

#include "diamond_girl.h"
#include "globals.h"
#include "game.h"
#include "highscore.h"
#include "playback.h"
#include "ui.h"
#include "cave.h"
#include <assert.h>
#include <libintl.h>


void settings_default(void)
{
#ifndef PROFILING
  size_t hs_size;
  char * msets[2] =
    {
      "/random",
      "a"
    };
  struct highscorelist * list;

  list = highscores_new();
  highscores_load(list, GAME_MODE_CLASSIC, msets[0]);
  highscores_get(list, &hs_size);
  if(hs_size == 0)
    { /* Populate the highscores with the tutorial playbacks. */
      for(int i = 0; i < 2; i++)
        {
          struct cave * cave;

          cave = cave_get(GAME_MODE_CLASSIC, msets[i]);

          list = highscores_new();
          highscores_load(list, GAME_MODE_CLASSIC, msets[i]);
          for(int j = 1; j <= 4; j++)
            {
              struct gamedata * gd;

              gd = gamedata_new(cave, false, 0);
              assert(gd != NULL);
              if(gd != NULL)
                {
                  char fn[128];
                    
                  gd->score = j;
                  snprintf(fn, sizeof fn, "data/tutorial%s-%d.dgp", cave_filename(msets[i]), j);
                  gd->playback = playback_load(get_data_filename(fn));
                  assert(gd->playback != NULL);
                  if(gd->playback != NULL)
                    {
                      snprintf(fn, sizeof fn, gettext("This is a demo of level %d."), j);
                      gd->traits = gd->playback->traits;
                      highscore_new(list, gd, fn);
                    }
                  gd = gamedata_free(gd);
                }
            }
          highscores_save(list, GAME_MODE_CLASSIC, msets[i]);
          list = highscores_free(list);

          /* Reload the highscores to work properly. */
          cave->highscores = highscores_new();
          highscores_load(cave->highscores, cave->game_mode, cave->name);
        }
    }
#endif
}
