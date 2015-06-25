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

#include "globals.h"
#include <libintl.h>
#include <locale.h>
#ifdef WIN32
#include <stdlib.h>
#endif

bool set_language(const char * locale)
{
  bool rv;
  char * default_locale = "en_US";
  
  rv = false;

  if(locale == NULL)
    locale = default_locale;
  
  free(globals.locale);
  globals.locale = strdup(locale);

  free(globals.language);
  globals.language = strdup(locale);

  if(globals.locale != NULL && globals.language != NULL)
    {
      char * underscore;
      
      underscore = strchr(globals.language, '_');
      if(underscore != NULL)
        *underscore = '\0';
      

#ifdef WIN32
      /* setlocale() doesn't seem to have effect in Windows, but setting environment variable works */
      static char envvar[1024];

      snprintf(envvar, sizeof envvar, "LC_MESSAGES=%s", globals.locale);
      if(putenv(envvar) == 0)
        rv = true;
#else
      char * tmp;
      
      tmp = setlocale(LC_MESSAGES, globals.locale);
      if(tmp != NULL)
        rv = true;
#endif
    }

  return rv;
}
