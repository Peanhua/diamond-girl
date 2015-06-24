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

#include "diamond_girl.h"
#include "ui.h"
#include "widget.h"

static void on_up(bool pressed, SDL_Event * event);
static void on_down(bool pressed, SDL_Event * event);
static void on_left(bool pressed, SDL_Event * event);
static void on_right(bool pressed, SDL_Event * event);
static void on_manipulate(bool pressed, SDL_Event * event);
static void on_alt_manipulate(bool pressed, SDL_Event * event);
static void on_manipulate_internal(bool pressed, bool alt);

void ui_set_navigation_handlers(void)
{
  ui_set_handler(UIC_UP,             on_up);
  ui_set_handler(UIC_DOWN,           on_down);
  ui_set_handler(UIC_LEFT,           on_left);
  ui_set_handler(UIC_RIGHT,          on_right);
  ui_set_handler(UIC_MANIPULATE,     on_manipulate);
  ui_set_handler(UIC_ALT_MANIPULATE, on_alt_manipulate);
}

static void on_up(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * current;
      
      current = widget_focus();
      while(current != NULL)
        {
          struct widget * new;
          
          new = widget_get_navigation_up(current);
          if(new != NULL)
            if(widget_enabled(new))
              if( ! (widget_flags(new) & WF_HIDDEN) )
                {
                  widget_set_focus(new);
                  new = NULL;
                }
          current = new;
        }
    }
}

static void on_down(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * current;
      
      current = widget_focus();
      while(current != NULL)
        {
          struct widget * new;
          
          new = widget_get_navigation_down(current);
          if(new != NULL)
            if(widget_enabled(new))
              if( ! (widget_flags(new) & WF_HIDDEN) )
                {
                  widget_set_focus(new);
                  new = NULL;
                }
          current = new;
        }
    }
}

static void on_left(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * current;

      current = widget_focus();
      while(current != NULL)
        {
          struct widget * new;

          new = widget_get_navigation_left(current);
          if(new != NULL)
            if(widget_enabled(new))
              if( ! (widget_flags(new) & WF_HIDDEN) )
                {
                  widget_set_focus(new);
                  new = NULL;
                }
          current = new;
        }
    }
}

static void on_right(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * current;

      current = widget_focus();
      while(current != NULL)
        {
          struct widget * new;

          new = widget_get_navigation_right(current);
          if(new != NULL)
            if(widget_enabled(new))
              if( ! (widget_flags(new) & WF_HIDDEN) )
                {
                  widget_set_focus(new);
                  new = NULL;
                }
          current = new;
        }
    }
}

static void on_manipulate(bool pressed, SDL_Event * event DG_UNUSED)
{
  on_manipulate_internal(pressed, false);
}

static void on_alt_manipulate(bool pressed, SDL_Event * event DG_UNUSED)
{
  on_manipulate_internal(pressed, true);
}

static void on_manipulate_internal(bool pressed, bool alt)
{
  struct widget * obj;

  obj = widget_focus();
  if(obj != NULL)
    {
      ui_func_on_activate_t func;

      func = NULL;
      if(pressed == true)
        {
          widget_set_activated(obj);
          func = widget_on_activate(obj);
        }
      else
        {
          if(widget_state(obj) == WIDGET_STATE_ACTIVATED)
            {
              widget_set_focus(obj);
              func = widget_on_release(obj);
            }
        }

      if(func != NULL)
        func(obj, alt == true ? WIDGET_BUTTON_RIGHT : WIDGET_BUTTON_LEFT);
    }
}
