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
#include "globals.h"
#include "names.h"
#include "quest.h"
#include "stack.h"
#include "treasure.h"
#include <assert.h>
#include <libintl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct treasure * treasure_new(enum TREASURE_TYPE type, enum TREASURE_MATERIAL material, enum TREASURE_GEMSTONE gemstones)
{
  struct treasure * object;

  object = malloc(sizeof *object);
  assert(object != NULL);
  if(object != NULL)
    {
      object->type      = type;
      object->material  = material;
      object->gemstones = gemstones;

      bool unique;

      unique = false;
      while(unique == false)
        {
          object->name = strdup(names_get(GENDER_NEUTER, 3, 20));
          unique = true;
          
          for(unsigned int i = 0; unique == true && i < globals.questlines_size; i++)
            {
              struct questline * ql;

              ql = globals.questlines[i];
              for(unsigned int j = 0; unique == true && j < ql->quests->size; j++)
                {
                  struct quest * q;

                  q = ql->quests->data[j];
                  if(!strcmp(q->treasure_object->name, object->name))
                    unique = false;
                }
            }
        }
    }

  return object;
}


struct treasure * treasure_free(struct treasure * object)
{
  assert(object != NULL);

  free(object->name);
  free(object);
  
  return NULL;
}

  
char * treasure_type_name(enum TREASURE_TYPE type)
{
  char * rv;

  rv = NULL;
  switch(type)
    {
    case TREASURE_TYPE_MIRROR:      rv = gettext("mirror");      break;
    case TREASURE_TYPE_STATUE:      rv = gettext("statue");      break;
    case TREASURE_TYPE_VASE:        rv = gettext("vase");        break;

    case TREASURE_TYPE_BRACELET:    rv = gettext("bracelet");    break;
    case TREASURE_TYPE_EARRING:     rv = gettext("earring");     break;
    case TREASURE_TYPE_HAND_MIRROR: rv = gettext("hand mirror"); break;
    case TREASURE_TYPE_NECKLACE:    rv = gettext("necklace");    break;
    case TREASURE_TYPE_PENDANT:     rv = gettext("pendant");     break;
    case TREASURE_TYPE_RING:        rv = gettext("ring");        break;

    case TREASURE_TYPE_SIZEOF_:     assert(0);                   break;
    }

  return rv;
}


char * treasure_material_name(enum TREASURE_MATERIAL material)
{
  char * rv;

  rv = NULL;
  switch(material)
    {
    case TREASURE_MATERIAL_BRONZE:  rv = gettext("bronze"); break;
    case TREASURE_MATERIAL_COPPER:  rv = gettext("copper"); break;
    case TREASURE_MATERIAL_GOLD:    rv = gettext("gold");   break;
    case TREASURE_MATERIAL_SILVER:  rv = gettext("silver"); break;
    case TREASURE_MATERIAL_SIZEOF_: assert(0);              break;
    }

  return rv;
}


char * treasure_gemstone_name(enum TREASURE_GEMSTONE gemstone)
{
  char * rv;
  static char * none = "";

  rv = NULL;
  switch(gemstone)
    {
    case TREASURE_GEMSTONE_NONE:      rv = none;                                   break;
    case TREASURE_GEMSTONE_AMBER:     rv = ngettext("amber",     "amber",      7); break;
    case TREASURE_GEMSTONE_AMETHYST:  rv = ngettext("amethyst",  "amethysts",  7); break;
    case TREASURE_GEMSTONE_DIAMOND:   rv = ngettext("diamond",   "diamonds",   7); break;
    case TREASURE_GEMSTONE_EMERALD:   rv = ngettext("emerald",   "emeralds",   7); break;
    case TREASURE_GEMSTONE_JADE:      rv = ngettext("jade",      "jades",      7); break;
    case TREASURE_GEMSTONE_OPAL:      rv = ngettext("opal",      "opals",      7); break;
    case TREASURE_GEMSTONE_RUBY:      rv = ngettext("ruby",      "rubies",     7); break;
    case TREASURE_GEMSTONE_SAPPHIRE:  rv = ngettext("sapphire",  "sapphires",  7); break;
    case TREASURE_GEMSTONE_TURQUOISE: rv = ngettext("turquoise", "turquoises", 7); break;
    case TREASURE_GEMSTONE_SIZEOF_:   assert(0);                                   break;
    }

  return rv;
}



char * treasure_longname(struct treasure * treasure)
{
  static char buf[1024];

  char embedding[128];

  if(treasure->gemstones == TREASURE_GEMSTONE_NONE)
    strcpy(embedding, "");
  else
    snprintf(embedding, sizeof embedding, gettext(" embedded with %s"), treasure_gemstone_name(treasure->gemstones));
  
  snprintf(buf, sizeof buf, gettext("%s %s '%s'%s"),
           treasure_material_name(treasure->material),
           treasure_type_name(treasure->type),
           treasure->name,
           embedding);

  return buf;
}


