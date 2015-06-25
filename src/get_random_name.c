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

#include "random.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

const char * get_random_name(void)
{
  static char buf[32];
  int len;
  char vow[] = "aeiouy";
  char con[] = "bcdfghjklmnpqrstvwxz";
  bool vowcon;

  len = 4 + get_rand(8);
  vowcon = get_rand(2);
  for(int i = 0; i < len; i++)
    {
      if(vowcon == true)
        { /* vowel */
          buf[i] = vow[get_rand(strlen(vow))];
          if(get_rand(100) < 35)
            vowcon = false;
        }
      else
        { /* consonant */
          buf[i] = con[get_rand(strlen(con))];
          if(get_rand(100) < 66)
            vowcon = true;
        }
      if(i == 0)
        buf[i] = toupper(buf[i]);
    }
  buf[len] = '\0';

  return buf;
}
