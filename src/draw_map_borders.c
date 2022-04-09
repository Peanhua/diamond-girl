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

#include "diamond_girl.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "map.h"
#include "gfx_glyph.h"
#include "gfx_image.h"
#include "gfx_material.h"
#include "globals.h"
#include "traits.h"
#include <assert.h>

#ifdef WITH_OPENGL
static void generate(struct map * map, int width, int height);
#endif


void draw_map_borders(struct map * map, enum GAME_MODE game_mode DG_UNUSED_WITHOUT_OPENGL, trait_t active_traits DG_UNUSED_WITHOUT_OPENGL, int topleft_x, int topleft_y, int width, int height)
{
  if(map->borders)
    {
      if(map->glyph_set != 0)
        gfx_set_current_glyph_set(0);

      if(globals.opengl)
        {
#ifdef WITH_OPENGL
          if(map->borderbuf == NULL)
            generate(map, width, height);

          gfxbuf_draw_init(map->borderbuf);
          gfxbuf_draw_at(map->borderbuf, topleft_x, topleft_y);

          if(game_mode == GAME_MODE_ADVENTURE || game_mode == GAME_MODE_PYJAMA_PARTY)
            {
              struct
              {
                trait_t        trait_id;
                enum GFX_IMAGE image_id;
              } traits[] =
                  {
                    { TRAIT_RIBBON,         GFX_IMAGE_TRAIT_RIBBON         },
                    { TRAIT_GREEDY,         GFX_IMAGE_TRAIT_GREEDY         },
                    { TRAIT_TIME_CONTROL,   GFX_IMAGE_TRAIT_TIME_CONTROL   },
                    { TRAIT_POWER_PUSH,     GFX_IMAGE_TRAIT_POWER_PUSH     },
                    { TRAIT_DIAMOND_PUSH,   GFX_IMAGE_TRAIT_DIAMOND_PUSH   },
                    { TRAIT_CHAOS,          GFX_IMAGE_TRAIT_CHAOS          },
                    { TRAIT_DYNAMITE,       GFX_IMAGE_TRAIT_DYNAMITE       },
                    { TRAIT_QUESTS,         GFX_IMAGE_TRAIT_QUESTS         },
                    { TRAIT_ALL,            GFX_IMAGE_SIZEOF_              }
                  };
              int x;

              x = topleft_x + 2;

              for(int i = 0; i < TRAIT_SIZEOF_; i++)
                if(traits_sorted[i] != TRAIT_QUESTS || game_mode == GAME_MODE_ADVENTURE) // No quests trait visible in pyjama party mode.
                  if(active_traits & traits_sorted[i])
                    {
                      bool found;
                  
                      found = false;
                      for(int j = 0; found == false && traits[j].trait_id != TRAIT_ALL; j++)
                        if(traits[j].trait_id == traits_sorted[i])
                          {
                            found = true;

                            struct image * img;
                            
                            img = gfx_image(traits[j].image_id);
                            if(img != NULL)
                              {
                                gfx2d_draw_image_scaled(x, topleft_y - 24 + 2, img, 20, 20);
                                x += 24;
                              }
                          }
                    }
              
              enum GFX_IMAGE image_id;

              image_id = GFX_IMAGE_SIZEOF_;
              switch(game_mode)
                {
                case GAME_MODE_CLASSIC:
                  break;
                case GAME_MODE_ADVENTURE:
                  image_id = GFX_IMAGE_TRAIT_ADVENTURE_MODE;
                  break;
                case GAME_MODE_PYJAMA_PARTY:
                  image_id = GFX_IMAGE_TRAIT_PYJAMA_PARTY;
                  break;
                }

              x = topleft_x - 24 + 2;
              if(image_id != GFX_IMAGE_SIZEOF_)
                gfx2d_draw_image_scaled(x, topleft_y - 24 + 2, gfx_image(image_id), 20, 20);
              if(globals.iron_girl_mode)
                gfx2d_draw_image_scaled(x, topleft_y + 2, gfx_image(GFX_IMAGE_TRAIT_IRON_GIRL), 20, 20);
            }
#endif
        }
      else
        {
#ifdef WITH_NONOPENGL
          for(int y = 0; y < height + 2; y++)
            {
              SDL_Rect r;
      
              r.x = topleft_x - 24;
              r.y = topleft_y - 24 + y * 24;
              r.w = 24;
              r.h = 24;
              gfx_draw_glyph_frame(r.x, r.y, MAP_BORDER, MOVE_NONE, 0);
              r.x = topleft_x + width * 24;
              gfx_draw_glyph_frame(r.x, r.y, MAP_BORDER, MOVE_NONE, 0);
            }
          for(int x = 0; x < width; x++)
            {
              SDL_Rect r;
      
              r.x = topleft_x + x * 24;
              r.y = topleft_y - 24;
              r.w = 24;
              r.h = 24;
              gfx_draw_glyph_frame(r.x, r.y, MAP_BORDER, MOVE_NONE, 0);
              r.y = topleft_y + height * 24;
              gfx_draw_glyph_frame(r.x, r.y, MAP_BORDER, MOVE_NONE, 0);
            }
#endif
        }
    }
}


#ifdef WITH_OPENGL
static void generate(struct map * map, int width, int height)
{
  assert(map->borderbuf == NULL);
  map->borderbuf = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, GFXBUF_TEXTURE);
  assert(map->borderbuf != NULL);
  if(map->borderbuf != NULL)
    {
      gfxbuf_resize(map->borderbuf, 4 * ((width + 2 + height + 2) * 2 + TRAIT_SIZEOF_));
      map->borderbuf->material = gfx_material_default();
      map->borderbuf->vertices = 0;

      for(int y = -1; y < height + 1; y++)
        {
          int dy;

          dy = y * 24;
          gfx_glyph_render_frame(map->borderbuf,        -24, dy, MAP_BORDER, MOVE_NONE, 0);
          gfx_glyph_render_frame(map->borderbuf, width * 24, dy, MAP_BORDER, MOVE_NONE, 0);
        }
      for(int x = 0; x < width; x++)
        {
          int dx;

          dx = x * 24;
          gfx_glyph_render_frame(map->borderbuf, dx,         -24, MAP_BORDER, MOVE_NONE, 0);
          gfx_glyph_render_frame(map->borderbuf, dx, height * 24, MAP_BORDER, MOVE_NONE, 0);
        }

      gfxbuf_update(map->borderbuf, 0, map->borderbuf->vertices);
    }
}
#endif

