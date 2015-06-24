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
#include "font.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "gfx_glyph.h"
#include "globals.h"
#include "title.h"
#include "texture.h"
#include "image.h"
#include "random.h"
#include "gfx_image.h"
#include "map.h"
#include "girl.h"

#include <assert.h>

static void draw_map_cursor(struct map * map, int topleft_x, int topleft_y, int width, int height);

#ifdef WITH_OPENGL
static void draw_map_opengl(struct map * map, enum GAME_MODE game_mode, int topleft_x, int topleft_y, int width, int height, enum MOVE_DIRECTION ameba_state);
#endif

#ifdef WITH_NONOPENGL
static void draw_map_non_opengl(struct map * map, int topleft_x, int topleft_y, int width, int height, enum MOVE_DIRECTION ameba_state);
#endif

void draw_map(struct map * map, enum GAME_MODE game_mode, trait_t active_traits, int topleft_x, int topleft_y, int width, int height)
{
  gfx_gl_log("%s:%s: map", __FILE__, __FUNCTION__);
  if(globals.map_tilting == true)
    topleft_y += map->tilt;

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      glScissor(topleft_x, SCREEN_HEIGHT - topleft_y - height * 24 - 1, width * 24, height * 24);
      glEnable(GL_SCISSOR_TEST);
#endif
    }
#ifdef WITH_NONOPENGL
  else
    {
      SDL_Rect r;
      
      r.x = topleft_x;
      r.y = topleft_y;
      r.w = width * 24;
      r.h = height * 24;
      SDL_SetClipRect(gfx_game_screen(), &r);
    }
#endif

  enum MOVE_DIRECTION amstate;
  
  if(game_mode == GAME_MODE_CLASSIC)
    amstate = MOVE_UP;
  else if(map->ameba_blocked == -1 || map->ameba_blocked == 0)
    amstate = MOVE_UP;
  else if(map->ameba_blocked_timer > map->frames_per_second * 3)
    amstate = MOVE_UP;
  else if(map->ameba_blocked_timer > map->frames_per_second * 1)
    amstate = MOVE_RIGHT;
  else
    amstate = MOVE_DOWN;

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      draw_map_opengl(map, game_mode, topleft_x, topleft_y, width, height, amstate);
#endif
    }
#ifdef WITH_NONOPENGL
  else
    draw_map_non_opengl(map, topleft_x, topleft_y, width, height, amstate);
#endif

#ifdef WITH_OPENGL
  bool texturechanged;

  texturechanged = false;
  if(globals.opengl && (game_mode == GAME_MODE_ADVENTURE || game_mode == GAME_MODE_PYJAMA_PARTY))
    if(map->girl->mob->alive && map->girl->mob->chat_bubble_timer > 0)
      {
        int x, y, w, h;

        w = font_width(map->girl->mob->chat_bubble);
        h = font_height();

        x = topleft_x + map->map_fine_x + (map->girl->mob->x - map->map_x    ) * 24 + 12 - w / 2;
        y = topleft_y + map->map_fine_y + (map->girl->mob->y - map->map_y - 1) * 24;

	x += map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 0];
	y += map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width) + 1];

        SDL_Rect r;

        r.x = x - 4;
        r.y = y - 2;
        r.w = w + 4 * 2;
        r.h = h + 2 * 2;
        gfx_draw_rectangle(&r, 0x00, 0x00, 0x00, 0x80);

        font_write(x, y, map->girl->mob->chat_bubble);
        texturechanged = true;
      }
#endif

  if(map->level_start_anim_on > 0 && map->is_intermission == true)
    {
      struct image * img;
      
      img = gfx_image(GFX_IMAGE_INTERMISSION);
      if(img != NULL)
        {
          int x, y;
          
          x = 20 + get_rand(SCREEN_WIDTH - img->width - 40);
          y = 20 + get_rand(SCREEN_WIDTH - img->height - 40);
          gfx2d_draw_image(x, y, img);
        }
    }

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      glDisable(GL_SCISSOR_TEST);
#endif
    }
#ifdef WITH_NONOPENGL
  else
    SDL_SetClipRect(gfx_game_screen(), NULL);
