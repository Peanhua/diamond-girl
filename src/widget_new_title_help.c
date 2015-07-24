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
#include "font.h"
#include "widget.h"
#include "gfx.h"
#include "globals.h"
#include <assert.h>

static char *  language        = NULL;
static char ** t_welcome       = NULL;
static char ** t_title_help    = NULL;
static char ** t_classic_hints = NULL;
static char ** t_misc1         = NULL;
static char ** t_misc2         = NULL;
static char ** t_pp_help       = NULL;
static char ** t_pp_hints      = NULL;

static void load_texts(void);
static void draw(struct widget * this);

struct widget * widget_new_title_help(struct widget * parent, int x, int y, int width, int height)
{
  struct widget * obj;

  load_texts();
  
  obj = widget_new_child(parent, x, y, width, height);
  assert(obj != NULL);
  if(obj != NULL)
    {
      widget_set_on_draw(obj, draw);
      widget_set_ulong(obj, "type", WT_TITLE_HELP);
      widget_set_flags(obj, 0);
      widget_set_ulong(obj, "help_screen", 0);
    }

  return obj;
}


static void load_texts(void)
{
  if(language == NULL)
    {
      language = strdup(globals.language);
  
      assert(t_welcome == NULL);
      t_welcome = read_localized_text_file(get_data_filename("data/title-welcome"));
      assert(t_welcome != NULL);

      assert(t_title_help == NULL);
      t_title_help = read_localized_text_file(get_data_filename("data/title-help"));
      assert(t_title_help != NULL);

      assert(t_classic_hints == NULL);
      t_classic_hints = read_localized_text_file(get_data_filename("data/title-classic_hints"));
      assert(t_classic_hints != NULL);

      assert(t_misc1 == NULL);
      t_misc1 = read_localized_text_file(get_data_filename("data/title-misc1"));
      assert(t_misc1 != NULL);

      assert(t_misc2 == NULL);
      t_misc2 = read_localized_text_file(get_data_filename("data/title-misc2"));
      assert(t_misc2 != NULL);
      
      assert(t_pp_help == NULL);
      t_pp_help = read_localized_text_file(get_data_filename("data/title-pyjama_party_help"));
      assert(t_pp_help != NULL);

      assert(t_pp_hints == NULL);
      t_pp_hints = read_localized_text_file(get_data_filename("data/title-pyjama_party_hints"));
      assert(t_pp_hints != NULL);
    }
}  


static void draw(struct widget * this)
{
  /* First check if the language has changed, and if so, reload the texts. */
  if(language == NULL || strcmp(language, globals.language))
    {
      free(language);
      language = NULL;

      char ** texts[] =
        {
          t_welcome,
          t_title_help,
          t_classic_hints,
          t_misc1,
          t_misc2,
          t_pp_help,
          t_pp_hints,
          NULL
        };

      for(int i = 0; texts[i] != NULL; i++)
        {
          for(int j = 0; texts[i][j] != NULL; j++)
            free(texts[i][j]);
          free(texts[i]);
        }

      t_welcome         = 
        t_title_help    =
        t_classic_hints =
        t_misc1         =
        t_misc2         =
        t_pp_help       =
        t_pp_hints      =
        NULL;

      load_texts();
    }
         
  

  /* help/instructions */

  int x, y, width, height;
  int fontwidth;
  int help_screen;

  struct screendef
  {
    char ** text;
    char *  special;
  };

  struct screendef screens_classic[] =
    {
      { t_welcome,       NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_welcome,       NULL     },
      { t_title_help,    NULL     },
      { t_misc1,         NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_classic_hints, NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_misc2,         NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { NULL,            "end"    }
    };

  struct screendef screens_adventure[] =
    {
      { t_welcome,       NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_welcome,       NULL     },
      { t_title_help,    NULL     },
      { t_misc1,         NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_classic_hints, NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_misc2,         NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { NULL,            "end"    }
    };

  struct screendef screens_pyjama_party[] =
    {
      { t_welcome,       NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { t_welcome,       NULL     },
      { t_pp_help,       NULL     },
      { t_misc1,         NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_pp_hints,      NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { t_misc2,         NULL     },
      { NULL,            "legend" },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { NULL,            NULL     },
      { NULL,            "end"    }
    };


  x = widget_absolute_x(this);
  y = widget_absolute_y(this);
  width = widget_width(this);
  height = widget_height(this);

  int max_help_screen;
  struct screendef * current;

  current = NULL;
  switch(globals.title_game_mode)
    {
    case GAME_MODE_CLASSIC:      current = screens_classic;      break;
    case GAME_MODE_ADVENTURE:    current = screens_adventure;    break;
    case GAME_MODE_PYJAMA_PARTY: current = screens_pyjama_party; break;
    }
  assert(current != NULL);
  max_help_screen = 0;
  for(max_help_screen = 0; current[max_help_screen].special == NULL || strcmp(current[max_help_screen].special, "end"); max_help_screen++)
    ;
  
  help_screen = widget_get_ulong(this, "help_screen");
  if(help_screen >= max_help_screen)
    {
      help_screen = 0;
      widget_set_ulong(this, "help_screen", help_screen);
    }

  fontwidth = font_width("a");

  clear_area(x - fontwidth / 2, y, width + fontwidth / 2, height);

  if(current[help_screen].text != NULL)
    {
      int i;
      
      i = 0;
      while(current[help_screen].text[i] != NULL)
        {
          font_write(x, y, current[help_screen].text[i]);
          y += font_height();
          i++;
        }
    }
  else if(current[help_screen].special != NULL && !strcmp(current[help_screen].special, "legend"))
    { /* game graphics and explanations of them */
      draw_legend(x, y + 20, NULL, NULL);
    }
}

