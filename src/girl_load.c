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
#include "girl.h"
#include <assert.h>

struct girl * girl_load(const char * filename)
{
  struct girl * girl;

  girl = girl_new();
  assert(girl != NULL);
  if(girl != NULL)
    {
      char * buf;
      int    bufsize;
      bool   ok;
      char fn[1024];

      snprintf(fn, sizeof fn, "party/%s", filename);
      ok = read_file(get_save_filename(fn), &buf, &bufsize);
      assert(ok == true);
      if(ok == true)
        {
          char * p;
          int version;

          p = buf;

          /* header */
          version = 0;
          assert(bufsize >= (int) strlen(GIRLFILE_HEADER_V1));
          if(memcmp(buf, GIRLFILE_HEADER_V1, strlen(GIRLFILE_HEADER_V1)) == 0)
            version = 1;
          else if(memcmp(buf, GIRLFILE_HEADER_V2, strlen(GIRLFILE_HEADER_V2)) == 0)
            version = 2;
          p += strlen(GIRLFILE_HEADER_V1);

          if(version >= 1)
            {
              /* name */
              for(int i = 0; *p != '\0' && i < bufsize; i++)
                {
                  girl->name[i] = *p;
                  girl->name[i + 1] = '\0';
                  p++;
                }
              assert(*p == '\0');
              if(*p == '\0')
                p++;
          
              /* birth_time */
              assert(p - buf + sizeof girl->birth_time <= (size_t) bufsize);
              memcpy(&girl->birth_time, p, sizeof girl->birth_time);
              p += sizeof girl->birth_time;

              /* traits */
              if(version == 1)
                { /* v1 had sizeof trait_t uint16_t */
                  uint16_t t;

                  assert(p - buf + sizeof t <= (size_t) bufsize);
                  memcpy(&t, p, sizeof t);
                  girl->traits = t;
                  p += sizeof t;
                }
              else /* if(version >= 2) */
                {
                  assert(p - buf + sizeof girl->traits <= (size_t) bufsize);
                  memcpy(&girl->traits, p, sizeof girl->traits);
                  p += sizeof girl->traits;
                }

              /* possible_traits */
              if(version == 1)
                { /* v1 had sizeof trait_t uint16_t */
                  uint16_t t;

                  assert(p - buf + sizeof t <= (size_t) bufsize);
                  memcpy(&t, p, sizeof t);
                  girl->possible_traits = t;
                  p += sizeof t;
                }
              else /* if(version >= 2) */
                {
                  assert(p - buf + sizeof girl->possible_traits <= (size_t) bufsize);
                  memcpy(&girl->possible_traits, p, sizeof girl->possible_traits);
                  p += sizeof girl->possible_traits;
                }                  

              /* diamonds */
              assert(p - buf + sizeof girl->diamonds <= (size_t) bufsize);
              memcpy(&girl->diamonds, p, sizeof girl->diamonds);
              p += sizeof girl->diamonds;

              /* diamonds_to_trait */
              assert(p - buf + sizeof girl->diamonds_to_trait <= (size_t) bufsize);
              memcpy(&girl->diamonds_to_trait, p, sizeof girl->diamonds_to_trait);
              p += sizeof girl->diamonds_to_trait;
            }
          else
            {
              fprintf(stderr, "Unable to load '%s': unknown file version.\n", get_save_filename(fn));
              girl = girl_free(girl);
            }
          free(buf);
        }
    }

  return girl;
}
