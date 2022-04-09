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
#include "gc.h"
#include "globals.h"
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

static char * hdname = NULL;


char * get_user_homedir(void)
{
  return hdname;
}
#endif


bool setup_user_homedir(void)
{
  bool homedir_ok;
  int hdname_size;

  homedir_ok = false;
  hdname_size = 0;
  
#if defined(AMIGA)

  homedir_ok = true;

#elif defined(WIN32)
  struct stat sb;
  char tmp[MAX_PATH];

  SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, tmp);
  hdname_size = strlen(tmp) + 128;
  hdname = malloc(hdname_size);
  assert(hdname != NULL);
  if(hdname != NULL)
    {
      snprintf(hdname, hdname_size, "%s/Diamond Girl%s", tmp, SAVE_DIR_NAME_APPEND);

      if(globals.read_only == false)
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
    }
  else
    fprintf(stderr, "Failed to allocate memory: %s\n", strerror(errno));

#else

  { /* setup working directory to $HOME/.diamond_girl, making sure it exists, set homedir_ok variable accordingly */
    char * hd;

    hd = getenv("HOME");
    if(hd != NULL)
      {
	struct stat sb;

        hdname_size = strlen(hd) + 128;
        hdname = malloc(hdname_size);
        assert(hdname != NULL);
        if(hdname != NULL)
          {
            snprintf(hdname, hdname_size, "%s/.diamond_girl%s", hd, SAVE_DIR_NAME_APPEND);
            assert(globals.read_only == false);
            if(globals.read_only == false)
              mkdir(hdname, S_IRWXU);
            if(stat(hdname, &sb) == 0)
              {
                homedir_ok = true;
              }
            else
              fprintf(stderr, "Failed to setup working directory %s.\n", hdname);
          }
        else
          fprintf(stderr, "Failed to allocate memory: %s\n", strerror(errno));
      }
    else
      fprintf(stderr, "Failed to get environment variable HOME: %s\n", strerror(errno));
  }

#endif

  if(hdname != NULL)
    gc_new(GCT_STRING, hdname);
  
  if(homedir_ok == true)
    { /* The party subdirectory. */
      assert(hdname_size > 0);
      char dn[hdname_size];

      snprintf(dn, sizeof dn, "%s/party", hdname);
      if(globals.read_only == false)
        {
#if defined(WIN32)
          mkdir(dn);
#else
          mkdir(dn, S_IRWXU);
#endif
        }
    }
  assert(homedir_ok == true);

  return homedir_ok;
}
