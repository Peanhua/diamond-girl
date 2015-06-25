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


#include "image.h"
#include <png.h>
#include <zlib.h>
#include <assert.h>

void image_save(struct image * image, const char * filename)
{
  FILE * fp;

  assert(image != NULL);
  assert(filename != NULL);

  fp = fopen(filename, "wb");
  assert(fp != NULL);
  if(fp != NULL)
    {
      png_structp png;

      png = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL, NULL, NULL); //user_error_ptr, user_error_fn, user_warning_fn);
      assert(png != NULL);
      if(png != NULL)
        {
          png_infop info;

          info = png_create_info_struct(png);
          assert(info != NULL);
          if(info != NULL)
            {
              png_init_io(png, fp);

              png_set_compression_level(png, Z_BEST_SPEED);

              int ctype, bpp;

              if(image->flags & IF_ALPHA)
                {
                  ctype = PNG_COLOR_TYPE_RGB_ALPHA;
                  bpp = 4;
                }
              else
                {
                  ctype = PNG_COLOR_TYPE_RGB;
                  bpp = 3;
                }
              png_set_IHDR(png, info, image->width, image->height, 8, ctype,
                           PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

              png_bytepp row_pointers;

              row_pointers = png_malloc(png, image->height * sizeof(png_bytep));
              assert(row_pointers != NULL);
              if(row_pointers != NULL)
                {
                  for (int i = 0; i < image->height; i++)
                    row_pointers[i] = image->data + (image->height - i - 1) * image->width * bpp;
                  png_set_rows(png, info, row_pointers);
                              
                  png_write_png(png, info, 0, NULL);
                  png_write_end(png, info);
                              
                  png_free(png, row_pointers);

                  png_destroy_write_struct(&png, &info);
                }
            }
          else
            png_destroy_write_struct(&png, (png_infopp) NULL);
        }
                    
      fclose(fp);
    }
}
