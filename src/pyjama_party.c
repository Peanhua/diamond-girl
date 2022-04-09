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
#include "girl.h"
#include "globals.h"
#include "highscore.h"
#include <assert.h>

bool pyjama_party_load(void)
{ /* Load all the pyjama party girls and stuff. */
  char * buf;
  int    bufsize;
  bool rv;

  globals.pyjama_party_girls_size = 0;
  globals.pyjama_party_girls = NULL;

  rv = read_file(get_save_filename("party/order"), &buf, &bufsize);
  if(rv == true)
    {
      char * curpos, * sep;

      curpos = buf;
      do {
        sep = strchr(curpos, '\n');
        if(sep != NULL)
          {
            *sep = '\0';
            curpos = sep + 1;
            globals.pyjama_party_girls_size++;
          }
      } while(sep != NULL);

      if(globals.pyjama_party_girls_size > 0)
        {
          globals.pyjama_party_girls = malloc(sizeof(struct girl *) * globals.pyjama_party_girls_size);
          assert(globals.pyjama_party_girls != NULL);
          for(int i = 0; i < globals.pyjama_party_girls_size; i++)
            globals.pyjama_party_girls[i] = NULL;

          curpos = buf;
          for(int i = 0; i < globals.pyjama_party_girls_size; i++)
            {
              globals.pyjama_party_girls[i] = girl_load(curpos);
              curpos += strlen(curpos) + 1;
            }
        }

      free(buf);
    }

  return rv;
}

bool pyjama_party_save(void)
{ /* Save all the pyjama party girls and stuff. */
  char * buf;
  int bufsize;
  bool rv;
  
  rv = true;
  bufsize = globals.pyjama_party_girls_size * sizeof(globals.pyjama_party_girls[0]->name) + 1;
  buf = malloc(bufsize);
  assert(buf != NULL);
  strcpy(buf, "");
  for(int i = 0; i < globals.pyjama_party_girls_size; i++)
    {
      if(girl_save(globals.pyjama_party_girls[i]) == true)
        snprintf(buf + strlen(buf), bufsize - strlen(buf), "%s\n", globals.pyjama_party_girls[i]->name);
      else
        rv = false;
    }

  if(write_file(get_save_filename("party/order"), buf, strlen(buf) + 1) == false)
    rv = false;

  free(buf);

  return rv;
}
