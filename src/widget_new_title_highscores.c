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

#include "widget.h"
#include "gfx.h"
#include "gfx_image.h"
#include "font.h"
#include "highscore.h"
#include "image.h"
#include "cave.h"

#include <assert.h>
#include <libintl.h>

static void draw(struct widget * this);
static void draw_row(struct widget * this);
static void on_highscore_row_clicked(struct widget * this, enum WIDGET_BUTTON button);

struct widget * widget_new_title_highscores(struct widget * parent, int x, int y, struct cave * cave)
{
  struct widget * obj;
  int amount;
  int width;

  amount = 20;
  width = 400;
  obj = widget_new_child(parent, x, y, width, 1);
  assert(obj != NULL);
  if(obj != NULL)
    {
      widget_set_on_draw(obj, draw);
      widget_set_ulong(obj, "type", WT_TITLE_HIGHSCORES);
      widget_set_flags(obj, 0);
      widget_set_ulong(obj, "first_highscore", 0);
      widget_set_long(obj, "selected_highscore", -1);
      widget_set_ulong(obj, "amount", amount);
      widget_set_pointer(obj, "cave", cave);
      obj->z_ += 1; /* Draw above its children. */

      struct widget * prev;

      prev = NULL;
      for(int i = 0; i < amount; i++)
        {
          struct widget * tmp;
          
          tmp = widget_new_button(obj, 0, (2 + i) * font_height(), "???");
          widget_set_width(tmp, width);
          widget_set_height(tmp, font_height());
          widget_set_ulong(tmp, "row", i);
          widget_set_on_draw(tmp, draw_row);
          widget_set_on_release(tmp, on_highscore_row_clicked);
          tmp->z_ -= 1; /* Draw below its parent. */

          if(prev != NULL)
            widget_set_navigation_updown(prev, tmp);
          prev = tmp;
        }
    }

  return obj;
}

static void draw(struct widget * this)
{
  /* highscore list */
  int amount; /* The number of entries per page. */
  char tbuf[128];
  int first_highscore;
  int x, y;
  int width;

  x                  = widget_absolute_x(this);
  y                  = widget_absolute_y(this);
  width              = widget_width(this);
  amount             = widget_get_ulong(this, "amount");
  first_highscore    = widget_get_ulong(this, "first_highscore");

  snprintf(tbuf, sizeof tbuf, gettext("Highscores %d -> %d:"), first_highscore + 1, (int) (first_highscore + amount));
  font_write(x + 10, y, tbuf);
  y += font_height();
  gfx_draw_hline(x + 0,   y, width - 20, 0x00, 0xff, 0x00, 0xff);
  gfx_draw_vline(x + 40,  y, (amount + 1) * font_height(),  0x00, 0xff, 0x00, 0xff);
  gfx_draw_vline(x + 130, y, (amount + 1) * font_height(),  0x00, 0xff, 0x00, 0xff);
  gfx_draw_vline(x + 195, y, (amount + 1) * font_height(),  0x00, 0xff, 0x00, 0xff);
  font_write(x + 10,  y, " #");
  font_write(x + 40,  y, gettext(" Score"));
  font_write(x + 130, y, gettext(" Level"));
  font_write(x + 195, y, gettext(" Timestamp"));
  y += font_height();
  gfx_draw_hline(x, y, width - 20, 0x00, 0xff, 0x00, 0xff);
}

static void draw_row(struct widget * this)
{
  /* highscore list */
  size_t size;
  struct highscore_entry ** entries;
  char tbuf[128];
  int x, y, width;
  int first_highscore;
  bool enabled;
  struct cave * cave;

  cave = widget_get_pointer(widget_parent(this), "cave");

  enabled            = widget_enabled(this);
  x                  = widget_absolute_x(this);
  y                  = widget_absolute_y(this);
  width              = widget_width(this);
  first_highscore    = widget_get_ulong(widget_parent(this), "first_highscore");

  entries = highscores_get(cave->highscores, &size);

  int cursor;
  time_t cursor_ts;

  cursor = 0;
  cursor_ts = 0;
  for(size_t i = 0; i < size; i++)
    if(entries[i]->timestamp > cursor_ts)
      {
        cursor_ts = entries[i]->timestamp;
        cursor = i;
      }

  int row, hind;

  row = widget_get_ulong(this, "row");
  hind = row + first_highscore;

  {
    uint8_t c_cursor[3]           = { 0x00, 0x00, 0xff };
    uint8_t c_selected[3]         = { 0x00, 0x40, 0x80 };
    uint8_t c_selected_noscore[3] = { 0x00, 0x20, 0x40 };
    uint8_t * c;
        
    c = NULL;
    
    if(cursor_ts > 0 && hind == cursor)
      c = c_cursor;
    
    if(c == NULL && enabled == true && widget_state(this) == WIDGET_STATE_FOCUSED)
      {
        if(hind < (int) size)
          c = c_selected;
        else
          c = c_selected_noscore;
      }
    
    if(c != NULL)
      {
        SDL_Rect r;
        
        r.x = x + 1;
        r.y = y + 1;
        r.w = width - 20;
        r.h = font_height() - 2;
        gfx_draw_rectangle(&r, c[0], c[1], c[2], 0x80);
      }
  }
      
  if(hind < (int) size)
    {
      {
        int tmpx;

        snprintf(tbuf, sizeof tbuf, "%3d", (int) hind + 1);
        tmpx = (40 - 2) - font_width(tbuf);
        font_write(x + tmpx, y, tbuf);
      }

      snprintf(tbuf, sizeof tbuf, " %6d", (int) entries[hind]->score);
      font_write(x + 40, y, tbuf);

      snprintf(tbuf, sizeof tbuf, " %3d", (int) entries[hind]->level);
      font_write(x + 130, y, tbuf);
	  
      struct tm * tm;
	  
      tm = localtime(&entries[hind]->timestamp);
      strftime(tbuf, sizeof tbuf, gettext(" %Y-%m-%d %H:%M"), tm);
      font_write(x + 195, y, tbuf);
    }
}


static void on_highscore_row_clicked(struct widget * this, enum WIDGET_BUTTON button)
{
  int first, row;

  first = widget_get_ulong(widget_parent(this), "first_highscore");
  row = widget_get_ulong(this, "row");
  widget_set_ulong(widget_parent(this), "selected_highscore", first + row);

  if(button == WIDGET_BUTTON_LEFT)
    { /* perform action (replay) */
      ui_func_on_activate_t f;

      f = widget_on_activate(widget_parent(this));
      if(f != NULL)
        f(widget_parent(this), button);
    }
  else
    { /* scroll */
      if(row < 10)
        { /* scroll up */
          if(first >= 20)
            first -= 20;
        }
      else
        { /* scroll down */
          size_t size;
          struct cave * cave;

          cave = widget_get_pointer(widget_parent(this), "cave");
          highscores_get(cave->highscores, &size);
          if(first + 20 < (int) size)
            first += 20;
        }
      widget_set_ulong(widget_parent(this), "first_highscore", first);
    }
}



