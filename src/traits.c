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
#include "traits.h"
#include "themes.h"
#include "girl.h"
#include "event.h"
#include "game.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>


#define FILE_HEADER_V2 "dgt2"
#define FILE_HEADER_V3 "dgt3"

static trait_t  traits_active;
static trait_t  traits_available;
static uint64_t traits_score;

void traits_initialize(void)
{
  FILE * fp;

  traits_active    = 0;
  traits_available = 0;
  traits_score     = 0;

  fp = fopen(get_save_filename("traits.dat"), "rb");
  if(fp != NULL)
    {
      int version;
      char header[4];

      version = 1; /* Version 1 did not have any header. */
      if(fread(header, 4, 1, fp) == 1) /* Header will always be 4 characters. */
        {
          if(memcmp(header, FILE_HEADER_V2, 4) == 0)
            version = 2;
          else if(memcmp(header, FILE_HEADER_V3, 4) == 0)
            version = 3;
        }

      if(version == 1)
        {
          uint8_t tmp;

          rewind(fp);

          fread(&tmp, sizeof tmp, 1, fp);
          traits_active = tmp;

          fread(&tmp, sizeof tmp, 1, fp);
          traits_available = tmp;

          fread(&tmp, sizeof tmp, 1, fp);
          traits_score = tmp;
        }
      else if(version == 2)
        {
          uint16_t t;
          
          fread(&t, sizeof t, 1, fp); traits_active    = t;
          fread(&t, sizeof t, 1, fp); traits_available = t;
        }
      else if(version == 3)
        {
          fread(&traits_active,    sizeof traits_active,    1, fp);
          fread(&traits_available, sizeof traits_available, 1, fp);
        }
      fread(&traits_score,     sizeof traits_score,     1, fp);
      fclose(fp);
    }
}


void traits_cleanup(void)
{
  FILE * fp;

  fp = fopen(get_save_filename("traits.dat"), "wb");
  assert(fp != NULL);
  if(fp != NULL)
    {
      fwrite(FILE_HEADER_V3,    strlen(FILE_HEADER_V3),  1, fp);
      fwrite(&traits_active,    sizeof traits_active,    1, fp);
      fwrite(&traits_available, sizeof traits_available, 1, fp);
      fwrite(&traits_score,     sizeof traits_score,     1, fp);
      fclose(fp);
    }
  else
    fprintf(stderr, "Failed to open '%s' for writing: %s\n", get_save_filename("traits.dat"), strerror(errno));
}


trait_t traits_get(enum GAME_MODE game_mode)
{
  trait_t rv;

  rv = traits_active;
  if(game_mode == GAME_MODE_CLASSIC)
    rv &=
      TRAIT_KEY            |
      TRAIT_ADVENTURE_MODE |
      TRAIT_IRON_GIRL      |
      TRAIT_TIME_CONTROL   |
      TRAIT_STARS1         |
      TRAIT_STARS2         |
      TRAIT_STARS3         |
      TRAIT_RECYCLER       |
      TRAIT_PYJAMA_PARTY;

  /* No iron girl in party mode. */
  if(game_mode == GAME_MODE_PYJAMA_PARTY)
    rv &= ~TRAIT_IRON_GIRL;

  return rv;
}

trait_t traits_get_active(void)
{
  return traits_active;
}

trait_t traits_get_available(void)
{
  return traits_available;
}

uint64_t traits_get_score(void)
{
  return traits_score;
}


void traits_set(trait_t active, trait_t available, uint64_t score)
{
  traits_active    = active;
  traits_available = available;
  traits_score     = score;
}


void traits_activate(trait_t traits)
{
  traits_active |= traits;
  event_trigger(EVENT_TYPE_TRAIT_ACTIVATED, traits);
}

void traits_deactivate(trait_t traits)
{
  traits_active &= ~traits;
  event_trigger(EVENT_TYPE_TRAIT_DEACTIVATED, traits);
}

void traits_make_available(trait_t traits)
{
  traits_available |= traits;
}


void traits_add_score(uint64_t score)
{
  traits_score += score;

  /* Double trait score during special days. */
  if(theme_get(THEME_SPECIAL_DAY) != NULL)
    traits_score += score;
}

void traits_delete_score(uint64_t score)
{
  assert(score <= traits_score);
  assert(score > 0);

  traits_score -= score;
}
