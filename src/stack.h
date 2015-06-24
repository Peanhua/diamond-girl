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

#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>

typedef int (* stack_compare_func)(const void *, const void *);

struct stack
{
  stack_compare_func compare_function;
  void **            data;
  unsigned int       size;
};


extern struct stack * stack_new(void);
extern struct stack * stack_free(struct stack * stack);

/* If the compare function is set, the stack is kept in order all the time. */
extern void           stack_set_compare_function(struct stack * stack, stack_compare_func new_compare_func);

/* If compare_func is NULL, use the function set with stack_set_sort_compare_function(), if that is also NULL, then don't do anything. */
extern bool           stack_sort(struct stack * stack, stack_compare_func compare_func);

extern void           stack_push(struct stack * stack, void * item);
extern void *         stack_pop(struct stack * stack);
extern bool           stack_pull(struct stack * stack, void * item); /* Remove the item from the stack. In case of multiple same items, FIFO is used. */


#endif
