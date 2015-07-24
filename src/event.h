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

#ifndef EVENT_H_
#define EVENT_H_

#include <inttypes.h>

enum EVENT_TYPE
  {
    EVENT_TYPE_TRAIT_ACTIVATED,
    EVENT_TYPE_TRAIT_DEACTIVATED,

    EVENT_TYPE_QUEST_STATE_CHANGED,

    EVENT_TYPE_SIZEOF_
  };

typedef void (* event_callback_func_t)(void * user_data, int64_t value);

extern void events_initialize(void);
extern void events_cleanup(void);

extern void event_register(enum EVENT_TYPE event_type, event_callback_func_t callback, void * user_data);
extern void event_unregister(enum EVENT_TYPE event_type, event_callback_func_t callback, void * user_data);
extern void event_unregister_all(enum EVENT_TYPE event_type, void * user_data);

extern void event_trigger(enum EVENT_TYPE event_type, int64_t value);

#endif
