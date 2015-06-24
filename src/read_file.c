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


bool read_file(const char * filename, char ** buffer_p, int * size_p)
{
  const int inbuf_size_step = 1024;

  bool   success;
  FILE * fp;
  char * inbuf;
  int    inbuf_size;
  int    inbuf_pos;

  assert(buffer_p != NULL);
  assert(size_p != NULL);

  *buffer_p = inbuf     = NULL;
  *size_p   = inbuf_pos = 0;
  inbuf_size            = 0;

  success = true;

  fp = fopen(filename, "rb");
  if(fp != NULL)
    {
      bool done;

      inbuf      = malloc(inbuf_size_step + 1);
      inbuf_size = inbuf_size_step;
      inbuf_pos  = 0;
      done = false;
      while(inbuf != NULL && success == true && done == false)
        {
          int rlen;

          rlen = fread(inbuf + inbuf_pos, 1, inbuf_size_step, fp);
          if(rlen > 0)
            {
              inbuf_pos += rlen;
              inbuf[inbuf_pos] = '\0';

              if(rlen == inbuf_size_step)
                {
                  char * tmp;
                  
                  tmp = realloc(inbuf, inbuf_size + inbuf_size_step + 1);
                  assert(tmp != NULL);
                  if(tmp != NULL)
                    {
                      inbuf = tmp;
                      inbuf_size += inbuf_size_step;
                    }
                  else
                    {
                      fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
                      success = false;
                    }
                }
            }
          else
            {
              if(feof(fp))
                done = true;
              else
                {
                  fprintf(stderr, "%s(): Failed to read from '%s': %s\n", __FUNCTION__, filename, strerror(errno));
                  success = false;
                }
            }
        }
      fclose(fp);
    }
  else
    { /* Try compressed file. */
      char fn[strlen(filename) + strlen(".bz2") + 1];

      strcpy(fn, filename);
      strcat(fn, ".bz2");
      
      fp = fopen(fn, "rb");
      if(fp != NULL)
        {
          BZFILE * bfp;
          int bzerror;

          bzerror = BZ_OK;
      
          bfp = BZ2_bzReadOpen(&bzerror, fp, 0, 0, NULL, 0);
          if(bzerror == BZ_OK)
            {
              inbuf      = malloc(inbuf_size_step + 1);
              inbuf_size = inbuf_size_step;
              inbuf_pos  = 0;
              while(inbuf != NULL && bzerror == BZ_OK)
                {
                  int rlen;

                  rlen = BZ2_bzRead(&bzerror, bfp, inbuf + inbuf_pos, inbuf_size_step);
                  if(bzerror == BZ_OK || bzerror == BZ_STREAM_END)
                    {
                      inbuf_pos += rlen;
                      inbuf[inbuf_pos] = '\0';

                      if(rlen == inbuf_size_step)
                        {
                          char * tmp;

                          tmp = realloc(inbuf, inbuf_size + inbuf_size_step + 1);
                          assert(tmp != NULL);
                          if(tmp != NULL)
                            {
                              inbuf = tmp;
                              inbuf_size += inbuf_size_step;
                            }
                          else
                            {
                              fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
                              success = false;
                            }
                        }
                    }
                  else
                    {
                      fprintf(stderr, "%s(): Compression read failed from '%s': %d\n", __FUNCTION__, filename, bzerror);
                      success = false;
                    }
                }

              BZ2_bzReadClose(&bzerror, bfp);
            }
          else
            {
              fprintf(stderr, "%s(): Compression read initialization failed: %d\n", __FUNCTION__, bzerror);
              success = false;
            }

          fclose(fp);
        }
      else
        {
          success = false;
        }
    }

  if(success == true)
    {
      assert(inbuf != NULL);
      assert(inbuf_pos > 0);

      *buffer_p = inbuf;
      *size_p   = inbuf_pos;
    }
  else
    {
      free(inbuf);
    }

  return success;
}
