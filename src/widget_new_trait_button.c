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
#include "traits.h"
#include "widget.h"
#include <assert.h>
#include <libintl.h>



struct widget * traitinfo;

static void on_trait_clicked(struct widget * this, enum WIDGET_BUTTON button);


struct widget * widget_new_trait_button(struct widget * parent, int x, int y, int width, int height, trait_t trait, bool always_enabled_image, bool controls)
{
  struct widget * rv;

  rv = widget_new_button(parent, x, y, NULL);
  assert(rv != NULL);
  if(rv != NULL)
    {
      widget_set_width(rv,  width);
      widget_set_height(rv, height);
      widget_delete_flags(rv, WF_BACKGROUND | WF_CAST_SHADOW);

      if(trait > 0)
        {
          struct trait * traitdata;

          traitdata = trait_get(trait);
          if(traitdata != NULL)
            {
              widget_set_on_release(rv, on_trait_clicked);
              widget_set_ulong(rv, "trait", trait);
              widget_set_ulong(rv, "controls", controls);

              struct image * img;

              if(always_enabled_image == true || traits_get_active() & trait)
                img = gfx_image(traitdata->image_id);
              else
                img = gfx_image(traitdata->disabled_image_id);
          
              if(img != NULL)
                {
                  widget_add_flags(rv, WF_SCALE_RAW_IMAGE);
                  widget_set_image_pointer(rv, "raw_image", img);
                }
            }
        }
    }

  return rv;
}



static void on_trait_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  trait_t trait;
  bool controls;

  trait    = widget_get_ulong(this, "trait");
  controls = widget_get_ulong(this, "controls");

  if(trait > 0)
    widget_new_trait_info_window(trait, controls);
}
