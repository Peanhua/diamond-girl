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

#include "widget.h"
#include "gfx_image.h"
#include <assert.h>

static int find_user_data(struct widget * object, const char * name, bool allocate_if_not_found);


char * widget_get_string(struct widget * object, const char * name)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  char * rv;
  int ind;

  ind = find_user_data(object, name, false);
  if(ind >= 0)
    rv = object->user_data_[ind];
  else
    rv = NULL;

  return rv;
}

long widget_get_long(struct widget * object, const char * name)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  long rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, false);
  if(ind >= 0)
    {
      long * ptr;

      ptr = object->user_data_[ind];
      rv = *ptr;
    }
  return rv;
}

unsigned long widget_get_ulong(struct widget * object, const char * name)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  unsigned long rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, false);
  if(ind >= 0)
    {
      unsigned long * ptr;
      
      ptr = object->user_data_[ind];
      rv = *ptr;
    }
  return rv;
}

double widget_get_double(struct widget * object, const char * name)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  double rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, false);
  if(ind >= 0)
    {
      double * ptr;
      
      ptr = object->user_data_[ind];
      rv = *ptr;
    }
  return rv;
}

void * widget_get_pointer(struct widget * object, const char * name)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  void * rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, false);
  if(ind >= 0)
    {
      void ** ptr;

      ptr = object->user_data_[ind];
      rv = *ptr;
    }
  return rv;
}




enum GFX_IMAGE widget_set_image(struct widget * object, const char * name, enum GFX_IMAGE user_data)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;

  ind = find_user_data(object, name, true);
  if(ind >= 0)
    {
      enum GFX_IMAGE * user_data_p;

      if(object->user_data_[ind] != NULL)
        free(object->user_data_[ind]);

      user_data_p = malloc(sizeof user_data);
      assert(user_data_p != NULL);
      if(user_data_p != NULL)
        {
          *user_data_p = user_data;
          object->user_data_[ind] = user_data_p;
        }
    }

  return widget_get_image(object, name);
}

char * widget_set_string(struct widget * object, const char * name, char const * const user_data, ...)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;

  ind = find_user_data(object, name, true);
  if(ind >= 0)
    {
      char buf[1024];
      va_list ap;

      if(object->user_data_[ind] != NULL)
        free(object->user_data_[ind]);

      va_start(ap, user_data);
      vsnprintf(buf, sizeof buf, user_data, ap);
      va_end(ap);

      object->user_data_[ind] = strdup(buf);
    }

  return widget_get_string(object, name);
}

long widget_set_long(struct widget * object, const char * name, long user_data)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;

  ind = find_user_data(object, name, true);
  if(ind >= 0)
    {
      long * ptr;

      ptr = object->user_data_[ind];
      if(ptr == NULL)
        object->user_data_[ind] = ptr = malloc(sizeof(long));
      *ptr = user_data;
    }

  return widget_get_long(object, name);
}

unsigned long widget_set_ulong(struct widget * object, const char * name, unsigned long user_data)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;

  ind = find_user_data(object, name, true);
  if(ind >= 0)
    {
      unsigned long * ptr;

      ptr = object->user_data_[ind];
      if(ptr == NULL)
        object->user_data_[ind] = ptr = malloc(sizeof(unsigned long));
      *ptr = user_data;
    }

  return widget_get_ulong(object, name);
}

double widget_set_double(struct widget * object, const char * name, double user_data)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;

  ind = find_user_data(object, name, true);
  if(ind >= 0)
    {
      double * ptr;

      ptr = object->user_data_[ind];
      if(ptr == NULL)
        object->user_data_[ind] = ptr = malloc(sizeof(double));
      *ptr = user_data;
    }

  return widget_get_double(object, name);
}

void * widget_set_pointer(struct widget * object, const char * name, void * user_data)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;

  ind = find_user_data(object, name, true);
  if(ind >= 0)
    {
      void ** ptr;
  
      ptr = object->user_data_[ind];
      if(ptr == NULL)
        object->user_data_[ind] = ptr = malloc(sizeof(void **));
      *ptr = user_data;
    }

  return widget_get_pointer(object, name);
}




static int find_user_data(struct widget * object, const char * name, bool allocate_if_not_found)
{
#if defined(UI_DEBUG)
  assert(object != NULL && object->deleted_ == 0);
#endif
  int ind;
  bool found;
  
  found = false;
  ind = -1;
  for(int i = 0; i < object->user_data_size_ && !found; i++)
    if(!strcmp(object->user_data_names_[i], name))
      {
	ind = i;
	found = true;
      }
  
  if(!found && allocate_if_not_found == true)
    {
      char ** names;
      void ** data;
      
      names = realloc(object->user_data_names_, sizeof(char *) * (object->user_data_size_ + 1));
      if(names != NULL)
	object->user_data_names_ = names;
      data = realloc(object->user_data_, sizeof(void *) * (object->user_data_size_ + 1));
      if(data != NULL)
	object->user_data_ = data;
      if(names != NULL && data != NULL)
	{
	  found = true;
	  ind = object->user_data_size_;
	  object->user_data_size_++;

	  object->user_data_[ind] = NULL;
	  object->user_data_names_[ind] = strdup(name);
	}

      if(names == NULL || data == NULL || object->user_data_names_[ind] == NULL)
	{
	  fprintf(stderr, "%s:%d  %s(%p, '%s'): Failed to allocate space for user_data.\n", __FILE__, (int) __LINE__, __FUNCTION__, object, name);
	  assert(found);
	  exit(EXIT_FAILURE);
	}
    }

  return ind;
}
