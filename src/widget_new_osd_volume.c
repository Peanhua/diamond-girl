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

#include "widget.h"
#include "globals.h"
#include <libintl.h>

static void on_draw(struct widget * this);

struct widget * widget_new_osd_volume(void)
{
  struct widget * rv, * tmp;
  char buf[64];

  tmp = widget_find(widget_root(), "osd_volume");
  if(tmp != NULL)
    widget_delete(tmp);

  rv = widget_new_frame(widget_root(), 10, 10, 0, 0);
  //  widget_delete_flags(rv, WF_CAST_SHADOW | WF_DRAW_BORDERS);
  widget_set_string(rv, "id", "osd_volume");
  widget_set_ulong(rv, "timer", 30 * 3);
  widget_set_on_draw(rv, on_draw);

  snprintf(buf, sizeof buf, gettext("Volume: %d%%"), (int) globals.volume);
  tmp = widget_new_text(rv, 0, 0, buf);
  widget_set_width(rv, widget_width(tmp));
  widget_set_height(rv, widget_height(tmp));

  return rv;
}

static void on_draw(struct widget * this)
{
  int timer;

  timer = widget_get_ulong(this, "timer");
  if(timer > 0)
    {
      timer--;
      widget_set_ulong(this, "timer", timer);
      widget_draw(this);
    }
  else
    {
      widget_add_flags(this, WF_DESTROY);
    }
}

