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

#include "diamond_girl.h"
#include "globals.h"

char ** read_localized_text_file(const char * filename)
{
  char ** rv;
  char fn[strlen(filename) + (globals.language != NULL ? strlen(globals.language) : 0) + strlen("-.txt") + 1];

  rv = NULL;
  
  if(globals.language != NULL)
    {
      snprintf(fn, sizeof fn, "%s-%s.txt", filename, globals.language);
      rv = read_text_file(fn);
    }

  if(rv == NULL)
    {
      snprintf(fn, sizeof fn, "%s.txt", filename);
      rv = read_text_file(fn);
    }
  
  return rv;
}
