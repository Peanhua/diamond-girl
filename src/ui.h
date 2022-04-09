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

#ifndef UI_H_
#define UI_H_

#include <SDL/SDL.h>
#include <stdbool.h>

enum UI_COMMAND
  {
    UIC_DRAW,    /* Called twice when ui_draw() is called.
                    First pressed=true just after clearing the screen.
                    And second time pressed=false when all the widget drawing is done, just before gfx_flip(). */
    UIC_EXIT,    /* Synonymous to SDL_QUIT. */
    UIC_CANCEL,  /* Cancel current system, for example in game cancel current life = commit suicide. */
    UIC_UP,
    UIC_DOWN,
    UIC_LEFT,
    UIC_RIGHT,
    UIC_PAUSE,
    UIC_MANIPULATE,
    UIC_ALT_MANIPULATE,
    UIC_SCROLL_UP,
    UIC_SCROLL_DOWN,
    UIC_TOGGLE_FULLSCREEN,
    UIC_SLOWER,
    UIC_FASTER,
    UIC_HILITE_TARGET,
    UIC_UNKNOWN_KEY, /* Only for callbacks, not for binding. The given callback is called if a key is pressed/released and it's not bound to any command. */
    UIC_SAVE_PLAYBACK,
    UIC_SHOW_HELP,
    UIC_SHOW_LEGEND,
    UIC_VOLUME_DOWN,
    UIC_VOLUME_UP,
    UIC_MUTE,
    UIC_RECORDING_START,
    UIC_RECORDING_STOP,

    UIC_CATCH_ALL_KEYBOARD,  /* Catch all events for the keyboard      */
    UIC_CATCH_ALL_MOUSE,     /* Catch all events for the mouse         */
    UIC_CATCH_ALL_JOYSTICKS, /* Catch all events for all the joysticks */

    UIC_SIZEOF_
  };

enum WIDGET_BUTTON
  {
    WIDGET_BUTTON_LEFT,
    WIDGET_BUTTON_MIDDLE,
    WIDGET_BUTTON_RIGHT
  };


/**
 * Callback function, called when the registered binding is materialized as an event.
 * 
 * @param pressed      false == no pressing (upon release)
 *                     true  == pressure is on
 */
typedef void (* ui_func_handler_t)(bool pressed, SDL_Event * event);


struct ui_binding
{
  enum UI_COMMAND command;
  SDL_Event       event;
  bool            editable;
  bool            processed;
};

struct ui_handler
{
  ui_func_handler_t handler;
};



/* The main API. */
extern int  ui_initialize(void);
extern void ui_cleanup(void);
extern void ui_draw(bool colorbuffer_clearing);
extern void ui_process_events(bool wait_infinitely);
extern void ui_destroy_pending_events(void);

extern void ui_state_push(void);
extern void ui_state_pop(void);

/* Last user action get/set, used to determine if the player is idle or not. */
extern time_t ui_get_last_user_action(void);
extern void   ui_set_last_user_action(time_t when);
/* Miscellaneous utilities. */
extern char *          ui_command_name(enum UI_COMMAND command);
extern char *          ui_keyboard_key_name(SDLKey key);
extern char *          ui_keyboard_modifier_name(SDLMod mod);
extern struct widget * ui_get_widget_at_coordinates(int x, int y);
extern void            ui_wait_for_window_close(struct widget * window, struct widget * closebutton);
/* Bindings. */
extern void                 ui_bindings_initialize(void);
extern void                 ui_bindings_cleanup(void);
extern void                 ui_bindings_default(bool editable_bindings);
extern struct ui_binding *  ui_find_binding(SDL_Event * event, bool * pressed);
extern struct ui_binding ** ui_get_bindings(int controller);
extern void                 ui_call_bound_handler(SDL_Event * event);
extern struct ui_binding *  ui_bind(enum UI_COMMAND command, SDL_Event * event);
extern struct ui_binding *  ui_unbind(struct ui_binding * binding);
/* Handlers. */
extern void                ui_clear_handlers(void);
extern void                ui_set_common_handlers(void); /* Setup handlers for common activities (volume adjustment). */
extern void                ui_set_navigation_handlers(void); /* Setup handlers for standard menu navigation (up/down/left/right/manipulate). */
extern void                ui_set_handler(enum UI_COMMAND command, ui_func_handler_t handler);
extern void                ui_call_handler(enum UI_COMMAND command, bool pressed, SDL_Event * event);
extern void                ui_push_handlers(void);
extern void                ui_pop_handlers(void);



/* Internal functions below. */
extern void ui_process_mouse_move(int mouse_x, int mouse_y);
extern int  ui_process_mouse_button(int mouse_x, int mouse_y, enum WIDGET_BUTTON button, unsigned char release);



#endif
