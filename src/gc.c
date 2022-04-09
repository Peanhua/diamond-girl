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

#include "gc.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "image.h"
#include "stack.h"
#include "widget.h"
#include <assert.h>
#include <stdio.h>

struct gc
{
  enum GC_TYPE   type;
  struct stack * stack;
};

static struct gc gcdata[GCT_SIZEOF_];

bool gc_initialize(void)
{
  bool rv;

  rv = true;
  for(int i = 0; i < GCT_SIZEOF_; i++)
    {
      gcdata[i].stack = stack_new();
      assert(gcdata[i].stack != NULL);
      if(gcdata[i].stack == NULL)
        rv = false;
    }

  return rv;
}


void gc_cleanup(void)
{
  for(int i = 0; i < GCT_SIZEOF_; i++)
    {
#ifndef NDEBUG
      int count;
      char * typename;
      char * t_gfxbuf = "gfxbuf";
      char * t_widget = "widget";
      char * t_cave   = "cave";
      char * t_theme  = "theme";
      char * t_string = "string";
      char * t_image  = "image";
      switch(i)
        {
        case GCT_GFXBUF:    typename = t_gfxbuf; break;
        case GCT_WIDGET:    typename = t_widget; break;
        case GCT_CAVE:      typename = t_cave;   break;
        case GCT_THEME:     typename = t_theme;  break;
        case GCT_STRING:    typename = t_string; break;
        case GCT_IMAGE:     typename = t_image;  break;
        default: assert(0); typename = NULL;     break;
        }
      count = 0;
      for(unsigned int j = 0; j < gcdata[i].stack->size; j++)
        if(gcdata[i].stack->data[j] != NULL)
          {
            char * bt;
            
            printf("GC-%s: not collected %p\n", typename, gcdata[i].stack->data[j]);
            switch(i)
              {
              case GCT_GFXBUF:
#ifdef WITH_OPENGL
                bt = ((struct gfxbuf *) gcdata[i].stack->data[j])->backtrace;
                gfxbuf_dump(gcdata[i].stack->data[j]);
#endif
                break;
              case GCT_WIDGET:
                bt = ((struct widget *) gcdata[i].stack->data[j])->backtrace_;
                widget_dump(gcdata[i].stack->data[j]);
                break;
              case GCT_STRING:
                bt = NULL;
                printf(" string = %s\n", (char *) gcdata[i].stack->data[j]);
                break;
              case GCT_IMAGE:
                bt = ((struct image *) gcdata[i].stack->data[j])->backtrace;
                image_dump(gcdata[i].stack->data[j], 0);
                break;
              case GCT_SIZEOF_:
                bt = NULL;
                break;
              }
            if(bt != NULL)
              printf("Backtrace:\n%s\n", bt);
            
            count++;
          }
      printf("GC-%s: not collected: total %d items\n", typename, count);
#endif

      bool done;

      done = false;
      while(done == false)
        {
          done = true;
          for(unsigned int j = 0; done == false && j < gcdata[i].stack->size; j++)
            if(gcdata[i].stack->data[j] != NULL)
              {
                if(i == GCT_STRING)
                  {
                    free(gcdata[i].stack->data[j]);
                  }
#ifdef WITH_OPENGL
                else if(i == GCT_GFXBUF)
                  {
                    done = false;
                    gfxbuf_free(gcdata[i].stack->data[j]);
                  }
#endif
                gcdata[i].stack->data[j] = NULL;
              }
        }
      gcdata[i].stack = stack_free(gcdata[i].stack);
    }
}


void gc_new(enum GC_TYPE type, void * object)
{
  assert(object != NULL);
  assert(type < GCT_SIZEOF_);
  assert(gcdata[type].stack != NULL);
  stack_push(gcdata[type].stack, object);
}


void gc_free(enum GC_TYPE type, void * object)
{
#ifndef NDEBUG
  bool success;

  assert(object != NULL);
  assert(type < GCT_SIZEOF_);
  assert(gcdata[type].stack != NULL);

  success =
#endif
    stack_pull(gcdata[type].stack, object);

  assert(success == true);
}


struct stack * gc_get_stack(enum GC_TYPE type)
{
  assert(type < GCT_SIZEOF_);
  assert(gcdata[type].stack != NULL);

  return gcdata[type].stack;
}



void gc_empty_stack(enum GC_TYPE type)
{
  for(unsigned int i = 0; i < gcdata[type].stack->size; i++)
    gcdata[type].stack->data[i] = NULL;
}


void gc_run(void)
{
  bool done;

  /* This is run once per frame in title screen.
   * Keeps the pointers available for referencing for one frame.
   * Also widget_get_widget_pointer() returns NULL if the object has been marked as deleted.
   */
  done = false;
  for(unsigned int i = 0; done == false && i < gcdata[GCT_WIDGET].stack->size; i++)
    {
      struct widget * widget;
      
      widget = gcdata[GCT_WIDGET].stack->data[i];
      if(widget != NULL)
        if(widget->deleted_ == true)
          {
            done = true; // gc_free() modifies the stack order and size
            gc_free(GCT_WIDGET, widget);
            widget_free_(widget);
          }
    }
}


bool gc_check(enum GC_TYPE type, void * object)
{
  bool found;

  found = false;
  for(unsigned int i = 0; found == false && i < gcdata[type].stack->size; i++)
    if(gcdata[type].stack->data[i] == object)
      found = true;

  return found;
}
