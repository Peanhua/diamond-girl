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
#include "themes.h"
#include "gc.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

static struct theme * find_theme(const char * name);
static struct theme * special_day_theme(const char * name);
static void           read_config(struct theme * theme);

static struct theme ** themes;
static int             themes_count;
static struct theme *  current_themes[THEME_DEFAULT + 1];



void themes_initialize(void)
{
  themes       = NULL;
  themes_count = 0;

  DIR * d;

  d = opendir(get_data_filename("themes"));
  if(d != NULL)
    {
      struct dirent * e;

      do
        {
          e = readdir(d);
          if(e != NULL)
            {
              if(isalnum(e->d_name[0]) && strchr(e->d_name, '.') == NULL)
                {
                  struct theme ** tmp;
                  
                  tmp = realloc(themes, sizeof(struct theme *) * (themes_count + 1));
                  assert(tmp != NULL);
                  if(tmp != NULL)
                    {
                      struct theme * theme;

                      themes = tmp;

                      theme = malloc(sizeof(struct theme));
                      assert(theme != NULL);
                      if(theme != NULL)
                        {
                          theme->name      = strdup(e->d_name);
                          assert(theme->name != NULL);

                          theme->directory = strdup(e->d_name);
                          assert(theme->directory != NULL);

                          theme->enabled   = true;

                          theme->text      = NULL;

                          theme->icon.glyph     = MAP_SIZEOF_;
                          theme->icon.direction = MOVE_NONE;

                          read_config(theme);

                          gc_new(GCT_THEME, theme);
                          themes[themes_count] = theme;
                          themes_count++;
                        }
                    }
                }
            }
        } while(e != NULL);

      closedir(d);
    }
  else
    fprintf(stderr, "Failed to open directory '%s': %s\n", get_data_filename("themes"), strerror(errno));

  for(int i = 0; i <= THEME_DEFAULT; i++)
    theme_set(i, NULL);
}


void themes_cleanup(void)
{
  for(int i = 0; i < themes_count; i++)
    if(themes[i] != NULL)
      {
        gc_free(GCT_THEME, themes[i]);
        free(themes[i]->name);
        free(themes[i]->directory);
        free(themes[i]);
      }
  free(themes);

  themes       = NULL;
  themes_count = 0;
}


struct theme ** themes_get(void)
{
  return current_themes;
}

struct theme *  theme_get(enum THEME type)
{
  return current_themes[type];
}

struct theme * theme_set(enum THEME type, const char * name)
{
  struct theme * theme;

  theme = NULL;
  switch(type)
    {
    case THEME_SPECIAL_DAY:
      theme = special_day_theme(name);
      break;
    case THEME_TRAIT:
      theme = find_theme(name);
      break;
    case THEME_DEFAULT:
      theme = find_theme("default");
      break;
    }
  current_themes[type] = theme;

  return theme;
}


static struct theme * find_theme(const char * name)
{
  struct theme * theme;

  theme = NULL;

  if(name != NULL)
    for(int i = 0; theme == NULL && i < themes_count; i++)
      if(themes[i] != NULL)
        if(!strcmp(themes[i]->name, name))
          theme = themes[i];
  
  return theme;
}


static struct theme * special_day_theme(const char * name)
{
  struct
  {
    int day, month, year;
    char * name;
  } dates[] =
      {
        { 31, 12,    0, "new-year"  },
        {  1,  1,    0, "new-year"  },
        { 31, 10,    0, "halloween" },
        { 23, 12,    0, "x-mas"     },
        { 24, 12,    0, "x-mas"     },
        { 25, 12,    0, "x-mas"     },
        { 26, 12,    0, "x-mas"     },
        {  8,  4, 2012, "easter"    },
        { 31,  3, 2013, "easter"    },
        { 20,  4, 2014, "easter"    },
        {  5,  4, 2015, "easter"    },
        { 27,  3, 2016, "easter"    },
        { 16,  4, 2017, "easter"    },
        {  1,  4, 2018, "easter"    },
        { 21,  4, 2019, "easter"    },
        { 12,  4, 2020, "easter"    },
        {  4,  4, 2021, "easter"    },
        { 17,  4, 2022, "easter"    },
        {  0,  0,    0, NULL        }
      };

  if(name == NULL)
    {
      struct tm t;
      time_t tt;
      bool found;

      /* Choose the theme to be used */
      tt = time(NULL);

#if HAVE_LOCALTIME_R

      localtime_r(&tt, &t);

#else
      struct tm * tmpt;

      tmpt = localtime(&tt);
      t.tm_year = tmpt->tm_year;
      t.tm_mon  = tmpt->tm_mon;
      t.tm_mday = tmpt->tm_mday;
#endif

      found = false;
      for(int i = 0; found == false && dates[i].name != NULL; i++)
        if(dates[i].year == 0 || dates[i].year - 1900 == t.tm_year)
          if(dates[i].month == t.tm_mon + 1)
            if(dates[i].day == t.tm_mday)
              {
                found = true;
                name = dates[i].name;
              }
    }

