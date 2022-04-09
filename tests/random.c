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

#include "../src/diamond_girl.h"

#if HAVE_LIBMHASH
# include <mhash.h>
#elif HAVE_LIBCRYPTO
# include <openssl/sha.h>
#else
# error Neither libmhash nor libcrypto defined to be present.
#endif

#ifdef WITH_BUNDLED_MTWIST
# define MT_GENERATE_CODE_IN_HEADER 0
# include "../mtwist-1.5/mtwist.h"
#else
# include <mtwist.h>
#endif

#include "../src/random.h"
#include "../src/random.c"


static int random_test(void);

#define TEST_OK         0
#define TEST_SKIP      77
#define TEST_FAIL       1
#define TEST_FAIL_HARD 99

int main(int argc DG_UNUSED, char *argv[] DG_UNUSED)
{
  return random_test();
}



static int random_test(void)
{
  int rv;

  rv = TEST_OK;
  
  const int nseeds    = 1000;
  const int blocksize = 1000;
  const int nvalues   = 200000 / blocksize;

  const char * correct_hash_string = "6e0863cdc9285e44d95beb96b0f6f7767dc936c9"; // For 1000x200000
  //const char * correct_hash_string = "c658344e85126c62f0b6305dfb7bb257e739a2d2"; // For 1000x100000
  //const char * correct_hash_string = "083138330d6bd49cd0b46bfd1bfc4746c5c50016"; // For 200x100000

  unsigned char * hash_digest;
  int             hash_digest_size;

#if HAVE_LIBMHASH
  hash_digest_size = mhash_get_block_size(MHASH_SHA1);
#elif HAVE_LIBCRYPTO
  hash_digest_size = SHA_DIGEST_LENGTH;
#endif

  hash_digest = malloc(hash_digest_size);
  if(hash_digest != NULL)
    {
#if HAVE_LIBMHASH
      MHASH hash;

      hash = mhash_init(MHASH_SHA1);
      if(hash != MHASH_FAILED)
        {
          for(int i = 0; rv == TEST_OK && i < nseeds; i++)
            {
              void * rs;
          
              rs = seed_rand_state(i);
              assert(rs != NULL);
              if(rs != NULL)
                {
                  for(int j = 0; rv == TEST_OK && j < nvalues; j++)
                    {
                      int block[blocksize];

                      for(int k = 0; k < blocksize; k++)
                        block[k] = get_rand_state(1000, rs);

                      mhash(hash, block, sizeof block);
                    }
                  free(rs);
                }
              else
                {
                  printf("Internal error: %s\n", "seed_rand_state");
                  rv = TEST_FAIL_HARD;
                }
            }

          if(rv == TEST_OK)
            {
              mhash_deinit(hash, hash_digest);
            }
        }
      else
        {
          printf("Internal error: %s\n", "mhash_init");
          rv = TEST_FAIL_HARD;
        }

#elif HAVE_LIBCRYPTO
      SHA_CTX context;

      if(SHA1_Init(&context))
        {
          for(int i = 0; rv == TEST_OK && i < nseeds; i++)
            {
              void * rs;
          
              rs = seed_rand_state(i);
              assert(rs != NULL);
              if(rs != NULL)
                {
                  for(int j = 0; rv == TEST_OK && j < nvalues; j++)
                    {
                      int block[blocksize];

                      for(int k = 0; k < blocksize; k++)
                        block[k] = get_rand_state(1000, rs);

                      if(SHA1_Update(&context, block, sizeof block) == 0)
                        {
                          printf("Internal error: %s\n", "SHA1_Update");
                          rv = TEST_FAIL_HARD;
                        }
                    }
                  free(rs);
                }
              else
                {
                  printf("Internal error: %s\n", "seed_rand_state");
                  rv = TEST_FAIL_HARD;
                }
            }

          if(rv == TEST_OK)
            {
              if(SHA1_Final(hash_digest, &context) == 0)
                {
                  printf("Internal error: %s\n", "SHA1_Final");
                  rv = TEST_FAIL_HARD;
                }
            }
        }
      else
        {        
          printf("Internal error: %s\n", "SHA1_Init");
          rv = TEST_FAIL_HARD;
        }
#endif
    }
  else
    {
      printf("Internal error: %s\n", "malloc");
      rv = TEST_FAIL_HARD;
    }

  if(rv == TEST_OK)
    {
      char hash_string[hash_digest_size * 2 + 1];

      for(int i = 0; i < hash_digest_size; i++)
        sprintf(hash_string + i * 2, "%.2x", hash_digest[i]);
  
      if(strcmp(hash_string, correct_hash_string))
        {
          printf("Random number generator failed!\n");
          printf("Got: %s\n", hash_string);
          printf("Expected: %s\n", correct_hash_string);
          rv = TEST_FAIL;
        }
    }

  return rv;
}
