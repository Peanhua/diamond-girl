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

#include "ai.h"
#include "random.h"

#include <assert.h>
#include <inttypes.h>

static struct ai_knowledge ** brains      = NULL;
static uint16_t               brains_size = 0;

float ai_brains_add(const char * address, float strength)
{
  struct ai_knowledge * knowledge;
  int empty_spot;

  knowledge = NULL;
  empty_spot = -1;
  for(uint16_t i = 0; knowledge == NULL && i < brains_size; i++)
    if(brains[i] != NULL)
      {
        if(!strcmp(brains[i]->address, address))
          knowledge = brains[i];
      }
    else
      {
        if(empty_spot == -1)
          empty_spot = i;
      }

  if(knowledge == NULL)
    { /* New knowledge. */
      if(empty_spot == -1)
        {
          struct ai_knowledge ** tmp;

          tmp = realloc(brains, sizeof(struct ai_knowledge *) * (brains_size + 1));
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              brains = tmp;
              empty_spot = brains_size;
              brains[empty_spot] = NULL;
              brains_size++;
            }
        }
      if(empty_spot > -1)
        {
          brains[empty_spot] = malloc(sizeof(struct ai_knowledge));
          assert(brains[empty_spot] != NULL);
          if(brains[empty_spot] != NULL)
            {
              knowledge = brains[empty_spot];

              knowledge->address = strdup(address);
              assert(knowledge->address != NULL);
              knowledge->value = 0.0f;
              knowledge->experience = 3.0f;
            }
        }
    }

  float rv;

  if(knowledge != NULL)
    { /* Alter old knowledge. */
      float exp;

      exp = knowledge->experience;
      if(exp > 10.0f)
        exp = 10.0f + (exp - 10.0f) * 0.1f;
      knowledge->value = (exp * knowledge->value + strength) / (exp + 1.0f);
      knowledge->experience += 1.0f;

      /* Clamp the value between -1 .. +1 */
      if(knowledge->value < -1.0f)
        knowledge->value = -1.0f;
      else if(knowledge->value > 1.0f)
        knowledge->value = 1.0f;

      rv = knowledge->value;
    }
  else
    rv = 0.0f;

  return rv;
}


float ai_brains_get(const char * address)
{
  struct ai_knowledge * knowledge;
  float rv;

  rv = 0.0f; /* Well.. yea.. maybe. -1 would be No, and +1 Yes. */

  knowledge = NULL;
  for(uint16_t i = 0; knowledge == NULL && i < brains_size; i++)
    if(brains[i] != NULL)
      if(!strcmp(brains[i]->address, address))
        knowledge = brains[i];

  if(knowledge != NULL)
      rv = knowledge->value;

  return rv;
}


void ai_brains_dump(void)
{
#ifndef NDEBUG
  double total_exp, total_exp2;

  printf("Brain dump:\n");
  printf(" size: %ld cells\n", (long int) brains_size);
  printf(" value exp     address\n");
  total_exp = 0.0;
  total_exp2 = 0.0;
  for(uint16_t i = 0; i < brains_size; i++)
    if(brains[i] != NULL)
      {
        total_exp  += brains[i]->experience;
        total_exp2 += brains[i]->experience - 3.0f;
        
        printf(" %5.2f %.0f(%.0f)  %s\n",
               brains[i]->value,
               brains[i]->experience - 3.0f,
               brains[i]->experience,
               brains[i]->address);
      }
  printf(" total memories: about %.0f(%.0f)\n", total_exp2, total_exp);
#endif
}


void ai_brains_fart(void)
{
  if(brains_size > 0)
    {
      uint16_t n;

      n = get_rand(brains_size);
      if(brains[n] != NULL)
        {
          float mod;

          mod = ((float) get_rand(100) - 50.0f) / 100.0f;
          brains[n]->value += mod;
          if(brains[n]->value < -1.0f)
            brains[n]->value = -1.0f;
          else if(brains[n]->value > 1.0f)
            brains[n]->value = 1.0f;
        }
    }
}


