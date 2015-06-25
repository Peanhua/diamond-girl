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

#include "map.h"

#include <assert.h>
#if HAVE_LIBMHASH
# include <mhash.h>
#elif HAVE_LIBCRYPTO
# include <openssl/md5.h>
#endif

char * map_calculate_hash(struct map * map)
{
  char * rv;

  rv = NULL;
  assert(map != NULL);

  unsigned char * hash;
  int             hash_size;

  hash      = NULL;
  hash_size = 0;
  
#if HAVE_LIBMHASH
  hash_size = mhash_get_block_size(MHASH_MD5);
  hash = malloc(hash_size);
  assert(hash != NULL);
  if(hash != NULL)
    {
      MHASH td;

      td = mhash_init(MHASH_MD5);
      if(td != MHASH_FAILED)
        { /* Hash together all the relevant data. */
          mhash(td, map->cave_name,        strlen(map->cave_name)     );
          mhash(td, &map->level,           sizeof map->level          );
          mhash(td, &map->is_intermission, sizeof map->is_intermission);
          mhash(td, &map->width,           sizeof map->width          );
          mhash(td, &map->height,          sizeof map->height         );
          mhash(td, &map->start_x,         sizeof map->start_x        );
          mhash(td, &map->start_y,         sizeof map->start_y        );
          mhash(td, &map->exit_x,          sizeof map->exit_x         );
          mhash(td, &map->exit_y,          sizeof map->exit_y         );
          mhash(td, &map->diamonds,        sizeof map->diamonds       );
          mhash(td, &map->diamonds_needed, sizeof map->diamonds_needed);
          mhash(td, &map->diamond_score,   sizeof map->diamond_score  );
          mhash(td, &map->time_score,      sizeof map->time_score     );
          mhash(td, map->data,             map->width * map->height   );
          mhash(td, &map->time,            sizeof map->time           );
          mhash(td, &map->ameba_time,      sizeof map->ameba_time     );
          mhash(td, &map->game_speed,      sizeof map->game_speed     );
          
          mhash_deinit(td, hash);
        }
      else
        {
          free(hash);
          hash = NULL;
        }
    }

#elif HAVE_LIBCRYPTO
  MD5_CTX context;

  hash_size = MD5_DIGEST_LENGTH;
  hash = malloc(hash_size);
  assert(hash != NULL);
  if(hash != NULL)
    {
      if(MD5_Init(&context))
        {
          int success;
          
          success             = MD5_Update(&context, map->cave_name,        strlen(map->cave_name)     );
          if(success) success = MD5_Update(&context, &map->level,           sizeof map->level          );
          if(success) success = MD5_Update(&context, &map->is_intermission, sizeof map->is_intermission);
          if(success) success = MD5_Update(&context, &map->width,           sizeof map->width          );
          if(success) success = MD5_Update(&context, &map->height,          sizeof map->height         );
          if(success) success = MD5_Update(&context, &map->start_x,         sizeof map->start_x        );
          if(success) success = MD5_Update(&context, &map->start_y,         sizeof map->start_y        );
          if(success) success = MD5_Update(&context, &map->exit_x,          sizeof map->exit_x         );
          if(success) success = MD5_Update(&context, &map->exit_y,          sizeof map->exit_y         );
          if(success) success = MD5_Update(&context, &map->diamonds,        sizeof map->diamonds       );
          if(success) success = MD5_Update(&context, &map->diamonds_needed, sizeof map->diamonds_needed);
          if(success) success = MD5_Update(&context, &map->diamond_score,   sizeof map->diamond_score  );
          if(success) success = MD5_Update(&context, &map->time_score,      sizeof map->time_score     );
          if(success) success = MD5_Update(&context, map->data,             map->width * map->height   );
          if(success) success = MD5_Update(&context, &map->time,            sizeof map->time           );
          if(success) success = MD5_Update(&context, &map->ameba_time,      sizeof map->ameba_time     );
          if(success) success = MD5_Update(&context, &map->game_speed,      sizeof map->game_speed     );
          
          if(success) success = MD5_Final(hash, &context);

          if(!success)
            {
              free(hash);
              hash = NULL;
            }
        }
      else
        {
          free(hash);
          hash = NULL;
        }
    }
#endif

  if(hash != NULL)
    {
      rv = malloc(hash_size * 2 + 1);
      assert(rv != NULL);
      if(rv != NULL)
        for(int i = 0; i < hash_size; i++)
          sprintf(rv + i * 2, "%.2x", hash[i]);
      free(hash);
    }
  
  return rv;
}
