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

uint32_t widget_flags(struct widget * widget)
{
  return widget->flags_;
}


uint32_t widget_set_flags(struct widget * widget, uint32_t flags)
{
  widget->flags_ = flags;

  return widget->flags_;
}


uint32_t widget_add_flags(struct widget * widget, uint32_t flags)
{
  widget->flags_ |= flags;

  return widget->flags_;
}


uint32_t widget_delete_flags(struct widget * widget, uint32_t flags)
{
  widget->flags_ &= ~flags;

  return widget->flags_;
}

