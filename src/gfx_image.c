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

#include "diamond_girl.h"
#include "gfx_image.h"
#include "image.h"
#include "texture.h"
#include "globals.h"
#include <assert.h>
#include <SDL_rotozoom.h>

static struct
{
  enum GFX_IMAGE const image_id;
  struct image *       image;
  char const * const   filename;
  bool const           alpha;
  bool const           texturize;
  bool const           mipmapping;
  bool const           greyscale;
  bool const           localized;
} images[GFX_IMAGE_SIZEOF_] =
  {
    { GFX_IMAGE_CHECKBOX_ON,                   NULL, "gfx/checkbox_on",           true,  true, false, false, false },
    { GFX_IMAGE_INTERMISSION,                  NULL, "gfx/intermission",          true,  true,  true, false, true  },
    { GFX_IMAGE_MAP_CURSOR,                    NULL, "gfx/map_cursor",            true,  true,  true, false, false },
    { GFX_IMAGE_WIDGET_SELECT,                 NULL, "gfx/widget_select",         true,  true, false, false, false },
    { GFX_IMAGE_LOGO,                          NULL, "gfx/logo",                 false, false, false, false, false },
    { GFX_IMAGE_CLOSE_BUTTON,                  NULL, "gfx/close_button",          true,  true, false, false, false },
    { GFX_IMAGE_SCORE_BACKGROUND,              NULL, "gfx/game_score",           false,  true, false, false, false },
    { GFX_IMAGE_SCORE_BACKGROUND_256,          NULL, "gfx/game_score",           false,  true, false, false, false },
    { GFX_IMAGE_WM_ICON,                       NULL, "gfx/desktop-icon",         false, false, false, false, false },
    { GFX_IMAGE_PLAY_BUTTON,                   NULL, "gfx/play",                  true,  true, false, false, false },
    { GFX_IMAGE_STOP_BUTTON,                   NULL, "gfx/stop",                  true,  true, false, false, false },
    { GFX_IMAGE_REWIND_BUTTON,                 NULL, "gfx/rewind",                true,  true, false, false, false },
    { GFX_IMAGE_FORWARD_BUTTON,                NULL, "gfx/forward",               true,  true, false, false, false },
    { GFX_IMAGE_EJECT_BUTTON,                  NULL, "gfx/eject",                 true,  true, false, false, false },
    { GFX_IMAGE_MODE_SWITCH_LEFT,              NULL, "gfx/mode_switch_left",      true,  true, false, false, false },
    { GFX_IMAGE_MODE_SWITCH_UP,                NULL, "gfx/mode_switch_up",        true,  true, false, false, false },
    { GFX_IMAGE_MODE_SWITCH_RIGHT,             NULL, "gfx/mode_switch_right",     true,  true, false, false, false },
    { GFX_IMAGE_TRAIT_ADVENTURE_MODE,          NULL, "gfx/trait_adventure_mode",  true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_ADVENTURE_MODE_DISABLED, NULL, "gfx/trait_adventure_mode",  true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_RIBBON,                  NULL, "gfx/trait_ribbon",          true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_RIBBON_DISABLED,         NULL, "gfx/trait_ribbon",          true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_GREEDY,                  NULL, "gfx/trait_greedy",          true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_GREEDY_DISABLED,         NULL, "gfx/trait_greedy",          true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_TIME_CONTROL,            NULL, "gfx/trait_time_control",    true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_TIME_CONTROL_DISABLED,   NULL, "gfx/trait_time_control",    true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_POWER_PUSH,              NULL, "gfx/trait_power_push",      true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_POWER_PUSH_DISABLED,     NULL, "gfx/trait_power_push",      true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_DIAMOND_PUSH,            NULL, "gfx/trait_diamond_push",    true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_DIAMOND_PUSH_DISABLED,   NULL, "gfx/trait_diamond_push",    true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_RECYCLER,                NULL, "gfx/trait_recycler",        true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_RECYCLER_DISABLED,       NULL, "gfx/trait_recycler",        true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_STARS1,                  NULL, "gfx/trait_stars1",          true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_STARS1_DISABLED,         NULL, "gfx/trait_stars1",          true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_STARS2,                  NULL, "gfx/trait_stars2",          true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_STARS2_DISABLED,         NULL, "gfx/trait_stars2",          true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_STARS3,                  NULL, "gfx/trait_stars3",          true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_STARS3_DISABLED,         NULL, "gfx/trait_stars3",          true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_CHAOS,                   NULL, "gfx/trait_chaos",           true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_CHAOS_DISABLED,          NULL, "gfx/trait_chaos",           true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_DYNAMITE,                NULL, "gfx/trait_dynamite",        true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_DYNAMITE_DISABLED,       NULL, "gfx/trait_dynamite",        true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_KEY,                     NULL, "gfx/trait_key",             true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_KEY_DISABLED,            NULL, "gfx/trait_key",             true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_IRON_GIRL,               NULL, "gfx/trait_iron_girl",       true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_IRON_GIRL_DISABLED,      NULL, "gfx/trait_iron_girl",       true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_PYJAMA_PARTY,            NULL, "gfx/trait_pyjama_party",    true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_PYJAMA_PARTY_DISABLED,   NULL, "gfx/trait_pyjama_party",    true,  true,  true,  true, false },
    { GFX_IMAGE_TRAIT_QUESTS,                  NULL, "gfx/trait_quests",          true,  true,  true, false, false },
    { GFX_IMAGE_TRAIT_QUESTS_DISABLED,         NULL, "gfx/trait_quests",          true,  true,  true,  true, false }
  };

