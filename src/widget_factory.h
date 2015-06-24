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

#ifndef WIDGET_FACTORY_H_
#define WIDGET_FACTORY_H_

enum WIDGET_FACTORY_DATA_TYPE
  {
    WFDT_CAVE,            /* struct cave *       */
    WFDT_MAP,             /* struct map *        */
    WFDT_ON_ACTIVATE_AT,  /* ui_on_activate_at_t */
    WFDT_ON_ACTIVATE,     /* ui_on_activate_t    */
    WFDT_ON_DRAW,         /* ui_on_draw_t        */
    WFDT_ON_FOCUS,        /* ui_on_focus_t       */
    WFDT_ON_MOUSE_MOVE,   /* ui_on_mouse_move_t  */
    WFDT_ON_RELEASE,      /* ui_on_activate_t    */
    WFDT_ON_UNLOAD,       /* ui_on_unload_t      */
    WFDT_STRING,          /* char *              */
    WFDT_SIZEOF_
  };

struct widget_factory_data
{
  enum WIDGET_FACTORY_DATA_TYPE type;
  char *                        name;
  void *                        value;
};

extern struct widget * widget_factory(struct widget_factory_data * data, struct widget * parent, const char * filename);

#endif
