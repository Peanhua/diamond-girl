/*
  Diamond Girl - Game where player collects diamonds.
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
#include <assert.h>
#include <bzlib.h>
#include <errno.h>
#include <unistd.h>


bool write_file(const char * filename, char * buffer, int size)
{
  assert(filename != NULL);

  bool   success;
  FILE * fp;
  char   fn[strlen(filename) + strlen(".bz2") + 1];

  success = true;

  strcpy(fn, filename);
  strcat(fn, ".bz2");

  fp = fopen(fn, "wb");
  if(fp != NULL)
    {
      BZFILE * bfp;
      int bzerror;

      bzerror = BZ_OK;

      bfp = BZ2_bzWriteOpen(&bzerror, fp, 1, 0, 0);
      if(bzerror == BZ_OK)
        {
          BZ2_bzWrite(&bzerror, bfp, buffer, size);
          if(bzerror != BZ_OK)
            {
              success = false;
              fprintf(stderr, "%s(): Compression write failed: %d\n", __FUNCTION__, bzerror);
            }

          BZ2_bzWriteClose(&bzerror, bfp, 0, NULL, NULL);
          if(success == true)
            if(bzerror != BZ_OK)
              {
                success = false;
                fprintf(stderr, "%s(): Compression write close failed: %d\n", __FUNCTION__, bzerror);
              }
        }
      else
        {
          success = false;
          fprintf(stderr, "%s(): Compression write initialization failed: %d\n", __FUNCTION__, bzerror);
        }
        
      fclose(fp);

      /* Don't keep broken files around. */
      if(success == false)
        unlink(fn);
    }
  else
    {
      fprintf(stderr, "%s(): Failed to open '%s' for writing: %s\n", __FUNCTION__, fn, strerror(errno));
      success = false;
    }

  return success;
}
