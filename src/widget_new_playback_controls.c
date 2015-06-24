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
#include "widget.h"
#include "gfx_image.h"
#include "image.h"
#include "map.h"
#include "ui.h"
#include <assert.h>


static void on_play_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_stop_clicked(struct widget * this, enum WIDGET_BUTTON button);
/*
static void on_rewind_clicked(struct widget * this, enum WIDGET_BUTTON button);
*/
static void on_forward_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_eject_clicked(struct widget * this, enum WIDGET_BUTTON button);

struct widget * widget_new_playback_controls(struct widget * parent, int x, int y, char const * const title)
{
  struct widget * pc;

  pc = widget_new_window(parent, 10 + 42 * 4 + 10, 24 + 40 + 10, title);
  assert(pc != NULL);
  if(pc != NULL)
    {
      widget_set_x(pc, x);
      widget_set_y(pc, y);

      int bx, by;
      struct image * img;
      struct widget * prev;

      prev = NULL;

      bx = 10;
      by = 24;
      img = gfx_image(GFX_IMAGE_PLAY_BUTTON);
      assert(img != NULL);
      if(img != NULL)
        {
          struct widget * button;

          button = widget_new_button(pc, bx, by, NULL);
          widget_set_image(button, "image", GFX_IMAGE_PLAY_BUTTON);
          widget_set_width(button, img->content_width + 2);
          widget_set_height(button, img->content_height + 2);
          widget_set_on_release(button, on_play_clicked);
          bx += widget_width(button);
          prev = button;
        }

      img = gfx_image(GFX_IMAGE_STOP_BUTTON);
      assert(img != NULL);
      if(img != NULL)
        {
          struct widget * button;

          button = widget_new_button(pc, bx, by, NULL);
          widget_set_image(button, "image", GFX_IMAGE_STOP_BUTTON);
          widget_set_width(button, img->content_width + 2);
          widget_set_height(button, img->content_height + 2);
          widget_set_on_release(button, on_stop_clicked);
          widget_set_focus(button);
          bx += widget_width(button);
          if(prev != NULL)
            widget_set_navigation_leftright(prev, button);
          prev = button;
        }
      /*
      img = gfx_image(GFX_IMAGE_REWIND_BUTTON);
      assert(img != NULL);
      if(img != NULL)
        {
          struct widget * button;

          button = widget_new_button(pc, bx, by, NULL);
          widget_set_image(button, "image", GFX_IMAGE_REWIND_BUTTON);
          widget_set_width(button, img->content_width + 2);
          widget_set_height(button, img->content_height + 2);
          widget_set_on_release(button, on_rewind_clicked);
          bx += widget_width(button);
          if(prev != NULL)
            widget_set_navigation_leftright(prev, button);
          prev = button;
        }
      */
      img = gfx_image(GFX_IMAGE_FORWARD_BUTTON);
      assert(img != NULL);
      if(img != NULL)
        {
          struct widget * button;

          button = widget_new_button(pc, bx, by, NULL);
          widget_set_image(button, "image", GFX_IMAGE_FORWARD_BUTTON);
          widget_set_width(button, img->content_width + 2);
          widget_set_height(button, img->content_height + 2);
          widget_set_on_release(button, on_forward_clicked);
          bx += widget_width(button);
          if(prev != NULL)
            widget_set_navigation_leftright(prev, button);
          prev = button;
        }

      img = gfx_image(GFX_IMAGE_EJECT_BUTTON);
      assert(img != NULL);
      if(img != NULL)
        {
          struct widget * button;

          button = widget_new_button(pc, bx, by, NULL);
          widget_set_image(button, "image", GFX_IMAGE_EJECT_BUTTON);
          widget_set_width(button, img->content_width + 2);
          widget_set_height(button, img->content_height + 2);
          widget_set_on_release(button, on_eject_clicked);
          bx += widget_width(button);
          if(prev != NULL)
            widget_set_navigation_leftright(prev, button);
          prev = button;
        }
    }

  return pc;
}


static void on_play_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct map * map;

  map = widget_get_pointer(widget_parent(this), "map");
  if(map != NULL)
    if(map->game_paused == true)
      map->toggle_pause(map, false);
}

static void on_stop_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct map * map;

  map = widget_get_pointer(widget_parent(this), "map");
  if(map != NULL)
    if(map->game_paused == false)
      map->toggle_pause(map, true);
}
/*
static void on_rewind_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
}
*/

static void on_forward_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct map * map;

  map = widget_get_pointer(widget_parent(this), "map");
  if(map != NULL)
    map->fast_forwarding = true;
}

static void on_eject_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  ui_call_handler(UIC_CANCEL, true, NULL);
}

