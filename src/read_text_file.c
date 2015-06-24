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
#include <string.h>
#include <assert.h>

/* Read a text file and return char ** list of lines, ending with a NULL. */
char ** read_text_file(const char * filename)
{
  char ** rv;
  char * buffer;
  int    buffer_size;

  rv = NULL;
  if(read_file(filename, &buffer, &buffer_size) == true)
    {
      int linecount;
      char * tmp;

      linecount = 0;
      tmp = buffer;
      while(tmp != NULL)
        {
          tmp = strchr(tmp, '\n');
          if(tmp != NULL)
            tmp++;
          linecount++;
        }
      assert(linecount > 0);

      rv = malloc(sizeof(char *) * linecount);
      assert(rv != NULL);
      if(rv != NULL)
        {
          char * curpos;

          curpos = buffer;
          for(int i = 0; i < linecount - 1; i++)
            {
              assert(curpos != NULL);
              tmp = strchr(curpos, '\n');
              if(tmp != NULL)
                {
                  *tmp = '\0';
                  rv[i] = strdup(curpos);
                  curpos = tmp + 1;
                }
            }
          rv[linecount - 1] = NULL;
        }
      free(buffer);
    }

  return rv;
}
