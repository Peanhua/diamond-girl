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

#include "diamond_girl.h"
#include "cave.h"
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


static int qsort_string_compare(const char ** a, const char ** b);
static bool add_cave(const char * name, char *** list, int * n);

char ** get_cave_names(bool sorted)
{ /* read cave names */
  char ** cave_names;
  int s;
  int nonsortable_caves;
  DIR * d;

  cave_names = NULL;
  s = 0;

  add_cave("beginners_cave", &cave_names, &s);
  add_cave("a",              &cave_names, &s);
  add_cave("/random",        &cave_names, &s);
  add_cave("/well",          &cave_names, &s);
  nonsortable_caves = 4; /* The number of caves added above. These cave names are not sorted later with others to preserve their order and position in the list. */

  d = opendir(get_data_filename("maps"));
  if(d != NULL)
    {
      struct dirent * e;
      do
        {
          e = readdir(d);
          if(e != NULL)
            if(isalnum(e->d_name[0]))
              {
                if(add_cave(e->d_name, &cave_names, &s) == true)
                  ;
                else
                  e = NULL;
              }
        } while(e != NULL);

      closedir(d);
    }
  else
    fprintf(stderr, "Failed to open directory '%s': %s\n", get_data_filename("maps"), strerror(errno));


  if(cave_names != NULL && sorted == true)
    {
      int n;
      
      n = 0;
      while(cave_names[n] != NULL)
	n++;

      if(n > nonsortable_caves)
        qsort(cave_names + nonsortable_caves, n - nonsortable_caves, sizeof(char *), (int(*)(const void *, const void *)) qsort_string_compare);
    }

  return cave_names;
}


static int qsort_string_compare(const char ** a, const char ** b)
{
  return strcmp(*a, *b);
}


static bool add_cave(const char * name, char *** list, int * n)
{
  bool rv;
  bool found;
  
  rv = true;

  found = false;
  for(int i = 0; found == false && i < *n; i++)
    {
      char * ename;

      ename = (*list)[i];
      if(*ename == '/')
        ename++;
      
      if(!strcmp(ename, name))
        found = true;
    }

  if(found == false)
    { /* Only add if the 'name' is not already in the 'list'. */
      char ** tmp;

      tmp = realloc((*list), sizeof(char *) * ((*n) + 1 + 1));
      if(tmp != NULL)
        {
          (*list) = tmp;

          (*list)[*n] = strdup(name);
          if((*list)[*n] != NULL)
            {
              (*n)++;
            }
          else
            {
              fprintf(stderr, "Failed to allocate memory for predefined cave names: %s\n", strerror(errno));
              rv = false;
            }

          (*list)[*n] = NULL;
        }
      else
        {
          fprintf(stderr, "Failed to allocate memory for predefined cave names: %s\n", strerror(errno));
          rv = false;
        }
    }
  
  return rv;
}

