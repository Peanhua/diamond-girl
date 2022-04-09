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
#include "girl.h"
#include <assert.h>

static void add(void * data, int len);

static char buf[sizeof(struct girl)];
static int pos;


bool girl_save(struct girl * girl)
{
  bool rv;
  char filename[1024];

  snprintf(filename, sizeof filename, "party/%s", girl->name);
  pos = 0;
  add(GIRLFILE_HEADER_V2,       strlen(GIRLFILE_HEADER_V2));
  add(girl->name,               strlen(girl->name) + 1);
  add(&girl->birth_time,        sizeof girl->birth_time);
  add(&girl->traits,            sizeof girl->traits);
  add(&girl->possible_traits,   sizeof girl->possible_traits);
  add(&girl->diamonds,          sizeof girl->diamonds);
  add(&girl->diamonds_to_trait, sizeof girl->diamonds_to_trait);
  rv = write_file(get_save_filename(filename), buf, pos);

  return rv;
}

static void add(void * data, int len)
{
  assert(pos + len < (int) sizeof buf);
  memcpy(&buf[pos], data, len);
  pos += len;
}
