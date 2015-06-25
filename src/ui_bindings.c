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

#include "ui.h"
#include "globals.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

static struct ui_binding ** bindings;
static int                  bindings_count;


static struct ui_binding * ui_find_binding_internal(SDL_Event * event, bool * pressed);


void ui_bindings_initialize(void)
{
  bindings       = NULL;
  bindings_count = 0;
}


void ui_bindings_cleanup(void)
{
  for(int i = 0; i < bindings_count; i++)
    free(bindings[i]);
  free(bindings);

  bindings       = NULL;
  bindings_count = 0;
}


void ui_call_bound_handler(SDL_Event * event)
{
  if(event->type == SDL_JOYAXISMOTION)
    { /* Switch between analog/digital joystick modes. */
      if(globals.joysticks_analog[event->jaxis.which] == true)
        { /* From N amount of digital events, switch to digital mode. */
          if(event->jaxis.value == 0 || event->jaxis.value == -32768 || event->jaxis.value == 32767)
            {
              globals.joysticks_digital_counter[event->jaxis.which]++;
              if(globals.joysticks_digital_counter[event->jaxis.which] >= 10)
                globals.joysticks_analog[event->jaxis.which] = false;
            }
          else /* From a single analog event, reset the digital counter. */
            globals.joysticks_digital_counter[event->jaxis.which] = 0;
        }
      else
        { /* From a single analog event, switch to analog mode. */
          if(event->jaxis.value != 0 && event->jaxis.value != -32768 && event->jaxis.value != 32767)
            {
              globals.joysticks_analog[event->jaxis.which]          = true;
              globals.joysticks_digital_counter[event->jaxis.which] = 0;
            }
        }
    }

  struct ui_binding * binding;
  bool pressed;

  for(int i = 0; i < bindings_count; i++)
    if(bindings[i] != NULL)
      bindings[i]->processed = false;

  do {
    binding = ui_find_binding_internal(event, &pressed);
    if(binding != NULL)
      { /* call the handler */
        ui_call_handler(binding->command, pressed, event);
        binding->processed = true;
      }
  } while(binding != NULL);
}

struct ui_binding * ui_find_binding(SDL_Event * event, bool * pressed)
{
  for(int i = 0; i < bindings_count; i++)
    if(bindings[i] != NULL)
      bindings[i]->processed = false;

  return ui_find_binding_internal(event, pressed);
}


static struct ui_binding * ui_find_binding_internal(SDL_Event * event, bool * pressed)
{
  struct ui_binding * binding;

  binding = NULL;
  for(int i = 0; binding == NULL && i < bindings_count; i++)
    if(bindings[i] != NULL && bindings[i]->processed == false)
      {
        bool match;
        bool tmpp;
        
        match = false;
        tmpp = false;
        
        if(event->type == bindings[i]->event.type)
          switch(event->type)
            {
            case SDL_KEYDOWN:
              tmpp = true;
            case SDL_KEYUP:
              if(bindings[i]->event.key.keysym.sym == event->key.keysym.sym)
                { /* Ignore modifiers if the binding does not have modifiers defined. */
                  SDLMod mod;

                  mod = event->key.keysym.mod & (KMOD_ALT | KMOD_CTRL | KMOD_SHIFT);
                  if(bindings[i]->event.key.keysym.mod == KMOD_NONE || bindings[i]->event.key.keysym.mod == mod)
                    match = true;
                }
              break;
            case SDL_MOUSEBUTTONDOWN:
              tmpp = true;
            case SDL_MOUSEBUTTONUP:
              if(event->button.button == bindings[i]->event.button.button)
                match = true;
              break;
            case SDL_JOYAXISMOTION:
              {
                const int threshold = 32768 / 2;

                if(event->jaxis.which == bindings[i]->event.jaxis.which && event->jaxis.axis == bindings[i]->event.jaxis.axis)
                  {
                    if(abs(event->jaxis.value) > threshold)
                      tmpp = true;

                    if(tmpp == true)
                      {
                        if((event->jaxis.value < 0 && bindings[i]->event.jaxis.value < 0) || (event->jaxis.value > 0 && bindings[i]->event.jaxis.value > 0))
                          match = true;
                      }
                    else /* if(tmpp == false) */
                      { /* Reset all movement within this axis. */
                        match = true;
                      }
                  }
              }
              break;
            case SDL_JOYBUTTONDOWN:
              tmpp = true;
            case SDL_JOYBUTTONUP:
              if(event->jbutton.which == bindings[i]->event.jbutton.which && event->jbutton.button == bindings[i]->event.jbutton.button)
                match = true;
              break;
            }

        if(match == true)
          {
            binding = bindings[i];
            if(pressed != NULL)
              (*pressed) = tmpp;
          }
      }

