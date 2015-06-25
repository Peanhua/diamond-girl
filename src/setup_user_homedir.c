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

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#if defined(WIN32)
#include <direct.h>
#include <shlobj.h>
#endif

#if !defined(AMIGA)

#if defined(WIN32)
static char hdname[MAX_PATH + 32];
#else
static char hdname[4096];
#endif


char * get_user_homedir(void)
{
  return hdname;
}
#endif


bool setup_user_homedir(void)
{
  bool homedir_ok;

  homedir_ok = false;

#if defined(AMIGA)

  homedir_ok = true;

#elif defined(WIN32)
  struct stat sb;
  char tmp[MAX_PATH];

  SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, tmp);
  snprintf(hdname, sizeof hdname, "%s/Diamond Girl", tmp);
  
  mkdir(hdname);
  if(stat(hdname, &sb) == 0)
    {
      char * p;

      homedir_ok = true;
      do
        {
          p = strchr(hdname, '\\');
          if(p != NULL)
            *p = '/';
        } while(p != NULL);
    }
  else
    fprintf(stderr, "Failed to setup working directory %s.\n", hdname);

#else

  { /* setup working directory to $HOME/.diamond_girl, making sure it exists, set homedir_ok variable accordingly */
    char * hd;

    hd = getenv("HOME");
    if(hd != NULL)
      {
	struct stat sb;

        assert(strlen(hd) + 1 < sizeof hdname);
        if(strlen(hd) + strlen("/.diamond_girl") + 1 < sizeof hdname)
          {
            snprintf(hdname, sizeof hdname, "%s/.diamond_girl", hd);
            mkdir(hdname, S_IRWXU);
            if(stat(hdname, &sb) == 0)
              {
                homedir_ok = true;
              }
            else
              fprintf(stderr, "Failed to setup working directory %s.\n", hdname);
          }
        else
          fprintf(stderr, "Failed to setup working directory: path to HOME too long (%d, max=%d).\n", (int) strlen(hd), (int) sizeof hdname - (int) strlen("/.diamond_girl") - 1);
      }
    else
      fprintf(stderr, "Failed to get environment variable HOME: %s\n", strerror(errno));
  }

#endif
  
  if(homedir_ok == true)
    { /* The party subdirectory. */
      char dn[sizeof hdname];

      snprintf(dn, sizeof dn, "%s/party", hdname);
#if defined(WIN32)
      mkdir(dn);
#else
      mkdir(dn, S_IRWXU);
#endif
    }


  return homedir_ok;
}
