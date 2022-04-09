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

#include "diamond_girl.h"
#include "ui.h"
#include "widget.h"

static void process(SDL_Event * event);


void ui_process_events(bool wait_infinitely)
{
  SDL_Event event;

  while(SDL_PollEvent(&event))
    process(&event);

  if(wait_infinitely == true)
    if(SDL_WaitEvent(&event))
      process(&event);
}


static void process(SDL_Event * event)
{
  if(event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_KEYDOWN || event->type == SDL_JOYBUTTONDOWN || event->type == SDL_QUIT)
    ui_set_last_user_action(time(NULL));

  if(event->type == SDL_MOUSEMOTION)
    {
      ui_process_mouse_move(event->motion.x, event->motion.y);
    }
  else if(event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
    {
      enum WIDGET_BUTTON b;
      
      if(event->button.button == SDL_BUTTON_LEFT)
        b = WIDGET_BUTTON_LEFT;
      else if(event->button.button == SDL_BUTTON_MIDDLE)
        b = WIDGET_BUTTON_MIDDLE;
      else if(event->button.button == SDL_BUTTON_RIGHT)
        b = WIDGET_BUTTON_RIGHT;
      else
        b = WIDGET_BUTTON_LEFT;

      int released;

      if(event->type == SDL_MOUSEBUTTONDOWN)
        released = 0;
      else /* if(event->type == SDL_MOUSEBUTTONUP) */
        released = 1;

      ui_process_mouse_button(event->button.x, event->button.y, b, released);
    }
  else if(event->type == SDL_QUIT)
    {
      ui_call_handler(UIC_EXIT, 1, event);
    }

  ui_call_bound_handler(event);

  if(event->type == SDL_KEYDOWN)
    ui_call_handler(UIC_CATCH_ALL_KEYBOARD, true, event);
  else if(event->type == SDL_KEYUP)
    ui_call_handler(UIC_CATCH_ALL_KEYBOARD, false, event);
  else if(event->type == SDL_JOYAXISMOTION)
    {
      if(event->jaxis.value != 0)
        ui_call_handler(UIC_CATCH_ALL_JOYSTICKS, true, event);
      else
        ui_call_handler(UIC_CATCH_ALL_JOYSTICKS, true, event);
    }
  else if(event->type == SDL_JOYBUTTONDOWN)
    ui_call_handler(UIC_CATCH_ALL_JOYSTICKS, true, event);
  else if(event->type == SDL_JOYBUTTONUP)
    ui_call_handler(UIC_CATCH_ALL_JOYSTICKS, true, event);
}

