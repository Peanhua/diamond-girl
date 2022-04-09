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

#ifndef TREASURE_H_
#define TREASURE_H_

enum TREASURE_TYPE
  { /* Bigger objects: */
    TREASURE_TYPE_MIRROR,
    TREASURE_TYPE_STATUE,
    TREASURE_TYPE_VASE,

    /* Smaller, jewelry: */
    TREASURE_TYPE_BRACELET,
    TREASURE_TYPE_EARRING,
    TREASURE_TYPE_HAND_MIRROR,
    TREASURE_TYPE_NECKLACE,
    TREASURE_TYPE_PENDANT,
    TREASURE_TYPE_RING,
    
    TREASURE_TYPE_SIZEOF_
  };

enum TREASURE_MATERIAL
  {
    TREASURE_MATERIAL_BRONZE,
    TREASURE_MATERIAL_COPPER,
    TREASURE_MATERIAL_GOLD,
    TREASURE_MATERIAL_SILVER,
    TREASURE_MATERIAL_SIZEOF_
  };

enum TREASURE_GEMSTONE
  {
    TREASURE_GEMSTONE_NONE,
    TREASURE_GEMSTONE_AMBER,
    TREASURE_GEMSTONE_AMETHYST,
    TREASURE_GEMSTONE_DIAMOND,
    TREASURE_GEMSTONE_EMERALD,
    TREASURE_GEMSTONE_JADE,
    TREASURE_GEMSTONE_OPAL,
    TREASURE_GEMSTONE_RUBY,
    TREASURE_GEMSTONE_SAPPHIRE,
    TREASURE_GEMSTONE_TURQUOISE,
    TREASURE_GEMSTONE_SIZEOF_
  };

struct treasure
{
  enum TREASURE_TYPE     type;
  enum TREASURE_MATERIAL material;
  enum TREASURE_GEMSTONE gemstones;

  char *                 name;
};


extern struct treasure * treasure_new(enum TREASURE_TYPE type, enum TREASURE_MATERIAL material, enum TREASURE_GEMSTONE gemstones);
extern struct treasure * treasure_free(struct treasure * object);

extern char * treasure_type_name(enum TREASURE_TYPE type);
extern char * treasure_material_name(enum TREASURE_MATERIAL material);
extern char * treasure_gemstone_name(enum TREASURE_GEMSTONE gemstone);
extern char * treasure_longname(struct treasure * treasure);
extern char * treasure_filename(struct treasure * treasure, char const * suffix); /* For locating 2d and 3d image files. */
extern unsigned int treasure_price(struct treasure const * treasure);

#endif
