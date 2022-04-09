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
#include "map.h"
#include "girl.h"
#include <assert.h>

static bool is_glyph_slippery(enum MAP_GLYPH glyph);


bool ai_is_passable_direction(struct map * map, enum MOVE_DIRECTION direction, bool cautious_mode, bool panic_mode)
{
  int xmod, ymod;

  switch(direction)
    {
    default:
    case MOVE_NONE:  xmod =  0; ymod =  0; break;
    case MOVE_UP:    xmod =  0; ymod = -1; break;
    case MOVE_LEFT:  xmod = -1; ymod =  0; break;
    case MOVE_RIGHT: xmod =  1; ymod =  0; break;
    case MOVE_DOWN:  xmod =  0; ymod =  1; break;
    }

  return ai_is_passable(map, map->girl->mob->x, map->girl->mob->y, xmod, ymod, cautious_mode, panic_mode, true);
}

bool ai_is_passable_mfp_cb(struct map * map, int x, int y, int xmod, int ymod)
{
  return ai_is_passable(map, x, y, xmod, ymod, true, false, true);
}

bool ai_is_passable_mfp_cb_cautiousoff(struct map * map, int x, int y, int xmod, int ymod)
{
  return ai_is_passable(map, x, y, xmod, ymod, false, false, true);
}

