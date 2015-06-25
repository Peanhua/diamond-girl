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

#include "gc.h"
#include "stack.h"
#include "gfx.h"
#include "widget.h"
#include "gfxbuf.h"
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

      count = 0;
      for(unsigned int j = 0; j < gcdata[i].stack->size; j++)
        if(gcdata[i].stack->data[j] != NULL)
          {
            printf("GC-%d: not collected %p\n", i, gcdata[i].stack->data[j]);
            switch(i)
              {
              case GCT_GFXBUF:
                {
                  char * bt;
                  
                  bt = ((struct gfxbuf *) gcdata[i].stack->data[j])->backtrace;
                  if(bt != NULL)
                    printf("Backtrace:\n%s\n", bt);
                }
                break;
              case GCT_WIDGET:
                widget_dump(gcdata[i].stack->data[j]);
                break;
              case GCT_SIZEOF_:
                break;
              }
            count++;
          }
      printf("GC-%d: not collected: total %d items\n", i, count);
#endif

#ifdef WITH_OPENGL
      if(i == GCT_GFXBUF)
        {
          bool done;

          done = false;
          while(done == false)
            {
              done = true;
              for(unsigned int j = 0; done == false && j < gcdata[i].stack->size; j++)
                if(gcdata[i].stack->data[j] != NULL)
                  {
                    done = false;
                    gfxbuf_free(gcdata[i].stack->data[j]);
                  }
            }
        }
#endif
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
