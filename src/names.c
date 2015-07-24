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
#include "names.h"
#include "random.h"
#include <assert.h>
#include <ctype.h>
#include <libintl.h>
#include <stdbool.h>
#include <string.h>


struct link
{
  unsigned long next[0xff];
  unsigned long total;
};


static void          initialize_chain(struct link * chain, unsigned char const * seed);
static char *        names_get_neuter(void);
static char *        names_get_markov(struct link * chain);

static bool          isconsonant(char c);
static int           getconsonantchance(int total_vowels, int current_consonant_count);


static struct link femalechain[0xff];
static struct link malechain[0xff];
static struct link surnamechain[0xff];


void names_initialize(void)
{
  initialize_chain(femalechain,
                   (unsigned char *) gettext("lucy mary patricia linda barbara elizabeth jennifer maria susan margaret dorothy lisa nancy karen betty helen sandra donna carol ruth sharon michelle laura sarah kimberly deborah jessica shirley cynthia angela melissa brenda amy anna rebecca virginia kathleen pamela martha debra amanda stephanie carolyn christine marie janet catherine frances ann joyce diane"));

  initialize_chain(malechain,
                   (unsigned char *) gettext("steve james john robert michael william david richard charles joseph thomas christopher daniel paul mark donald george kenneth steven edward brian ronald anthony kevin jason matthew gary timothy jose larry jeffrey frank scott eric stephen andrew raymond gregory joshua jerry dennis walter patrick peter harold douglas henry carl arthur ryan roger"));

  initialize_chain(surnamechain,
                   (unsigned char *) gettext("smith johnson williams brown jones miller davis rodriguez wilson martinez anderson taylor thomas hernandez moore martin jackson thompson white lopez lee gonzalez harris clark lewis robinson walker perez hall young allen sanchez wright king scott green baker adams nelson hill ramirez campbell mitchell roberts carter phillips evans turner torres"));
}


void names_cleanup(void)
{
}


char const * names_get(enum GENDER gender, unsigned int minlength, unsigned int maxlength)
{
  char * rv;
  bool done;
  
  done = false;
  while(done == false)
    {
      switch(gender)
        {
        case GENDER_NEUTER: rv = names_get_neuter();             break;
        case GENDER_FEMALE: rv = names_get_markov(femalechain);  break;
        case GENDER_MALE:   rv = names_get_markov(malechain);    break;
        case GENDER_FAMILY: rv = names_get_markov(surnamechain); break;
        default:            rv = NULL; assert(0);                break;
        }

      if(rv != NULL)
        if(strlen(rv) >= minlength && strlen(rv) <= maxlength)
          done = true;
    }

  if(rv != NULL)
    *rv = toupper(*rv);
  
  return rv;
}


static void initialize_chain(struct link * chain, unsigned char const * seed)
{ /* Initialize markov chain of characters, word separator is linefeed '\n'. */
  for(unsigned int i = 0; i < 0xff; i++)
    {
      for(unsigned int j = 0; j < 0xff; j++)
        chain[i].next[j] = 0;

      chain[i].total = 0;
    }

  unsigned int seedlen;

  seedlen = strlen((char *) seed);

  for(unsigned int i = 0; i < seedlen; i++)
    {
      int nextchar;

      nextchar = tolower(seed[i + 1]);
      if(nextchar < 'a' || nextchar > 'z')
        nextchar = '\n';
      chain[seed[i]].next[nextchar] += 1;
      chain[seed[i]].total += 1;
    }
}


static char * names_get_neuter(void)
{
  static char buf[32];
  int len;
  char vow[] = "aeiouy";
  char con[] = "bcdfghjklmnpqrstvwxz";
  bool vowcon;

  len = 4 + get_rand(8);
  vowcon = get_rand(2);
  for(int i = 0; i < len; i++)
    {
      if(vowcon == true)
        { /* vowel */
          buf[i] = vow[get_rand(strlen(vow))];
          if(get_rand(100) < 35)
            vowcon = false;
        }
      else
        { /* consonant */
          buf[i] = con[get_rand(strlen(con))];
          if(get_rand(100) < 66)
            vowcon = true;
        }
    }
  buf[len] = '\0';

  return buf;
}




static char * names_get_markov(struct link * chain)
{
  static char buf[1024];
  bool done;
  int consonant_count;
  int vowel_count;

  consonant_count = 0;
  vowel_count = 0;

  buf[0] = '\0';
  while(chain[(unsigned char) buf[0]].total == 0)  
    buf[0] = 'a' + get_rand('z' - 'a');
  
  if(isconsonant(buf[0]) == true)
    consonant_count++;
  else
    vowel_count++;
  
  done = false;
  for(unsigned int i = 1; done == false && i < sizeof buf; i++)
    {
      bool nextdone;
      int ctry;
      
      nextdone = false;
      ctry = 0;
      while(done == false && nextdone == false)
        {
          long n = get_rand(chain[(unsigned char) buf[i - 1]].total);
          unsigned int nchar = 0;
          for(nchar = 0; n >= (int) chain[(unsigned char) buf[i - 1]].next[nchar] && nchar < 0xff; nchar++)
            n -= chain[(unsigned char) buf[i - 1]].next[nchar];

          if(nchar == '\n')
            {
              buf[i] = '\0';
              done = true;
            }
          else
            {
              if(isconsonant(nchar) == false)
                {
                  buf[i] = nchar;
                  nextdone = true;
                  consonant_count = 0;
                  vowel_count++;
                }
              else
                {
                  if((int) get_rand(100) < getconsonantchance(vowel_count, consonant_count))
                    {
                      buf[i] = nchar;
                      nextdone = true;
                      consonant_count++;
                    }
                  ctry++;
                  if(ctry > 30)
                    {
                      buf[i] = '\0';
                      done = true;
                    }
                }
            }
        }
    }
  
  return buf;
}

static bool isconsonant(char c)
{
  char * vowels = "aeiouy";

  return strchr(vowels, c) == NULL ? true : false;
}

static int getconsonantchance(int total_vowels, int current_consonant_count)
{ /* Return chance in 0..1 for the next character after current to be a consonant (1).
  ** Takes current_count as the number of consonants done after last vowel. */
  if(total_vowels == 0 && current_consonant_count > 1) return   0;
  else if(current_consonant_count < 1)                 return 100;
  else if(current_consonant_count < 2)                 return  50;
  else if(current_consonant_count < 3)                 return  10;
  else                                       return   0;
}
