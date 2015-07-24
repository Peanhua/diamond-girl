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
#include "gfx_image.h"
#include "widget.h"
#include <assert.h>

static int find_user_data(struct widget * object, const char * name, char type, bool allocate_if_not_found);


char * widget_get_string(struct widget * object, const char * name)
{
  assert(object != NULL && object->deleted_ == false);
  char * rv;
  int ind;

  ind = find_user_data(object, name, 's', false);
  if(ind >= 0)
    rv = object->user_data_[ind];
  else
    rv = NULL;

  return rv;
}

long widget_get_long(struct widget * object, const char * name)
{
  assert(object != NULL && object->deleted_ == false);
  long rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, 'l', false);
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
  assert(object != NULL && object->deleted_ == false);
  unsigned long rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, 'u', false);
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
  assert(object != NULL && object->deleted_ == false);
  double rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, 'd', false);
  if(ind >= 0)
    {
      double * ptr;
      
      ptr = object->user_data_[ind];
      rv = *ptr;
    }
  return rv;
}

void * widget_get_pointer(struct widget * object, const char * name, char type)
{
  assert(object != NULL && object->deleted_ == false);
  void * rv;
  int ind;

  rv = 0;
  ind = find_user_data(object, name, type, false);
  if(ind >= 0)
    {
      void ** ptr;

      ptr = object->user_data_[ind];
      rv = *ptr;
    }
  return rv;
}

struct cave * widget_get_cave_pointer(struct widget * widget, char const * name)
{
  return widget_get_pointer(widget, name, 'C');
}

struct gamedata * widget_get_gamedata_pointer(struct widget * widget, char const * name)
{
  return widget_get_pointer(widget, name, 'G');
}

struct image * widget_get_image_pointer(struct widget * widget, char const * name)
{
  return widget_get_pointer(widget, name, 'I');
}

struct map * widget_get_map_pointer(struct widget * widget, char const * name)
{
  return widget_get_pointer(widget, name, 'M');
}

struct questline * widget_get_questline_pointer(struct widget * widget, char const * name)
{
  return widget_get_pointer(widget, name, 'Q');
}

struct theme * widget_get_theme_pointer(struct widget * widget, char const * name)
{
  return widget_get_pointer(widget, name, 'T');
}

struct widget * widget_get_widget_pointer(struct widget * widget, char const * name)
{
  struct widget * wp;

  wp = widget_get_pointer(widget, name, 'W');

  if(wp != NULL)
    if(gc_check(GCT_WIDGET, wp) == false)
      wp = NULL;

  if(wp != NULL && wp->deleted_ == true)
    wp = NULL;
  
  return wp;
}




enum GFX_IMAGE widget_set_image(struct widget * object, const char * name, enum GFX_IMAGE user_data)
{
  assert(object != NULL && object->deleted_ == false);
  int ind;

  ind = find_user_data(object, name, 'i', true);
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
  assert(object != NULL && object->deleted_ == false);
  int ind;

  ind = find_user_data(object, name, 's', true);
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
  assert(object != NULL && object->deleted_ == false);
  int ind;

  ind = find_user_data(object, name, 'l', true);
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
  assert(object != NULL && object->deleted_ == false);
  int ind;

  ind = find_user_data(object, name, 'u', true);
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
  assert(object != NULL && object->deleted_ == false);
  int ind;

  ind = find_user_data(object, name, 'd', true);
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

void * widget_set_pointer(struct widget * object, const char * name, char type, void * user_data)
{
  assert(object != NULL && object->deleted_ == false);
  int ind;

  ind = find_user_data(object, name, type, true);
  if(ind >= 0)
    {
      void ** ptr;
  
      ptr = object->user_data_[ind];
      if(ptr == NULL)
        object->user_data_[ind] = ptr = malloc(sizeof(void **));
      *ptr = user_data;
#ifndef NDEBUG
      object->user_data_types_[ind] = type;
#endif
    }

  return widget_get_pointer(object, name, type);
}

struct cave * widget_set_cave_pointer(struct widget * widget, char const * name, struct cave * cave)
{
  return widget_set_pointer(widget, name, 'C', cave);
}

struct gamedata * widget_set_gamedata_pointer(struct widget * widget, char const * name, struct gamedata * gamedata)
{
  return widget_set_pointer(widget, name, 'G', gamedata);
}

struct image * widget_set_image_pointer(struct widget * widget, char const * name, struct image * image)
{
  return widget_set_pointer(widget, name, 'I', image);
}

struct map * widget_set_map_pointer(struct widget * widget, char const * name, struct map * map)
{
  return widget_set_pointer(widget, name, 'M', map);
}

struct questline * widget_set_questline_pointer(struct widget * widget, char const * name, struct questline * questline)
{
  return widget_set_pointer(widget, name, 'Q', questline);
}

struct theme * widget_set_theme_pointer(struct widget * widget, char const * name, struct theme * theme)
{
  return widget_set_pointer(widget, name, 'T', theme);
}

struct widget * widget_set_widget_pointer(struct widget * widget, char const * name, struct widget * widget_pointer)
{
  return widget_set_pointer(widget, name, 'W', widget_pointer);
}



static int find_user_data(struct widget * object, const char * name, char type DG_NDEBUG_UNUSED, bool allocate_if_not_found)
{
  assert(object != NULL && object->deleted_ == false);
  int ind;
  bool found;
  
  found = false;
  ind = -1;
  for(int i = 0; i < object->user_data_size_ && !found; i++)
    if(!strcmp(object->user_data_names_[i], name))
      {
        assert(object->user_data_types_[i] == type);
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
#ifndef NDEBUG
      char * types;

      types = realloc(object->user_data_types_, sizeof(char) * (object->user_data_size_ + 1));
      assert(types != NULL);
      object->user_data_types_ = types;
#endif
      if(names != NULL && data != NULL)
	{
	  found = true;
	  ind = object->user_data_size_;
	  object->user_data_size_++;

	  object->user_data_[ind] = NULL;
	  object->user_data_names_[ind] = strdup(name);
#ifndef NDEBUG
          object->user_data_types_[ind] = type;
#endif
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