#endif


  gfx_gl_log("%s:%s: map cursor", __FILE__, __FUNCTION__);
  draw_map_cursor(map, topleft_x, topleft_y, width, height);
  if(texturechanged == true)
    gfx_set_current_glyph_set(0);
  gfx_gl_log("%s:%s: map borders", __FILE__, __FUNCTION__);
  draw_map_borders(map, game_mode, active_traits, topleft_x, topleft_y, width, height);

  if(game_mode == GAME_MODE_ADVENTURE || game_mode == GAME_MODE_PYJAMA_PARTY || globals.smooth_classic_mode == true)
    if(map->girl->mob->moved_over_glyph != MAP_SIZEOF_)
      {
	int8_t * p;
            
	p = &map->move_offsets[2 * (map->girl->mob->x + map->girl->mob->y * map->width)];
	if(abs(p[0]) < 12 && abs(p[1]) < 12)
	  map->girl->mob->moved_over_glyph = MAP_SIZEOF_;
      }
}

#ifdef WITH_NONOPENGL
static void draw_map_non_opengl(struct map * map, int topleft_x, int topleft_y, int width, int height, enum MOVE_DIRECTION ameba_state)
{
  bool stop_diamond_blink; /* If true, then stop the diamond blinking for diamonds that are currently blinking. */

  if(gfx_get_frame(MAP_DIAMOND) == gfx_get_maxframe(MAP_DIAMOND) - 1)
    stop_diamond_blink = true;
  else
    stop_diamond_blink = false;

  gfx_set_current_glyph_set(map->glyph_set);

  for(int y = 0 - 1; y < height; y++)
    for(int x = 0 - 1; x < width; x++)
      {
        int dx, dy;
        char m;

        dx = topleft_x + x * 24 + map->map_fine_x;
        dy = topleft_y + y * 24 + map->map_fine_y;

        if(map->map_x + x >= 0 && map->map_x + x < map->width && 
           map->map_y + y >= 0 && map->map_y + y < map->height)
          {
            m = map->data[map->map_x + x + (map->map_y + y) * map->width];
          }
        else
          m = MAP_ILLEGAL;

        switch((enum MAP_GLYPH) m)
          {
          case MAP_SMOKE2:
            gfx_draw_glyph_frame(dx, dy, MAP_SMOKE, MOVE_NONE, 0);
            break;
          case MAP_SMOKE1:
            gfx_draw_glyph_frame(dx, dy, MAP_SMOKE, MOVE_NONE, 1);
            break;
          case MAP_EXTRA_LIFE_ANIM:
          case MAP_SMOKE:
          case MAP_PLAYER_ARMOUR0:
          case MAP_PLAYER_ARMOUR1:
          case MAP_PLAYER_ARMOUR2:
            break;
          case MAP_BORDER:
          case MAP_BRICK:
            gfx_draw_glyph(dx, dy, MAP_BRICK, MOVE_NONE);
            break;
          case MAP_BRICK_MORPHER:
            if(map->in_editor == true || map->morpher_is_on)
              gfx_draw_glyph(dx, dy, MAP_BRICK_MORPHER, MOVE_NONE);
            else
              gfx_draw_glyph(dx, dy, MAP_BRICK, MOVE_NONE);
            break;
          case MAP_BRICK_EXPANDING:
            if(map->in_editor == true)
              gfx_draw_glyph(dx, dy, MAP_BRICK_EXPANDING, MOVE_NONE);
            else
              gfx_draw_glyph(dx, dy, MAP_BRICK, MOVE_NONE);
            break;
          case MAP_BRICK_UNBREAKABLE:
            gfx_draw_glyph(dx, dy, MAP_BRICK_UNBREAKABLE, MOVE_NONE);
            break;
          case MAP_EMPTY:
            if(map->extra_life_anim > 0)
              gfx_draw_glyph(dx, dy, MAP_EXTRA_LIFE_ANIM, MOVE_NONE);
            else
              gfx_draw_glyph(dx, dy, MAP_EMPTY, MOVE_NONE);
            break;
          case MAP_ILLEGAL:
            if(map->background_type == 0)
              gfx_draw_glyph(dx, dy, MAP_ILLEGAL, MOVE_NONE);
            else
              gfx_draw_glyph(dx, dy, MAP_EMPTY, MOVE_NONE);
            break;
          case MAP_SAND:
            gfx_draw_glyph(dx, dy, MAP_SAND, MOVE_NONE);
            break;
          case MAP_BOULDER:
          case MAP_BOULDER_FALLING:
            gfx_draw_glyph(dx, dy, MAP_BOULDER, MOVE_NONE);
            break;
          case MAP_DIAMOND:
          case MAP_DIAMOND_FALLING:
            {
              enum MOVE_DIRECTION d;
              
              d = map->move_directions[map->map_x + x + (map->map_y + y) * map->width];
              gfx_draw_glyph(dx, dy, MAP_DIAMOND, d);
              if(stop_diamond_blink)
                map->move_directions[map->map_x + x + (map->map_y + y) * map->width] = MOVE_NONE;
            }
            break;
          case MAP_PLAYER:
            {
              enum MAP_GLYPH g;
              int mdir;

              if(map->in_editor == false)
                {
                  if(map->girl->mob->armour == 0)
                    g = MAP_PLAYER_ARMOUR0;
                  else if(map->girl->mob->armour == 1)
                    g = MAP_PLAYER_ARMOUR1;
                  else /* if(map->girl->mob->armour == 2)*/
                    g = MAP_PLAYER_ARMOUR2;

                  if(map->girl->mob->direction != MOVE_NONE || map->girl->mob->animation == 0)
                    mdir = map->girl->mob->direction;
                  else
                    {
                      switch(map->girl->mob->animation)
                        {
                        default:
                        case 1: mdir = MOVE_NONE + 1; break;
                        case 2: mdir = MOVE_DOWN;     break;
                        }
                    }
                }
              else
                {
                  g = MAP_PLAYER_ARMOUR2;
                  mdir = MOVE_NONE;
                }
              gfx_draw_glyph(dx, dy, g, mdir);
            }
            break;
          case MAP_AMEBA:
            gfx_draw_glyph(dx, dy, MAP_AMEBA, ameba_state);
            break;
          case MAP_SLIME:
            gfx_draw_glyph(dx, dy, MAP_SLIME, MOVE_UP);
            break;
          case MAP_ENEMY1:
            gfx_draw_glyph(dx, dy, MAP_ENEMY1, MOVE_NONE);
            break;
          case MAP_ENEMY2:
            gfx_draw_glyph(dx, dy, MAP_ENEMY2, MOVE_NONE);
            break;
          case MAP_EXIT_LEVEL:
            gfx_draw_glyph(dx, dy, MAP_EXIT_LEVEL, MOVE_NONE);
            break;
          case MAP_SIZEOF_:
            break;
          }
      }
}
#endif


