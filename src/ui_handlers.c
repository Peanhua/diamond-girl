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

#include "ui.h"
#include "stack.h"

#include <assert.h>


static struct ui_handler handlers[UIC_SIZEOF_];

static struct stack * stack = NULL;

void ui_clear_handlers(void)
{
  for(int i = 0; i < UIC_SIZEOF_; i++)
    handlers[i].handler = NULL;
}

void ui_set_handler(enum UI_COMMAND command, ui_func_handler_t handler)
{
  handlers[command].handler = handler;
}


void ui_call_handler(enum UI_COMMAND command, bool pressed, SDL_Event * event)
{
  if(handlers[command].handler != NULL)
    handlers[command].handler(pressed, event);
}

void ui_push_handlers(void)
{
  if(stack == NULL)
    stack = stack_new();

  assert(stack != NULL);

  if(stack != NULL)
    {
      struct ui_handler * tmp;
      size_t s;

      s = sizeof handlers;
      tmp = malloc(s);
      assert(tmp != NULL);
      if(tmp != NULL)
        {
          memcpy(tmp, handlers, sizeof handlers);
          stack_push(stack, tmp);
        }
    }
}

void ui_pop_handlers(void)
{
  assert(stack != NULL);

  if(stack != NULL)
    {
      struct ui_handler * tmp;

      tmp = stack_pop(stack);
      assert(tmp != NULL);
      if(tmp != NULL)
        {
          memcpy(handlers, tmp, sizeof handlers);
          free(tmp);
        }
    }
}
