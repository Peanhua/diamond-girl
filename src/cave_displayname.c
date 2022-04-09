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

#include "cave.h"
#include "globals.h"
#include <libintl.h>
#include <stdio.h>
#include <string.h>

char * cave_displayname(char * cave_name)
{
  static struct
  {
    char * cave_name;
    char * displayname;
  } caves[] =
      {
        { "/random",        NULL },
        { "/well",          NULL },
        { "a",              NULL },
        { "beginners_cave", NULL },
        { NULL,             NULL }
      };
  static bool initdone = false;
  static char * lang   = NULL;

  if(initdone == true)
    if(lang == NULL || strcmp(lang, globals.language))
      { // Language has changed since last initialization.
        free(lang);
        initdone = false;
      }
  
  if(initdone == false)
    {
      lang = strdup(globals.language);
      caves[0].displayname = gettext("the Deep Cave");
      caves[1].displayname = gettext("the Well");
      caves[2].displayname = gettext("the Antiquitarian Land");
      caves[3].displayname = gettext("the Beginners Cave");
      initdone = true;
    }
  
  char * displayname;

  displayname = NULL;

  for(int i = 0; displayname == NULL && caves[i].cave_name != NULL; i++)
    if(!strcmp(caves[i].cave_name, cave_name))
      displayname = caves[i].displayname;

  if(displayname == NULL)
    displayname = cave_name;
  
  return displayname;
}