bool gfx_image_initialize(void)
{
  for(int i = 0; i < GFX_IMAGE_SIZEOF_; i++)
    {
      images[i].image = NULL;
      assert(images[i].image_id == (enum GFX_IMAGE) i);
    }

  return true;
}


void gfx_image_cleanup(void)
{
  for(int i = 0; i < GFX_IMAGE_SIZEOF_; i++)
    if(images[i].image != NULL)
      images[i].image = image_free(images[i].image);
}


struct image * gfx_image(enum GFX_IMAGE image_id)
{
  struct image * image;

  image = images[image_id].image;
  if(image == NULL)
    {
      char fn[1024];
      
      if(images[image_id].localized == true)
        {
          snprintf(fn, sizeof fn, "%s-%s.png", images[image_id].filename, globals.language);
          image = image_load(get_data_filename(fn), images[image_id].alpha);
        }

      if(image == NULL)
        {
          snprintf(fn, sizeof fn, "%s.png", images[image_id].filename);
          image = image_load(get_data_filename(fn), images[image_id].alpha);
        }

      if(image_id == GFX_IMAGE_SCORE_BACKGROUND_256)
        {
          SDL_Surface * tmp;

          assert(image->height <= 256);
          tmp = zoomSurface(image->sdl_surface, (float) (256 + 1) / (float) image->width, 1.0f, 1);
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              struct image * ni;

              ni = image_load_from_surface(tmp, images[image_id].alpha);
              assert(ni != NULL);
              if(ni != NULL)
                {
                  image_free(image);
                  image = ni;
                }
            }
        }

      images[image_id].image = image;

      if(image != NULL)
        {
          if(images[image_id].greyscale == true)
            image_to_greyscale(image);

#ifdef WITH_OPENGL
          if(globals.opengl)
            {
              if(globals.opengl_power_of_two_textures == true)
                {
                  int origw, origh, w, h;
                  
                  origw = image->width;
                  origh = image->height;
                  w = round_to_power_of_two(origw);
                  h = round_to_power_of_two(origh);
                  
                  if(w != origw || h != origh)
                    image_expand(image, w, h);
                }
              
              if(images[image_id].texturize == true)
                image_to_texture(image, images[image_id].alpha, images[image_id].mipmapping, true);
            }
#endif
        }
    }

  assert(image != NULL);

  return image;
}