void ai_brains_load(const char * filename)
{
  FILE * fp;

  ai_brains_cleanup();

  fp = fopen(filename, "rb");
  if(fp != NULL)
    {
      enum
      {
        MODE_HEADER,
        MODE_DATA
      } mode;
      bool done;
      int version;

      version = 0;
      mode = MODE_HEADER;
      done = false;
      while(done == false)
        {
          char linebuf[1024 * 4];

          if( fgets(linebuf, (sizeof linebuf) - 1, fp) != NULL )
            {
              {
                char * p;
                
                p = strchr(linebuf, '\n');
                if(p != NULL)
                  *p = '\0';
              }

              switch(mode)
                {
                case MODE_HEADER:
                  if(!strcmp(linebuf, "DIAMOND GIRL BRAIN 1.0"))
                    {
                      mode = MODE_DATA;
                      version = 10;
                    }
                  else if(!strcmp(linebuf, "DIAMOND GIRL BRAIN 1.1"))
                    {
                      mode = MODE_DATA;
                      version = 11;
                    }
                  else
                    {
                      fprintf(stderr, "%s('%s'): Illegal header: %s\n", __FUNCTION__, filename, linebuf);
                      done = true;
                    }
                  break;
                case MODE_DATA:
                  {
                    char * pos;
                    
                    pos = strchr(linebuf, ' ');
                    if(pos != NULL)
                      {
                        char * address;

                        address = linebuf;
                        *pos = '\0';
                        pos++;
                        
                        float value, experience;
                        char * tmp;

                        value = strtof(pos, &tmp);
                        experience = strtof(tmp, NULL);
                        
                        struct ai_knowledge ** tmp_brains;
                        
                        tmp_brains = realloc(brains, sizeof(struct ai_knowledge *) * (brains_size + 1));
                        assert(tmp_brains != NULL);
                        if(tmp_brains != NULL)
                          {
                            brains = tmp_brains;
                            brains[brains_size] = malloc(sizeof(struct ai_knowledge));
                            assert(brains[brains_size] != NULL);
                            if(brains[brains_size] != NULL)
                              {
                                if(version == 10)
                                  { /* map_set -> cave */
                                    char * adr;
                                    char * o = ",map_set="; /* replace this */
                                    char * n = ",cave=";    /* with this */

                                    adr = strstr(address, o);
                                    if(adr != NULL)
                                      {
                                        int diff;

                                        diff = strlen(o) - strlen(n);
                                        memcpy(adr, n, strlen(n));
                                        adr += strlen(n) + diff;
                                        while(*adr != '\0')
                                          {
                                            *(adr - diff) = *adr;
                                            adr++;
                                          }
                                        *adr = '\0';
                                      }
                                  }
                                brains[brains_size]->address = strdup(address);
                                brains[brains_size]->value = value;
                                brains[brains_size]->experience = experience;
                                brains_size++;
                              }
                          }
                      }
                  }
                  break;
                }
            }
          else
            done = true;
        }
      fclose(fp);
    }
}


void ai_brains_save(const char * filename)
{
  FILE * fp;

  fp = fopen(filename, "wb");
  if(fp != NULL)
    {
      fprintf(fp, "DIAMOND GIRL BRAIN 1.1\n");

      for(uint16_t i = 0; i < brains_size; i++)
        if(brains[i] != NULL)
          fprintf(fp,
                  "%s %f %f\n",
                  brains[i]->address,
                  brains[i]->value,
                  brains[i]->experience
                  );

      fclose(fp);
    }
}

void ai_brains_cleanup(void)
{
  if(brains != NULL)
    {
      for(uint16_t i = 0; i < brains_size; i++)
        if(brains[i] != NULL)
          {
            if(brains[i]->address != NULL)
              free(brains[i]->address);
            
            free(brains[i]);
            brains[i] = NULL;
          }

      free(brains);
      brains = NULL;
    }
  brains_size = 0;
}