#ifdef WITH_OPENGL

static void draw_map_opengl(struct map * map, enum GAME_MODE game_mode, int topleft_x, int topleft_y, int width, int height, enum MOVE_DIRECTION ameba_state)
{
  int ppf; /* pixels per frame */
  bool stop_diamond_blink; /* If true, then stop the diamond blinking for diamonds that are currently blinking. */

  assert(globals.opengl);
  assert(map != NULL);
  assert(width  < 40);
  assert(height < 40);

  if(map->drawbuf == NULL)
    { /* Allocate buffer big enough to hold the largest possible amount of data. */
      map->drawbuf = gfxbuf_new(GFXBUF_DYNAMIC, GL_QUADS, GFXBUF_TEXTURE);
      if(map->drawbuf != NULL)
        gfxbuf_resize(map->drawbuf, 40 * 40 * 4);
    }
  assert(map->drawbuf != NULL);

  ppf = map->frames_per_second / map->game_speed - 1;
  if(ppf < 1)
    ppf = 1;
  
  gfx_colour(map->display_colour[0],
             map->display_colour[1],
             map->display_colour[2],
             map->display_colour[3]);
  map->drawbuf->vertices = 0;

  if(gfx_get_frame(MAP_DIAMOND) == gfx_get_maxframe(MAP_DIAMOND) - 1)
    stop_diamond_blink = true;
  else
    stop_diamond_blink = false;

  gfx_set_current_glyph_set(map->glyph_set);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  /* Draw empty space first. */
  if(map->background_type == 0 || map->extra_life_anim > 0)
    for(int y = 0 - 1; y < height; y++)
      for(int x = 0 - 1; x < width; x++)
        if(map->map_x + x >= 0 && map->map_x + x < map->width && 
           map->map_y + y >= 0 && map->map_y + y < map->height)
          {
            char m;
            
            m = map->data[map->map_x + x + (map->map_y + y) * map->width];

            if(m == MAP_EMPTY   || /* m == MAP_EXTRA_LIFE_ANIM || */
               m == MAP_PLAYER  ||
               m == MAP_BOULDER || m == MAP_BOULDER_FALLING ||
               m == MAP_DIAMOND || m == MAP_DIAMOND_FALLING ||
               m == MAP_ENEMY1  || m == MAP_ENEMY2  ||
               m == MAP_SMOKE   || m == MAP_SMOKE1 || m == MAP_SMOKE2)
              {
                if(map->background_type == 0)
                  gfx_glyph_render(map->drawbuf, x * 24, y * 24, MAP_EMPTY, MOVE_NONE);
                if(map->extra_life_anim > 0)
                  gfx_glyph_render(map->drawbuf, x * 24, y * 24, MAP_EXTRA_LIFE_ANIM, MOVE_NONE);
              }
            if(globals.opengl_max_draw_map_vertices > 0 && globals.opengl_max_draw_map_vertices < map->drawbuf->vertices)
              {
                gfxbuf_update(map->drawbuf, 0, map->drawbuf->vertices);
                gfxbuf_draw_at(map->drawbuf, topleft_x + map->map_fine_x, topleft_y + map->map_fine_y);
                map->drawbuf->vertices = 0;
              }
          }

  /* Draw the map.
   * Need to loop over a bit larger area, so that the move_offsets get updated.
   */
  for(int y = 0 - 1 - 1; y < height + 1; y++)
    for(int x = 0 - 1 - 1; x < width + 1; x++)
      {
	char m;
        int bgx, bgy, dx, dy;
                
        dx = x * 24;
        dy = y * 24;

        bgx = dx;
        bgy = dy;

	if(map->map_x + x >= 0 && map->map_x + x < map->width && 
	   map->map_y + y >= 0 && map->map_y + y < map->height)
          {
            m = map->data[map->map_x + x + (map->map_y + y) * map->width];

            if(map->in_editor == false)
              if(game_mode == GAME_MODE_ADVENTURE || game_mode == GAME_MODE_PYJAMA_PARTY || globals.smooth_classic_mode == true)
                if(m == MAP_PLAYER  ||
                   m == MAP_BOULDER || m == MAP_BOULDER_FALLING ||
                   m == MAP_DIAMOND || m == MAP_DIAMOND_FALLING ||
                   m == MAP_ENEMY1  ||
                   m == MAP_ENEMY2 )
                  {
                    int8_t * p;
                  
                    p = &map->move_offsets[2 * (map->map_x + x + (map->map_y + y) * map->width)];

                    for(int i = 0; i < 2; i++)
                      if(p[i] <= -ppf)
                        p[i] += ppf;
                      else if(p[i] >= ppf)
                        p[i] -= ppf;
                      else
                        p[i] = 0;

                    dx += p[0];
                    dy += p[1];
                  }
          }
	else
          {
            m = MAP_ILLEGAL;
          }

        if(y >= -1 && y < height && x >= -1 && x < width)
          switch((enum MAP_GLYPH) m)
            {
            case MAP_SMOKE2:
              gfx_glyph_render_frame(map->drawbuf, dx, dy, MAP_SMOKE, MOVE_NONE, 0);
              break;
            case MAP_SMOKE1:
              gfx_glyph_render_frame(map->drawbuf, dx, dy, MAP_SMOKE, MOVE_NONE, 1);
              break;
            case MAP_EXTRA_LIFE_ANIM:
            case MAP_SMOKE:
            case MAP_PLAYER_ARMOUR0:
            case MAP_PLAYER_ARMOUR1:
            case MAP_PLAYER_ARMOUR2:
              assert(0);
              break;
            case MAP_BORDER:
            case MAP_BRICK:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_BRICK, MOVE_NONE);
              break;
            case MAP_BRICK_MORPHER:
              if(map->in_editor == true || map->morpher_is_on)
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_BRICK_MORPHER, MOVE_NONE);
              else
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_BRICK, MOVE_NONE);
              break;
            case MAP_BRICK_EXPANDING:
              if(map->in_editor == true)
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_BRICK_EXPANDING, MOVE_NONE);
              else
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_BRICK, MOVE_NONE);
              break;
            case MAP_BRICK_UNBREAKABLE:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_BRICK_UNBREAKABLE, MOVE_NONE);
              break;
            case MAP_EMPTY:
              /* This was already drawn earlier. */
              break;
            case MAP_ILLEGAL:
              if(map->background_type == 0)
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_ILLEGAL, MOVE_NONE);
              break;
            case MAP_SAND:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_SAND, MOVE_NONE);
              break;
            case MAP_BOULDER:
            case MAP_BOULDER_FALLING:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_BOULDER, MOVE_NONE);
              break;
            case MAP_DIAMOND:
            case MAP_DIAMOND_FALLING:
              {
                enum MOVE_DIRECTION d;

                d = map->move_directions[map->map_x + x + (map->map_y + y) * map->width];
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_DIAMOND, d);
                if(stop_diamond_blink)
                  map->move_directions[map->map_x + x + (map->map_y + y) * map->width] = MOVE_NONE;
              }
              break;
            case MAP_PLAYER:
              if(map->in_editor == true)
                gfx_glyph_render(map->drawbuf, dx, dy, MAP_PLAYER_ARMOUR2, MOVE_NONE);
              else
                {
                  if(map->girl->mob->moved_over_glyph != MAP_SIZEOF_)
                    if(map->girl->mob->moved_over_glyph != MAP_EMPTY)
                      gfx_glyph_render(map->drawbuf, bgx, bgy, map->girl->mob->moved_over_glyph, MOVE_NONE);
                  if(map->girl->mob->alive == true)
                    {
                      enum MAP_GLYPH g;
                      int mdir;
                    
                      if(map->girl->mob->armour == 0)
                        g = MAP_PLAYER_ARMOUR0;
                      else if(map->girl->mob->armour == 1)
                        g = MAP_PLAYER_ARMOUR1;
                      else /* if(map->girl->mob->armour == 2)*/
                        g = MAP_PLAYER_ARMOUR2;
                    
                      if(map->girl->mob->direction != MOVE_NONE || map->girl->mob->animation == 0)
                        mdir = map->girl->mob->direction;
                      else
                        {
                          switch(map->girl->mob->animation)
                            {
                            default:
                            case 1: mdir = MOVE_NONE + 1; break;
                            case 2: mdir = MOVE_DOWN;     break;
                            }
                        }
                      gfx_glyph_render(map->drawbuf, dx, dy, g, mdir);
                    }
                }
              break;
            case MAP_AMEBA:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_AMEBA, ameba_state);
              break;
            case MAP_SLIME:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_SLIME, MOVE_UP);
              break;
            case MAP_ENEMY1:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_ENEMY1, MOVE_NONE);
              break;
            case MAP_ENEMY2:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_ENEMY2, MOVE_NONE);
              break;
            case MAP_EXIT_LEVEL:
              gfx_glyph_render(map->drawbuf, dx, dy, MAP_EXIT_LEVEL, MOVE_NONE);
              break;
            case MAP_SIZEOF_:
              break;
            }

        if(globals.opengl_max_draw_map_vertices > 0 && globals.opengl_max_draw_map_vertices < map->drawbuf->vertices)
          {
            gfxbuf_update(map->drawbuf, 0, map->drawbuf->vertices);
            gfxbuf_draw_at(map->drawbuf, topleft_x + map->map_fine_x, topleft_y + map->map_fine_y);
            map->drawbuf->vertices = 0;
          }
      }

  if(map->drawbuf->vertices > 0)
    {
      gfxbuf_update(map->drawbuf, 0, map->drawbuf->vertices);
      gfxbuf_draw_at(map->drawbuf, topleft_x + map->map_fine_x, topleft_y + map->map_fine_y);
    }

  glDisable(GL_BLEND);
  GFX_GL_ERROR();
  glDisable(GL_TEXTURE_2D);
  GFX_GL_ERROR();
}



