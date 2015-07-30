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
#include "themes.h"
#include <unistd.h>
#include <assert.h>

#if defined(WIN32)
#include <sys/stat.h>
#include <windows.h>
#include <direct.h>
#include <winreg.h>
#endif



const char * get_data_filename(const char * name)
{
  static char buf[1024 * 4];

#if defined(AMIGA)
  snprintf(buf, sizeof buf, "PROGDIR:");
#elif defined(WIN32)
  {
    long n;
    HKEY key;

    n = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     "Software\\Diamond_Girl",
                     0,
                     KEY_READ,
                     &key);
    assert(n == ERROR_SUCCESS);
    if(n == ERROR_SUCCESS)
      {
        unsigned long type, size;
        
        type = REG_SZ;
        size = sizeof buf;
        n = RegQueryValueEx(key,
                            "Install_Dir",
                            NULL,
                            &type,
                            (unsigned char *) buf,
                            &size);
        RegCloseKey(key);

        assert(n == ERROR_SUCCESS);
        if(n != ERROR_SUCCESS)
          {
            fprintf(stderr, "%s(): Failed to obtain the Install_Dir registry value.\n", __FUNCTION__);
            strcpy(buf, "");
          }
      }
    else
      {
        fprintf(stderr, "%s(): Failed to obtain the Software/Diamond_Girl registry key.\n", __FUNCTION__);
        strcpy(buf, "");
      }
  }
#else
  snprintf(buf, sizeof buf, "%s/diamond-girl", DIAMOND_GIRL_DATADIR);
#endif

  /* Get from theme specific directories if applicable. */
  if(!strncmp(name, "gfx/", strlen("gfx/")) || !strncmp(name, "sfx/", strlen("sfx/")) || !strncmp(name, "txt/", strlen("txt/")))
    { /* Check first the current theme directory, if not found, then the 'default' theme directory. */
      char tmp[sizeof buf];
      struct theme ** themes;
      bool done;

      done = false;

      tmp[0] = '\0';
      themes = themes_get();
      for(int i = 0; done == false && i <= THEME_DEFAULT; i++)
        if(themes[i] != NULL)
          {
            snprintf(tmp, sizeof tmp, "%s/themes/%s/%s", buf, themes[i]->directory, name);
            if(access(tmp, R_OK) == 0)
              {
                strcpy(buf, tmp);
                done = true;
              }
          }
      if(done == false) /* If not found, return the default theme (last try) anyway. */
        strcpy(buf, tmp);
    }
  else
    { /* This is not a theme specific file. */
      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "/%s", name);
    }

  return buf;
}
