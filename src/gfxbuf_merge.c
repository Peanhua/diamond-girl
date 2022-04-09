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

#ifdef WITH_OPENGL

#include "gfxbuf.h"

bool gfxbuf_merge(struct gfxbuf * destination, struct gfxbuf * source)
{
  bool rv;

  rv = false;
  if(destination->type           == source->type &&
     destination->primitive_type == source->primitive_type &&
     destination->options        == source->options)
    {
      if(gfxbuf_resize(destination, destination->vertices + source->vertices) == true)
        {
          for(unsigned int i = 0; i < source->vertices; i++)
            {
              for(unsigned int j = 0; j < source->ncoords; j++)
                destination->vbuf[(destination->vertices + i) * source->ncoords + j] = source->vbuf[i * source->ncoords + j];
              
              if(source->options & GFXBUF_NORMALS)
                for(unsigned int j = 0; j < 3; j++)
                  destination->nbuf[(destination->vertices + i) * 3 + j] = source->nbuf[i * 3 + j];

              if(source->options & GFXBUF_TEXTURE)
                for(unsigned int j = 0; j < 2; j++)
                  destination->tbuf[(destination->vertices + i) * 2 + j] = source->tbuf[i * 2 + j];
              
              if(source->options & GFXBUF_COLOUR)
                for(unsigned int j = 0; j < 4; j++)
                  destination->cbuf[(destination->vertices + i) * 4 + j] = source->cbuf[i * 4 + j];
            }
          destination->vertices += source->vertices;
          rv = true;
        }
    }
  
  return rv;
}

#endif
