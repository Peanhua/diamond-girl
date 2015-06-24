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

#include "map.h"
#include "image.h"
#include "globals.h"
#include <assert.h>

#define SCALE 3

struct image * map_thumbnail_image(struct map * map)
{
  struct image * image;

  image = NULL;
  if(map != NULL && map->width > 0 && map->height > 0)
    {
      int imgwidth;

      imgwidth = map->width * SCALE;
      imgwidth += 4 - (imgwidth % 4);

      image = image_new(imgwidth, map->height * SCALE, true);
      if(image != NULL)
        {
          struct
          {
            enum MAP_GLYPH glyph;
            uint8_t rgb[3];
          } colours[MAP_SIZEOF_] =
              {
                { MAP_EMPTY,             { 0x00, 0x00, 0x00 } },
                { MAP_BORDER,            { 0x00, 0x00, 0x00 } },
                { MAP_ILLEGAL,           { 0x00, 0x00, 0x00 } },
                { MAP_SAND,              { 0xe0, 0x9b, 0x14 } },
                { MAP_PLAYER,            { 0x00, 0x00, 0xff } },
                { MAP_PLAYER_ARMOUR0,    { 0x00, 0x00, 0x00 } },
                { MAP_PLAYER_ARMOUR1,    { 0x00, 0x00, 0x00 } },
                { MAP_PLAYER_ARMOUR2,    { 0x00, 0x00, 0x00 } },
                { MAP_BOULDER,           { 0xb8, 0xb8, 0xb8 } },
                { MAP_BOULDER_FALLING,   { 0x00, 0x00, 0x00 } },
                { MAP_DIAMOND,           { 0xff, 0xff, 0xff } },
                { MAP_DIAMOND_FALLING,   { 0xff, 0xff, 0xff } },
                { MAP_ENEMY1,            { 0xff, 0x00, 0x00 } },
                { MAP_ENEMY2,            { 0xff, 0x00, 0x00 } },
                { MAP_SMOKE,             { 0x00, 0x00, 0x00 } },
                { MAP_SMOKE1,            { 0x00, 0x00, 0x00 } },
                { MAP_SMOKE2,            { 0x00, 0x00, 0x00 } },
                { MAP_EXIT_LEVEL,        { 0x80, 0x80, 0x80 } },
                { MAP_EXTRA_LIFE_ANIM,   { 0x00, 0x00, 0x00 } },
                { MAP_BRICK,             { 0x80, 0x80, 0x80 } },
                { MAP_BRICK_EXPANDING,   { 0x80, 0x80, 0x80 } },
                { MAP_BRICK_UNBREAKABLE, { 0x70, 0x70, 0x70 } },
                { MAP_BRICK_MORPHER,     { 0x80, 0x80, 0x80 } },
                { MAP_AMEBA,             { 0x00, 0xff, 0x00 } },
                { MAP_SLIME,             { 0x00, 0x00, 0xff } }
              };

#ifndef NDEBUG
          for(int i = 0; i < MAP_SIZEOF_; i++)
            {
              assert(colours[i].glyph == (enum MAP_GLYPH) i);
            }
#endif
          memset(image->data, 0, image->width * image->height * 4);

          for(int y = 0; y < map->height; y++)
            for(int x = 0; x < map->width; x++)
              {
                enum MAP_GLYPH glyph;

                glyph = map->data[x + (map->height - y - 1) * map->width];
                assert(glyph < MAP_SIZEOF_);
                for(int my = 0; my < SCALE; my++)
                  for(int mx = 0; mx < SCALE; mx++)
                    {
                      int pos;

                      pos = (mx + x * SCALE + (my + y * SCALE) * image->width) * 4;
                      image->data[pos + 0] = colours[glyph].rgb[0];
                      image->data[pos + 1] = colours[glyph].rgb[1];
                      image->data[pos + 2] = colours[glyph].rgb[2];
                      image->data[pos + 3] = 0xa0;
                    }
              }
#ifdef WITH_OPENGL
          if(globals.opengl)
            image_to_texture(image, true, true, true);
#endif
        }
    }

  return image;
}
