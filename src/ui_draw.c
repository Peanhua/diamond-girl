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
#include "gfx.h"
#include "globals.h"
#include "widget.h"
#include "stack.h"
#include <assert.h>

static bool ui_draw_process(struct widget * obj, int z);

static struct stack * destroy = NULL;

void ui_draw(bool colorbuffer_clearing DG_UNUSED_WITHOUT_OPENGL)
{
#ifdef WIN32
  /* This fixes garbage staying on screen in Windows8.1: */
  colorbuffer_clearing = true;
#endif

  if(globals.opengl)
    {
#ifdef WITH_OPENGL
      if(colorbuffer_clearing)
        glClear(GL_COLOR_BUFFER_BIT);
#endif
    }
#ifdef WITH_NONOPENGL
  else
    clear_area(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

  ui_call_handler(UIC_DRAW, true, NULL);

  struct widget * root;

  root = widget_root();
  assert(root != NULL && root->deleted_ == false);

  bool found;

  if(destroy == NULL)
    destroy = stack_new();
  destroy->size = 0;
  
  found = true;
  for(int z = 0; found == true || z < 9; z++)
    found = ui_draw_process(root, z);

  ui_call_handler(UIC_DRAW, false, NULL);

  struct widget * widget;

  do {
    widget = stack_pop(destroy);
    if(widget != NULL)
      widget_delete(widget);
  } while(widget != NULL);

  gfx_flip();
}


static bool ui_draw_process(struct widget * obj, int z)
{
  bool found;

  found = false;
  if(obj != NULL)
    if((widget_flags(obj) & WF_HIDDEN) == 0)
      {
        if(obj->z_ == z)
          {
            found = true;

            if(obj->on_timeout_ != NULL)
              {
                if(obj->timeout_timer_ == 0)
                  {
                    ui_func_on_timeout_t f;

                    f = obj->on_timeout_;
                    obj->on_timeout_ = NULL;
                    f(obj);
                  }
                else
                  obj->timeout_timer_--;
              }
            
            if(obj->on_draw_ != NULL)
              obj->on_draw_(obj);

            if(widget_flags(obj) & WF_DESTROY)
              stack_push(destroy, obj);
          }

        if(widget_flags(obj) & WF_CLIPPING)
          {
            if(globals.opengl)
              {
#ifdef WITH_OPENGL
                glScissor(widget_absolute_x(obj), SCREEN_HEIGHT - widget_absolute_y(obj) - widget_height(obj), widget_width(obj), widget_height(obj));
                gfxgl_state(GL_SCISSOR_TEST, true);
#endif
              }
#ifdef WITH_NONOPENGL
            else
              {
                SDL_Rect r;
      
                r.x = widget_absolute_x(obj);
                r.y = widget_absolute_y(obj);
                r.w = widget_width(obj);
                r.h = widget_height(obj);
                SDL_SetClipRect(gfx_game_screen(), &r);
              }
#endif
          }

        for(int i = 0; i < obj->children_count_; i++)
          if(obj->children_[i] != NULL)
            if(ui_draw_process(obj->children_[i], z) == true)
              found = true;

        if(widget_flags(obj) & WF_CLIPPING)
          {
            if(globals.opengl)
              {
#ifdef WITH_OPENGL
                gfxgl_state(GL_SCISSOR_TEST, false);
#endif
              }
#ifdef WITH_NONOPENGL
            else
              SDL_SetClipRect(gfx_game_screen(), NULL);
#endif
          }
      }
  
  return found;
}