  return binding;
}



struct ui_binding * ui_bind(enum UI_COMMAND command, SDL_Event * event)
{
  struct ui_binding * binding;

  binding = malloc(sizeof(struct ui_binding));
  assert(binding != NULL);
  if(binding != NULL)
    {
      int ind;

      ind = -1;
      for(int i = 0; ind == -1 && i < bindings_count; i++)
        if(bindings[i] == NULL)
          ind = i;

      if(ind == -1)
        {
          struct ui_binding ** tmp;

          tmp = realloc(bindings, (bindings_count + 1) * sizeof(struct ui_binding *));
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              ind = bindings_count;
              bindings = tmp;
              bindings_count += 1;
            }
          else
            fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
        }
      
      if(ind >= 0)
        {
          bindings[ind] = binding;

          binding->command = command;
          memcpy(&binding->event, event, sizeof(SDL_Event));
          binding->editable = true;
        }
      else
        {
          free(binding);
          binding = NULL;
        }
    }
  else
    fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));

  /* Automatically allocate the release events too. */
  if(event->type == SDL_KEYDOWN)
    {
      event->type = SDL_KEYUP;
      ui_bind(command, event);
    }
  else if(event->type == SDL_JOYBUTTONDOWN)
    {
      event->type = SDL_JOYBUTTONUP;
      ui_bind(command, event);
    }
  
  return binding;
}



struct ui_binding * ui_unbind(struct ui_binding * binding)
{
  for(int i = 0; binding != NULL && i < bindings_count; i++)
    if(bindings[i] == binding)
      {
        free(bindings[i]);
        bindings[i] = NULL;
        binding = NULL;
      }

  return NULL;
}


/**
 * Get bindings for a specific controller.
 * 
 * @param int controller 0 = keyboard, 1 = mouse, 2+ = joysticks
 */
struct ui_binding ** ui_get_bindings(int controller)
{
  struct ui_binding ** rv;
  int rv_count;

  rv = NULL;
  rv_count = 0;
  for(int i = 0; i < bindings_count; i++)
    if(bindings[i] != NULL)
      {
        bool add_this;

        add_this = false;
        switch(controller)
          {
          case -1: /* All */
            add_this = true;
            break;
          case 0: /* Keyboard */
            if(bindings[i]->event.type == SDL_KEYDOWN)
              add_this = true;
            break;
          case 1: /* Mouse */
            if(bindings[i]->event.type == SDL_MOUSEMOTION || bindings[i]->event.type == SDL_MOUSEBUTTONDOWN)
              add_this = true;
            break;
          default: /* Joysticks */
            if((bindings[i]->event.type == SDL_JOYAXISMOTION && bindings[i]->event.jaxis.which   == controller - 2) ||
               (bindings[i]->event.type == SDL_JOYBUTTONDOWN && bindings[i]->event.jbutton.which == controller - 2)   )
              add_this = true;
            break;
          }

        if(add_this == true)
          {
            struct ui_binding ** tmp;

            tmp = realloc(rv, (rv_count + 2) * sizeof(struct ui_binding *));
            assert(tmp != NULL);
            if(tmp != NULL)
              {
                rv = tmp;
                rv[rv_count    ] = bindings[i];
                rv[rv_count + 1] = NULL;
                rv_count += 1;
              }
          }
      }

  return rv;
}