bool ai_is_passable(struct map * map, int x, int y, int xmod, int ymod, bool cautious_mode, bool panic_mode, bool avoid_dead_ends)
{
  bool passable;

  passable = false;

  x += xmod;
  y += ymod;

  if(x >= 0 && x < map->width && y >= 0 && y < map->height)
    {
      enum MAP_GLYPH glyph;

      glyph = map->data[x + y * map->width];
      if(glyph == MAP_EXIT_LEVEL && map->exit_open == true)
        {
          passable = true;
        }
      else if(glyph == MAP_EMPTY   ||
              glyph == MAP_PLAYER  ||
              glyph == MAP_SAND    ||
              glyph == MAP_DIAMOND ||
              glyph == MAP_SMOKE   ||
              glyph == MAP_SMOKE1  ||
              glyph == MAP_SMOKE2  ||
              glyph == MAP_BOULDER)
        {

          passable = true;

          if(y > 0)
            { /* Check that there is no diamond or boulder falling down where we're going to go. */
              enum MAP_GLYPH above;
              
              above = map->data[x + (y - 1) * map->width];
              if(above == MAP_DIAMOND_FALLING || above == MAP_BOULDER_FALLING)
                passable = false;
            }

          if(passable == true && glyph == MAP_BOULDER)
            if(y - ymod - 1 >= 0)
              { /* If we're pushing boulder, make sure we're not getting crushed by a falling diamond or boulder in current location, because pushing takes time.
                 * Todo: determine if we can make the push this turn, in that case we would continue pushing.
                 */
                enum MAP_GLYPH above;

                above = map->data[x - xmod + (y - ymod - 1) * map->width];
                if(above == MAP_DIAMOND_FALLING || above == MAP_BOULDER_FALLING)
                  passable = false;
              }
      
          /* Push, adding the xmod again to the x-coordinate, we can see if there's space behind. */
          if(passable == true)
            if(glyph == MAP_BOULDER)
              {
                if(panic_mode == false)
                  {
                    if(ymod == 0)
                      {
                        if(xmod != 0)
                          if(x + xmod >= map->width || map->data[x + xmod + y * map->width] != MAP_EMPTY)
                            passable = false; /* There was no space behind it. */
                      }
                    else
                      passable = false; /* Can't push up or down. */
                  }
                else
                  passable = false; /* There is no time to push in panic mode. */
              }
        }
  
      if(passable == true && cautious_mode == true && glyph != MAP_EXIT_LEVEL)
        { /* Check that there are no enemies next to the location where we're going to go.
           * Ignore the enemy avoidance if we're in fact moving into the exit level.
           */
          for(int iy = -1; passable == true && iy <= 1; iy++)
            for(int ix = -1; passable == true && ix <= 1; ix++)
              if((iy == 0 && ix != 0) || (iy != 0 && ix == 0))
                if(x + ix >= 0 && x + ix < map->width && y + iy >= 0 && y + iy < map->height)
                  {
                    enum MAP_GLYPH enemy;
                    
                    enemy = map->data[x + ix + (y + iy) * map->width];
                    if(enemy == MAP_ENEMY1 || enemy == MAP_ENEMY2)
                      passable = false;
                  }
        }

      if(passable == true && avoid_dead_ends == true)
        { /* Avoid dead-ends when moving side-ways, if the only exit is going to get blocked.
           * For example, don't move to the left (#=brick, B=boulder, P=player, S=sand, space=empty:
           *  #B
           * # PS
           *  #S
           */
          if(xmod != 0 && ymod == 0 && y > 0)
            {
              enum MAP_GLYPH above;

              above = map->data[x - xmod + (y - 1) * map->width];

              bool boulder_coming;

              boulder_coming = false;

              if(above == MAP_BOULDER || above == MAP_BOULDER_FALLING)
                { /* The obvious case */
                  boulder_coming = true;
                }
              else if(above == MAP_EMPTY)
                {
                  enum MAP_GLYPH above2;

                  if(y > 1)
                    above2 = map->data[x - xmod + (y - 2) * map->width];
                  else
                    above2 = MAP_BORDER;

                  if(above2 == MAP_BOULDER || above2 == MAP_BOULDER_FALLING)
                    { /* Two up */
                      boulder_coming = true;
                    }
                  else
                    {
                      enum MAP_GLYPH above1_side, side;
                      int tmpx;

                      tmpx = x - xmod - xmod;
                      if(tmpx >= 0 && tmpx < map->width)
                        {
                          above1_side = map->data[tmpx + (y - 1) * map->width];
                          side = map->data[tmpx + y * map->width];
                        }
                      else
                        {
                          above1_side = MAP_BORDER;
                          side = MAP_BORDER;
                        }
                      
                      if(above1_side == MAP_BOULDER || above1_side == MAP_BOULDER_FALLING)
                        if(is_glyph_slippery(side) == true)
                          { /* A boulder is coming from the up&side, by slipping over whatever is in the side. */
                            boulder_coming = true;
                          }
                    }
                }
              
              if(boulder_coming == true)
                { /* A boulder is coming down to this location, so we see if it'd block this location.
                   * We do this by first checking if we could push it out of our way.
                   */
                  int tmpx;
              
                  tmpx = x - xmod - xmod;
                  if(tmpx < 0 || tmpx >= map->width || map->data[tmpx + y * map->width] != MAP_EMPTY)
                    { /* Nope, we couldn't push our way out, so we check if the boulder would fall further down. */
                      enum MAP_GLYPH below;

                      if(y + 1 < map->height)
                        below = map->data[x - xmod + (y + 1) * map->width];
                      else
                        below = MAP_BORDER;
                
                      if(below != MAP_EMPTY  &&
                         below != MAP_PLAYER &&
                         below != MAP_SMOKE  &&
                         below != MAP_SMOKE1 &&
                         below != MAP_SMOKE2 &&
                         below != MAP_BRICK_MORPHER)
                        { /* The boulder would block our exit because the boulder can't drop down, and there's no space behind it.
                           * Next we check our new location, if it's all blocked, we won't be going there.
                           */
                          if(ai_is_passable(map, x, y, xmod,  0, cautious_mode, panic_mode, false) == false &&
                             ai_is_passable(map, x, y,    0, -1, cautious_mode, panic_mode, false) == false &&
                             ai_is_passable(map, x, y,    0,  1, cautious_mode, panic_mode, false) == false)
                            {
                              passable = false;
                            }
                        }
                    }
                }
            }
        }

      if(passable == true && glyph != MAP_EXIT_LEVEL)
        { /* Avoid explosion zones. */
          for(int iy = -1; passable == true && iy <= 1; iy++)
            if(iy + y > 0 && iy + y < map->height) /* We make sure there can be a boulder/diamond above this y coordinate,
                                                    * when this is extended to ameba, this needs to be changed. */
              for(int ix = -1; passable == true && ix <= 1; ix++)
                if(ix + x >= 0 && ix + x < map->width)
                  if(ix != 0 || iy != 0) /* No use to check the middle one, because that's where we're going to. */
                    {
                      enum MAP_GLYPH enemy;

                      enemy = map->data[x + ix + (y + iy) * map->width];
                      if(enemy == MAP_ENEMY1 || enemy == MAP_ENEMY2)
                        { /* Something to explode nearby, we now check only if there's a boulder or diamond falling on top of it,
                           * but we should also check for ameba.
                           * The validity of y coordinate has already been checked for one above this.
                           */
                          enum MAP_GLYPH exploder;

                          exploder = map->data[x + ix + (y + iy - 1) * map->width];
                          if(exploder == MAP_BOULDER_FALLING || exploder == MAP_DIAMOND_FALLING)
                            { /* This combination would explode next tick, and we're in the zone. */
                              passable = false;
                            }
                        }
                    }
        }
    }

  return passable;
}



static bool is_glyph_slippery(enum MAP_GLYPH glyph)
{
  bool rv;

  if(glyph == MAP_BOULDER         ||
     glyph == MAP_BOULDER_FALLING ||
     glyph == MAP_DIAMOND         ||
     glyph == MAP_DIAMOND_FALLING ||
     glyph == MAP_BRICK)
    rv = true;
  else
    rv = false;

  return rv;
}