/* WITH_OPENGL */
#endif





static void draw_map_cursor(struct map * map, int topleft_x, int topleft_y, int width, int height)
{
  if(globals.ai_hilite_current_target)
    if(map->player_target != NULL)
      {
        int hilite_x, hilite_y;

        hilite_x = (map->player_target->x - map->map_x);
        hilite_y = (map->player_target->y - map->map_y);
        if(hilite_x >= 0 && hilite_x < width + 1 && hilite_y >= 0 && hilite_y < height + 1)
          {
            hilite_x = hilite_x * 24 + topleft_x + map->map_fine_x - 4;
            hilite_y = hilite_y * 24 + topleft_y + map->map_fine_y - 4;

            if(globals.opengl)
              {
                struct image * image;

                image = gfx_image(GFX_IMAGE_MAP_CURSOR);
                assert(image != NULL);
                if(image != NULL)
                  gfx2d_draw_image(hilite_x, hilite_y, image);
              }
            else
              {
                gfx_draw_hline(hilite_x,      hilite_y,      23, 0x00, 0xff, 0x00, 0xff);
                gfx_draw_hline(hilite_x,      hilite_y + 23, 23, 0x00, 0xff, 0x00, 0xff);
                gfx_draw_vline(hilite_x,      hilite_y,      23, 0x00, 0xff, 0x00, 0xff);
                gfx_draw_vline(hilite_x + 23, hilite_y,      23, 0x00, 0xff, 0x00, 0xff);
              }
          }
      }
}
