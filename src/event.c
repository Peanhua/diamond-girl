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

#include "event.h"
#include "stack.h"
#include <stdio.h>

static struct stack * callbacks[EVENT_TYPE_SIZEOF_];

void events_initialize(void)
{
  for(int i = 0; i < EVENT_TYPE_SIZEOF_; i++)
    callbacks[i] = stack_new();
}


void events_cleanup(void)
{
  for(int i = 0; i < EVENT_TYPE_SIZEOF_; i++)
    callbacks[i] = stack_free(callbacks[i]);
}



void event_register(enum EVENT_TYPE event_type, event_callback_func_t callback)
{
  stack_push(callbacks[event_type], callback);
}


void event_unregister(enum EVENT_TYPE event_type, event_callback_func_t callback)
{
  stack_pull(callbacks[event_type], callback);
}


void event_trigger(enum EVENT_TYPE event_type, int value)
{
  for(unsigned int i = 0; i < callbacks[event_type]->size; i++)
    {
      event_callback_func_t cb;

      cb = callbacks[event_type]->data[i];
      if(cb != NULL)
        cb(value);
    }
}
