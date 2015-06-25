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

#ifndef GFX_IMAGE_H_
#define GFX_IMAGE_H_

#include <stdbool.h>

struct image;


/* Predefined images, obtained by calling gfx_image().
 * They are free'd during gfx_cleanup() and loaded on demand.
 */
enum GFX_IMAGE
  {
    GFX_IMAGE_CHECKBOX_ON,
    GFX_IMAGE_INTERMISSION,
    GFX_IMAGE_MAP_CURSOR,
    GFX_IMAGE_WIDGET_SELECT,
    GFX_IMAGE_LOGO,
    GFX_IMAGE_CLOSE_BUTTON,
    GFX_IMAGE_SCORE_BACKGROUND,
    GFX_IMAGE_SCORE_BACKGROUND_256,
    GFX_IMAGE_WM_ICON,
    GFX_IMAGE_PLAY_BUTTON,
    GFX_IMAGE_STOP_BUTTON,
    GFX_IMAGE_REWIND_BUTTON,
    GFX_IMAGE_FORWARD_BUTTON,
    GFX_IMAGE_EJECT_BUTTON,
    GFX_IMAGE_MODE_SWITCH_LEFT,
    GFX_IMAGE_MODE_SWITCH_UP,
    GFX_IMAGE_MODE_SWITCH_RIGHT,
    GFX_IMAGE_TRAIT_ADVENTURE_MODE,
    GFX_IMAGE_TRAIT_ADVENTURE_MODE_DISABLED,
    GFX_IMAGE_TRAIT_RIBBON,
    GFX_IMAGE_TRAIT_RIBBON_DISABLED,
    GFX_IMAGE_TRAIT_GREEDY,
    GFX_IMAGE_TRAIT_GREEDY_DISABLED,
    GFX_IMAGE_TRAIT_TIME_CONTROL,
    GFX_IMAGE_TRAIT_TIME_CONTROL_DISABLED,
    GFX_IMAGE_TRAIT_POWER_PUSH,
    GFX_IMAGE_TRAIT_POWER_PUSH_DISABLED,
    GFX_IMAGE_TRAIT_DIAMOND_PUSH,
    GFX_IMAGE_TRAIT_DIAMOND_PUSH_DISABLED,
    GFX_IMAGE_TRAIT_RECYCLER,
    GFX_IMAGE_TRAIT_RECYCLER_DISABLED,
    GFX_IMAGE_TRAIT_STARS1,
    GFX_IMAGE_TRAIT_STARS1_DISABLED,
    GFX_IMAGE_TRAIT_STARS2,
    GFX_IMAGE_TRAIT_STARS2_DISABLED,
    GFX_IMAGE_TRAIT_STARS3,
    GFX_IMAGE_TRAIT_STARS3_DISABLED,
    GFX_IMAGE_TRAIT_CHAOS,
    GFX_IMAGE_TRAIT_CHAOS_DISABLED,
    GFX_IMAGE_TRAIT_DYNAMITE,
    GFX_IMAGE_TRAIT_DYNAMITE_DISABLED,
    GFX_IMAGE_TRAIT_KEY,
    GFX_IMAGE_TRAIT_KEY_DISABLED,
    GFX_IMAGE_TRAIT_IRON_GIRL,
    GFX_IMAGE_TRAIT_IRON_GIRL_DISABLED,
    GFX_IMAGE_TRAIT_PYJAMA_PARTY,
    GFX_IMAGE_TRAIT_PYJAMA_PARTY_DISABLED,
    GFX_IMAGE_SIZEOF_
  };


extern bool           gfx_image_initialize(void);
extern void           gfx_image_cleanup(void);
extern struct image * gfx_image(enum GFX_IMAGE image_id);

#endif
