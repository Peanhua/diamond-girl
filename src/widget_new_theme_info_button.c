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

#include "gfx.h"
#include "gfx_glyph.h"
#include "gfx_material.h"
#include "gfxbuf.h"
#include "globals.h"
#include "themes.h"
#include "widget.h"
#include <assert.h>

static void on_theme_info_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_draw(struct widget * this);


struct widget * widget_new_theme_info_button(struct widget * parent, int x, int y, struct theme * theme)
{
  struct widget * button;

  button = widget_new_button(parent, x, y, NULL);
  assert(button != NULL);
  if(button != NULL)
    {
      widget_set_width(button,    24);
      widget_set_height(button,   24);
      //widget_delete_flags(button, WF_DRAW_BORDERS);
      //widget_set_ulong(button,    "alpha",     0x80);
      widget_set_theme_pointer(button, "theme", theme);
#ifdef WITH_OPENGL
      widget_set_pointer(button, "buffer", 'P', NULL);
#endif
      widget_set_on_draw(button, on_draw);
      widget_set_on_release(button, on_theme_info_clicked);
    }
  
  return button;
}

static void on_theme_info_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct theme * theme;

  theme = widget_get_theme_pointer(this, "theme");
  assert(theme != NULL);
  if(theme != NULL)
    widget_new_theme_info_window(theme);
}

static void on_draw(struct widget * this)
{
  struct theme * theme;

  theme = widget_get_theme_pointer(this, "theme");
  assert(theme != NULL);
  if(theme != NULL && theme->icon.glyph != MAP_SIZEOF_)
    {
      int x, y;

      x = widget_absolute_x(this);
      y = widget_absolute_y(this);
#ifdef WITH_OPENGL
      if(globals.opengl)
        {
          struct gfxbuf * buffer;

          buffer = widget_get_pointer(this, "buffer", 'P');
          if(buffer == NULL)
            {
              buffer = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, GFXBUF_TEXTURE | GFXBUF_BLENDING);
              if(buffer != NULL)
                {
                  gfxbuf_resize(buffer, 4);
                  buffer->material = gfx_material_new();
                  gfx_glyph_render_frame(buffer, 0, 0, theme->icon.glyph, theme->icon.direction, 0);
                  gfxbuf_update(buffer, 0, 4);
                  widget_set_pointer(this, "buffer", 'P', buffer);
                }
            }
          if(buffer != NULL)
            {
              gfx_set_current_glyph_set(0);

              gfxbuf_draw_init(buffer);
              gfxbuf_draw_at(buffer, x, y);
            }
        }
      else
#endif
        {
#ifdef WITH_NONOPENGL
          gfx_draw_glyph_frame(x, y, theme->icon.glyph, theme->icon.direction, 0);
#endif
        }
    }
}

