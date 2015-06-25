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

#ifndef NDEBUG

#include "backtrace.h"
#include "globals.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#if !defined(WIN32)
#include <execinfo.h>
#endif


char * get_backtrace(void)
{
  char * rv;

  rv = NULL;

#if !defined(WIN32)

  if(globals.backtrace == true)
    {
      int n, len;
      void * buffer[256];
      char ** tmp;
      
      n = backtrace(buffer, 256);
      tmp = backtrace_symbols(buffer, n);
      
      len = 0;
      for(int i = 0; i < n; i++)
        len += strlen(tmp[i]) + 1;
      
      rv = malloc(len + 1);
      assert(rv != NULL);
      
      if(rv != NULL)
        {
          strcpy(rv, "");
          for(int i = 0; i < n; i++)
            {
              strcat(rv, tmp[i]);
              strcat(rv, "\n");
            }
        }
      free(tmp);
    }

#endif

  return rv;
}

#endif
