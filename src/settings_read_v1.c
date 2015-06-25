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
#include "ui.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

void settings_read_v1(void)
{
  FILE * fp;
	
  fp = fopen(get_save_filename("settings"), "r");
  if(fp != NULL)
    {
      int done;
	    
      done = 0;
      while(!done)
	{
	  char linebuf[1024];
		
	  if(fgets(linebuf, sizeof linebuf, fp) != NULL)
	    {
	      char * lf;
	      char var[1024], val[1024];
	      int i, j;
		    
	      lf = strchr(linebuf, '\n');
	      if(lf != NULL)
		*lf = '\0';
		    
	      for(i = 0, j = 0; linebuf[i] != '\0' && linebuf[i] != '='; i++)
		var[j++] = linebuf[i];
	      var[j] = '\0';
	      if(linebuf[i++] != '\0')
		{
		  for(j = 0; linebuf[i] != '\0'; i++)
		    val[j++] = linebuf[i];
		  val[j] = '\0';
			
		  if(!strcmp(var, "FULLSCREEN"))
		    globals.fullscreen = strtoul(val, NULL, 0);
		  else if(!strcmp(var, "SOUND"))
                    {
                      globals.use_sfx = strtoul(val, NULL, 0);
                      globals.use_music = globals.use_sfx;
                    }
		  else if(!strcmp(var, "MAP_SELECTION"))
                    fprintf(stderr, "Warning! Settings v1: MAP_SELECTION is ignored.\n");
		}
	    }
	  else
	    {
	      if(!feof(fp))
		fprintf(stderr, "Warning! An error occured while reading settings from '%s': %s\n", get_save_filename("settings"), strerror(errno));
	      done = 1;
	    }
	}
      fclose(fp);

      /* Remove the old file, we will be writing in different name when we save. */
      unlink(get_save_filename("settings"));

      /* Add bindings */
      ui_bindings_default(true);
    }
}
