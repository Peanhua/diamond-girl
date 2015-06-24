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

#include "stack.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

struct stack * stack_new(void)
{
  struct stack * stack;

  stack = malloc(sizeof(struct stack));
  assert(stack != NULL);
  if(stack != NULL)
    {
      stack->compare_function = NULL;
      stack->data             = NULL;
      stack->size             = 0;
    }
  else
    fprintf(stderr, "%s:%s(): Failed to allocate memory: %s\n", __FILE__, __FUNCTION__, strerror(errno));

  return stack;
}


struct stack * stack_free(struct stack * stack)
{
  assert(stack != NULL);

  stack->compare_function = NULL;
  if(stack->data != NULL)
    {
      free(stack->data);
      stack->data = NULL;
    }
  free(stack);

  return NULL;
}


/* If the compare function is set, the stack is kept in order all the time. */
void stack_set_compare_function(struct stack * stack, stack_compare_func new_compare_func)
{
  assert(stack != NULL);

  stack->compare_function = new_compare_func;
  stack_sort(stack, NULL);
}


/* If compare_func is NULL, use the function set with stack_sort_compare_function(), if that is also NULL, then don't do anything. */
bool stack_sort(struct stack * stack, stack_compare_func compare_func)
{
  bool rv;

  assert(stack != NULL);

  if(compare_func == NULL)
    compare_func = stack->compare_function;

  if(compare_func != NULL)
    {
      qsort(stack->data, stack->size, sizeof(void *), compare_func);
      rv = true;
    }
  else
    rv = false;

  return rv;
}


void stack_push(struct stack * stack, void * item)
{
  void * tmp;

  assert(stack != NULL);
  
  tmp = realloc(stack->data, sizeof(void **) * (stack->size + 1));
  assert(tmp != NULL);
  if(tmp != NULL)
    {
      stack->data = tmp;
      stack->data[stack->size] = item;
      stack->size++;

      stack_sort(stack, NULL);
    }
  else
    fprintf(stderr, "%s:%s(): Failed to allocate memory: %s\n", __FILE__, __FUNCTION__, strerror(errno));
}


void * stack_pop(struct stack * stack)
{
  void * item;

  assert(stack != NULL);
  if(stack->size > 0)
    {
      item = stack->data[stack->size - 1];
      stack->data[stack->size - 1] = NULL;
      stack->size--;
    }
  else
    item = NULL;
  
  return item;
}


bool stack_pull(struct stack * stack, void * item)
{ /* Remove the item from the stack, FIFO. */
  bool success;

  assert(stack != NULL);
  success = false;
  for(unsigned int i = 0; success == false && i < stack->size; i++)
    if(stack->data[i] == item)
      {
        success = true;

        for(unsigned int j = i + 1; j < stack->size; j++)
          stack->data[j - 1] = stack->data[j];
        stack->size -= 1;
      }

  return success;
}

