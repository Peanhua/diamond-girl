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

#ifndef GC_H_
#define GC_H_

#include <stdbool.h>

enum GC_TYPE
  {
    GCT_GFXBUF,
    GCT_WIDGET,
    GCT_CAVE,
    GCT_THEME,
    GCT_SIZEOF_
  };

struct stack;

extern bool gc_initialize(void);
extern void gc_cleanup(void);

extern void           gc_new(enum GC_TYPE type, void * object);
extern void           gc_free(enum GC_TYPE type, void * object);
extern struct stack * gc_get_stack(enum GC_TYPE type);
extern void           gc_empty_stack(enum GC_TYPE type);


#endif
