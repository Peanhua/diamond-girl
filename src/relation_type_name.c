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

#include "quest.h"
#include <assert.h>
#include <libintl.h>

char const * relation_type_name(enum RELATION_TYPE type)
{
  char const * rv;

  rv = NULL;
  switch(type)
    {
    case RELATION_TYPE_GREAT_GRANDMOTHER: rv = gettext("great grandmother"); break;
    case RELATION_TYPE_GREAT_GRANDFATHER: rv = gettext("great grandfather"); break;
    case RELATION_TYPE_GRANDMOTHER:       rv = gettext("grandmother");       break;
    case RELATION_TYPE_GRANDFATHER:       rv = gettext("grandfather");       break;
    case RELATION_TYPE_MOTHER:            rv = gettext("mother");            break;
    case RELATION_TYPE_FATHER:            rv = gettext("father");            break;
    case RELATION_TYPE_SISTER:            rv = gettext("sister");            break;
    case RELATION_TYPE_BROTHER:           rv = gettext("brother");           break;
    case RELATION_TYPE_AUNT:              rv = gettext("aunt");              break;
    case RELATION_TYPE_UNCLE:             rv = gettext("uncle");             break;
    case RELATION_TYPE_COUSIN:            rv = gettext("cousin");            break;
    case RELATION_TYPE_RELATIVE:          rv = gettext("relative");          break;
    case RELATION_TYPE_FEMALE_FRIEND:     rv = gettext("friend");            break;
    case RELATION_TYPE_MALE_FRIEND:       rv = gettext("friend");            break;
    case RELATION_TYPE_ANCIENT_QUEEN:     rv = gettext("queen");             break;
    case RELATION_TYPE_ANCIENT_KING:      rv = gettext("king");              break;
    case RELATION_TYPE_ANCIENT_PRINCESS:  rv = gettext("princess");          break;
    case RELATION_TYPE_ANCIENT_PRINCE:    rv = gettext("prince");            break;
    case RELATION_TYPE_ANCIENT_KNIGHT:    rv = gettext("knight");            break;
    case RELATION_TYPE_ANCIENT_WAR_HERO:  rv = gettext("war hero");          break;
    case RELATION_TYPE_ANCIENT_PRIEST:    rv = gettext("priest");            break;
    case RELATION_TYPE_ANCIENT_SHAMAN:    rv = gettext("shaman");            break;
    case RELATION_TYPE_SIZEOF_:           assert(0);                         break;
    }

  return rv;
}