char * treasure_filename(struct treasure * treasure, char const * suffix)
{
  char typename[64];
  char materialname[64];
  char gemstonesname[64];

  switch(treasure->type)
    {
    case TREASURE_TYPE_MIRROR:      strcpy(typename, "mirror");      break;
    case TREASURE_TYPE_STATUE:      strcpy(typename, "statue");      break;
    case TREASURE_TYPE_VASE:        strcpy(typename, "vase");        break;
    case TREASURE_TYPE_BRACELET:    strcpy(typename, "bracelet");    break;
    case TREASURE_TYPE_EARRING:     strcpy(typename, "earring");     break;
    case TREASURE_TYPE_HAND_MIRROR: strcpy(typename, "hand_mirror"); break;
    case TREASURE_TYPE_NECKLACE:    strcpy(typename, "necklace");    break;
    case TREASURE_TYPE_PENDANT:     strcpy(typename, "pendant");     break;
    case TREASURE_TYPE_RING:        strcpy(typename, "ring");        break;
    case TREASURE_TYPE_SIZEOF_:     assert(0);                       break;
    }

  switch(treasure->material)
    {
    case TREASURE_MATERIAL_BRONZE:  strcpy(materialname, "bronze"); break;
    case TREASURE_MATERIAL_COPPER:  strcpy(materialname, "copper"); break;
    case TREASURE_MATERIAL_GOLD:    strcpy(materialname, "gold");   break;
    case TREASURE_MATERIAL_SILVER:  strcpy(materialname, "silver"); break;
    case TREASURE_MATERIAL_SIZEOF_: assert(0);                      break;
    }

  switch(treasure->gemstones)
    {
    case TREASURE_GEMSTONE_NONE:      strcpy(gemstonesname, "");          break;
    case TREASURE_GEMSTONE_AMBER:     strcpy(gemstonesname, "amber");     break;
    case TREASURE_GEMSTONE_AMETHYST:  strcpy(gemstonesname, "amethyst");  break;
    case TREASURE_GEMSTONE_DIAMOND:   strcpy(gemstonesname, "diamond");   break;
    case TREASURE_GEMSTONE_EMERALD:   strcpy(gemstonesname, "emerald");   break;
    case TREASURE_GEMSTONE_JADE:      strcpy(gemstonesname, "jade");      break;
    case TREASURE_GEMSTONE_OPAL:      strcpy(gemstonesname, "opal");      break;
    case TREASURE_GEMSTONE_RUBY:      strcpy(gemstonesname, "ruby");      break;
    case TREASURE_GEMSTONE_SAPPHIRE:  strcpy(gemstonesname, "sapphire");  break;
    case TREASURE_GEMSTONE_TURQUOISE: strcpy(gemstonesname, "turquoise"); break;
    case TREASURE_GEMSTONE_SIZEOF_:   assert(0);                          break;
    }

  static char fn[1024];

  if(strlen(gemstonesname) == 0)
    {
      if(!strcmp(suffix, "obj"))
        snprintf(fn, sizeof fn, "gfx/%s.%s", typename, suffix);
      else
        snprintf(fn, sizeof fn, "gfx/%s-%s.%s", typename, materialname, suffix);
    }
  else
    {
      if(!strcmp(suffix, "obj"))
        snprintf(fn, sizeof fn, "gfx/gemstone_%s.%s", typename, suffix);
      else
        snprintf(fn, sizeof fn, "gfx/gemstone_%s-%s-%s.%s", typename, materialname, gemstonesname, suffix);
    }

  return fn;
}


unsigned int treasure_price(struct treasure const * treasure)
{
  double cost;

  assert(treasure != NULL);
  cost = 0.0;
  switch(treasure->type)
    {
    case TREASURE_TYPE_MIRROR:      cost = 1.0; break;
    case TREASURE_TYPE_STATUE:      cost = 1.0; break;
    case TREASURE_TYPE_VASE:        cost = 0.7; break;

    case TREASURE_TYPE_BRACELET:    cost = 0.6; break;
    case TREASURE_TYPE_EARRING:     cost = 0.5; break;
    case TREASURE_TYPE_HAND_MIRROR: cost = 0.6; break;
    case TREASURE_TYPE_NECKLACE:    cost = 0.8; break;
    case TREASURE_TYPE_PENDANT:     cost = 0.8; break;
    case TREASURE_TYPE_RING:        cost = 0.5; break;

    case TREASURE_TYPE_SIZEOF_:     assert(0);  break;
    }
  
  switch(treasure->material)
    {
    case TREASURE_MATERIAL_BRONZE:  cost *= 0.50; break;
    case TREASURE_MATERIAL_COPPER:  cost *= 0.55; break;
    case TREASURE_MATERIAL_GOLD:    cost *= 1.00; break;
    case TREASURE_MATERIAL_SILVER:  cost *= 0.75; break;
    case TREASURE_MATERIAL_SIZEOF_: assert(0);    break;
    }
  
  switch(treasure->gemstones)
    {
    case TREASURE_GEMSTONE_NONE:      cost *= 0.50; break;
    case TREASURE_GEMSTONE_AMBER:     cost *= 0.91; break;
    case TREASURE_GEMSTONE_AMETHYST:  cost *= 0.90; break;
    case TREASURE_GEMSTONE_DIAMOND:   cost *= 1.00; break;
    case TREASURE_GEMSTONE_EMERALD:   cost *= 0.90; break;
    case TREASURE_GEMSTONE_JADE:      cost *= 0.89; break;
    case TREASURE_GEMSTONE_OPAL:      cost *= 0.78; break;
    case TREASURE_GEMSTONE_RUBY:      cost *= 0.94; break;
    case TREASURE_GEMSTONE_SAPPHIRE:  cost *= 0.93; break;
    case TREASURE_GEMSTONE_TURQUOISE: cost *= 0.92; break;
    case TREASURE_GEMSTONE_SIZEOF_:   assert(0);    break;
    }

  cost *= 200.0;

  return cost;
}