  struct theme * rv;

  rv = find_theme(name);

  return rv;
}

static void read_config(struct theme * theme)
{
  char * buffer;
  int    buffer_len;
  char   fn[1024];

  snprintf(fn, sizeof fn, "themes/%s.config", theme->name);
  if(read_file(get_data_filename(fn), &buffer, &buffer_len) == true)
    {
      char * pos;

      pos = buffer;
      while(pos < buffer + buffer_len)
        {
          char * tmp;

          /* LF -> EOS */
          tmp = strchr(pos, '\n');
          if(tmp != NULL && *tmp == '\n')
            *tmp = '\0';

          tmp = strchr(pos, '=');
                                  
          if(tmp != NULL)
            { /* Process the line */
              char * varname, * varvalue;

              *tmp = '\0';
              varname = strdup(pos);
              pos += strlen(varname) + 1;

              assert(varname != NULL);
              varvalue = tmp + 1;
              pos += strlen(varvalue) + 1;

              if(!strcmp(varname, "ICON"))
                {
                  char * name;

                  name = varvalue;
                  tmp = strchr(varvalue, ':');
                  if(tmp != NULL && *tmp == ':')
                    {
                      *tmp = '\0';
                      tmp++;
                      if(!strcmp(tmp, "up"))
                        theme->icon.direction = MOVE_UP;
                      else if(!strcmp(tmp, "down"))
                        theme->icon.direction = MOVE_DOWN;
                      else if(!strcmp(tmp, "left"))
                        theme->icon.direction = MOVE_LEFT;
                      else if(!strcmp(tmp, "right"))
                        theme->icon.direction = MOVE_RIGHT;
                      else if(!strcmp(tmp, "none"))
                        theme->icon.direction = MOVE_NONE;
                      else
                        {
                          fprintf(stderr, "%s: Failed to parse %s: unknown direction '%s' for the icon.\n", __FUNCTION__, get_data_filename(fn), tmp);
                          pos = buffer + buffer_len; /* exit */
                        }
                    }
                  else
                    theme->icon.direction = MOVE_UP;

                  struct
                  {
                    char *         name;
                    enum MAP_GLYPH glyph;
                  } glyphs[] =
                      {
                        { "empty",             MAP_EMPTY             },
                        { "border",            MAP_BORDER            },
                        { "illegal",           MAP_ILLEGAL           },
                        { "sand",              MAP_SAND              },
                        { "player",            MAP_PLAYER            },
                        { "player_armour0",    MAP_PLAYER_ARMOUR0    },
                        { "player_armour1",    MAP_PLAYER_ARMOUR1    },
                        { "player_armour2",    MAP_PLAYER_ARMOUR2    },
                        { "boulder",           MAP_BOULDER           },
                        { "diamond",           MAP_DIAMOND           },
                        { "enemy1",            MAP_ENEMY1            },
                        { "enemy2",            MAP_ENEMY2            },
                        { "smoke",             MAP_SMOKE             },
                        { "smoke1",            MAP_SMOKE1            },
                        { "smoke2",            MAP_SMOKE2            },
                        { "exit_level",        MAP_EXIT_LEVEL        },
                        { "extra_life_anim",   MAP_EXTRA_LIFE_ANIM   },
                        { "brick",             MAP_BRICK             },
                        { "brick_expanding",   MAP_BRICK_EXPANDING   },
                        { "brick_unbreakable", MAP_BRICK_UNBREAKABLE },
                        { "brick_morpher",     MAP_BRICK_MORPHER     },
                        { "ameba",             MAP_AMEBA             },
                        { "slime",             MAP_SLIME             },
                        { NULL,                MAP_SIZEOF_           }
                      };
                  
                  for(int i = 0; glyphs[i].name != NULL && name != NULL; i++)
                    if(!strcmp(glyphs[i].name, name))
                      {
                        name = NULL;
                        theme->icon.glyph = glyphs[i].glyph;
                      }
                  if(name != NULL)
                    {
                      fprintf(stderr, "%s: Failed to parse %s: unknown icon name '%s'.\n", __FUNCTION__, get_data_filename(fn), name);
                      pos = buffer + buffer_len; /* exit */
                    }
                }
              else if(!strcmp(varname, "TEXT"))
                {
                  theme->text = strdup(varvalue);
                }
              else
                {
                  fprintf(stderr, "%s: Failed to parse %s: unknown variable '%s'.\n", __FUNCTION__, get_data_filename(fn), varname);
                  pos = buffer + buffer_len; /* exit */
                }

              free(varname);
            }
          else
            {
              fprintf(stderr, "%s: Failed to parse %s: unknown syntax '%s'.\n", __FUNCTION__, get_data_filename(fn), pos);
              pos = buffer + buffer_len; /* exit */
            }
        }
      free(buffer);
    }
}
