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

void ui_bindings_default(bool editable_bindings)
{
  SDL_Event e;
  int joycount;
  struct ui_binding * b;

  joycount = 2;

  /* UIC_CANCEL */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_ESCAPE;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_CANCEL, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    for(int i = 0; i < joycount; i++)
      {
        e.type           = SDL_JOYBUTTONDOWN;
        e.jbutton.which  = i;
        e.jbutton.button = 8;
        ui_bind(UIC_CANCEL, &e);
      }

  /* UIC_UP */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_UP;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_UP, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    for(int i = 0; i < joycount; i++)
      {
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 1;
        e.jaxis.value = -32768;
        ui_bind(UIC_UP, &e);
        
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 2;
        e.jaxis.value = -32768;
        ui_bind(UIC_UP, &e);
      }
  
  /* UIC_DOWN */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_DOWN;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_DOWN, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    for(int i = 0; i < joycount; i++)
      {
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 1;
        e.jaxis.value = 32767;
        ui_bind(UIC_DOWN, &e);
        
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 2;
        e.jaxis.value = 32767;
        ui_bind(UIC_DOWN, &e);
      }

  /* UIC_LEFT */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_LEFT;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_LEFT, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    for(int i = 0; i < joycount; i++)
      {
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 0;
        e.jaxis.value = -32768;
        ui_bind(UIC_LEFT, &e);
        
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 3;
        e.jaxis.value = -32768;
        ui_bind(UIC_LEFT, &e);
      }

  /* UIC_RIGHT */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_RIGHT;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_RIGHT, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    for(int i = 0; i < joycount; i++)
      {
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 0;
        e.jaxis.value = 32767;
        ui_bind(UIC_RIGHT, &e);
        
        e.type        = SDL_JOYAXISMOTION;
        e.jaxis.which = i;
        e.jaxis.axis  = 3;
        e.jaxis.value = 32767;
        ui_bind(UIC_RIGHT, &e);
      }

  /* UIC_PAUSE */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_PAUSE;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_PAUSE, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    for(int i = 0; i < joycount; i++)
      {
        e.type           = SDL_JOYBUTTONDOWN;
        e.jbutton.which  = i;
        e.jbutton.button = 9;
        ui_bind(UIC_PAUSE, &e);
      }

  /* UIC_MANIPULATE */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_RETURN;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_MANIPULATE, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_RSHIFT;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_MANIPULATE, &e);

      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_LSHIFT;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_MANIPULATE, &e);

      for(int i = 0; i < joycount; i++)
        {
          e.type           = SDL_JOYBUTTONDOWN;
          e.jbutton.which  = i;
          e.jbutton.button = 0;
          ui_bind(UIC_MANIPULATE, &e);
          
          e.type           = SDL_JOYBUTTONDOWN;
          e.jbutton.which  = i;
          e.jbutton.button = 2;
          ui_bind(UIC_MANIPULATE, &e);
        }
    }

  /* UIC_ALT_MANIPULATE */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_BACKSPACE;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_ALT_MANIPULATE, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_RCTRL;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_ALT_MANIPULATE, &e);

      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_LCTRL;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_ALT_MANIPULATE, &e);

      for(int i = 0; i < joycount; i++)
        {
          e.type           = SDL_JOYBUTTONDOWN;
          e.jbutton.which  = i;
          e.jbutton.button = 1;
          ui_bind(UIC_ALT_MANIPULATE, &e);
          
          e.type           = SDL_JOYBUTTONDOWN;
          e.jbutton.which  = i;
          e.jbutton.button = 3;
          ui_bind(UIC_ALT_MANIPULATE, &e);
        }
    }


  /* UIC_SCROLL_UP */
  if(editable_bindings == true)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_PAGEUP;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_SCROLL_UP, &e);
    }

  /* UIC_SCROLL_DOWN */
  if(editable_bindings == true)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_PAGEDOWN;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_SCROLL_DOWN, &e);
    }

  /* UIC_SHOW_HELP */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_F1;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_SHOW_HELP, &e);
      b->editable = false;
    }

  /* UIC_SHOW_LEGEND */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_F2;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_SHOW_LEGEND, &e);
      b->editable = false;
    }

  /* UIC_TOGGLE_FULLSCREEN */
  if(editable_bindings == false)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_RETURN;
      e.key.keysym.mod = KMOD_LALT;
      b = ui_bind(UIC_TOGGLE_FULLSCREEN, &e);
      b->editable = false;

      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_RETURN;
      e.key.keysym.mod = KMOD_RALT;
      b = ui_bind(UIC_TOGGLE_FULLSCREEN, &e);
      b->editable = false;
    }

  /* UIC_FASTER */
  if(editable_bindings == true)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_PAGEUP;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_FASTER, &e);
    }

  /* UIC_SLOWER */
  if(editable_bindings == true)
    {
      e.type           = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_PAGEDOWN;
      e.key.keysym.mod = KMOD_NONE;
      ui_bind(UIC_SLOWER, &e);
    }

  /* UIC_VOLUME_DOWN */
  if(editable_bindings == false)
    {
      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_MINUS;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_VOLUME_DOWN, &e);
      b->editable = false;

      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_KP_MINUS;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_VOLUME_DOWN, &e);
      b->editable = false;
    }

  /* UIC_VOLUME_UP */
  if(editable_bindings == false)
    {
      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_PLUS;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_VOLUME_UP, &e);
      b->editable = false;

      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_KP_PLUS;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_VOLUME_UP, &e);
      b->editable = false;
    }

  /* UIC_MUTE */
  if(editable_bindings == false)
    {
      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_F10;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_MUTE, &e);
      b->editable = false;
    }

  if(editable_bindings == true)
    {
      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_F11;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_RECORDING_START, &e);
      b->editable = true;

      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = SDLK_F12;
      e.key.keysym.mod = KMOD_NONE;
      b = ui_bind(UIC_RECORDING_STOP, &e);
      b->editable = true;
    }
}
