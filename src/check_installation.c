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
#include <unistd.h>
#include <errno.h>

bool check_installation(void)
{
  bool rv;
  char * filenames[] =
    {
      "gfx/diamond.png",
      NULL
    };
  
  rv = true;

  for(int i = 0; rv == true && filenames[i] != NULL; i++)
    {
      const char * fn;

      fn = get_data_filename(filenames[i]);
      if(fn != NULL)
        {
          if(access(fn, R_OK) != 0)
            {
              fprintf(stderr, "Unable to access '%s': %s\n", fn, strerror(errno));
              rv = false;
            }
        }
      else
        {
          fprintf(stderr, "Unable to determine filename.\n");
          rv = false;
        }
    }

  return rv;
}
