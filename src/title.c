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
#include "title.h"
#include "ui.h"
#include "widget.h"
#include "widget_factory.h"
#include "image.h"
#include "texture.h"
#include "td_object.h"
#include "sfx.h"
#include "gfx.h"
#include "gfx_glyph.h"
#include "highscore.h"
#include "globals.h"
#include "playback.h"
#include "game.h"
#include "ai.h"
#include "random.h"
#include "twinkle.h"
#include "map.h"
#include "traits.h"
#include "gfx_image.h"
#include "girl.h"
#include "themes.h"
#include "cave.h"
#include "event.h"

#include <assert.h>
#include <SDL/SDL_framerate.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <ctype.h>
#include <libintl.h>
#ifdef WITH_OPENGL
#include <GL/glew.h>
#include <GL/glu.h>
#endif


static void on_ui_draw(bool pressed, SDL_Event * event);
static void load_cave_data(void);
static void on_settings_changed(bool gfx_restart, bool sfx_restart, bool new_opengl);
static void reset_ui(bool reset_gfx, bool new_opengl, trait_t traits_to_highlight);
static void player_death(struct gamedata * gamedata, bool sounds);
static void on_title_exit(bool pressed, SDL_Event * event);
static void on_trait_changed(int64_t changes);

static bool quit;

static trait_t next_trait_from_this_cave;
static int     next_trait_from_this_cave_level;
static int help_screen_timer;
#define help_screen_seconds 12
static int anim_timer;

static struct gamedata * previous_game;

static char ** cave_names;
static int level_selection_change;
static struct cave * current_cave;

static struct widget * uiobj_game_mode;
static struct widget * uiobj_partystatus;
static struct widget * uiobj_help;
static struct widget * uiobj_cave;
static struct widget * uiobj_cave_left;
static struct widget * uiobj_cave_right;
static struct widget * uiobj_level;
static struct widget * uiobj_level_thumbnail;
static struct widget * uiobj_level_left;
static struct widget * uiobj_level_right;
static struct widget * uiobj_next_trait;
static struct widget * uiobj_key;
static struct widget * uiobj_config;
static struct widget * uiobj_editor;
static struct widget * uiobj_quit;
static struct widget * uiobj_trait_buttons[TRAIT_SIZEOF_];

static struct widget * uiobj_newgame_window;

static struct map * mainmenu_map;



static void setup_ui(trait_t traits_to_highlight);
static void unload_ui(void);
static void play_game(void);
static void button_newgame(struct widget * this, enum WIDGET_BUTTON button);
static void button_newgame_resume(struct widget * this, enum WIDGET_BUTTON button);
static void button_newgame_new(struct widget * this, enum WIDGET_BUTTON button);
static void button_newgame_cancel(struct widget * this, enum WIDGET_BUTTON button);
static void button_newgame_cancel2(bool pressed, SDL_Event * event);
static void button_cave_left(struct widget * this, enum WIDGET_BUTTON button);
static void button_cave_right(struct widget * this, enum WIDGET_BUTTON button);
static void button_level_left(struct widget * this, enum WIDGET_BUTTON button);
static void button_level_right(struct widget * this, enum WIDGET_BUTTON button);
static void button_config(struct widget * this, enum WIDGET_BUTTON button);
static void button_editor(struct widget * this, enum WIDGET_BUTTON button);
static void button_quit(struct widget * this, enum WIDGET_BUTTON button);
static void button_game_mode(struct widget * this, enum WIDGET_BUTTON button);
static void on_change_game_mode(struct widget * this, enum WIDGET_BUTTON button);
static void on_highscore_activated(struct widget * this, enum WIDGET_BUTTON button);
static void on_cave_changed(void);
static void on_level_changed(void);
static int  get_cave_number(const char * cave);
static void set_next_trait_from_this_cave(void);
static void new_highscore(struct gamedata * gamedata);


void title(void)
{
  FPSmanager framerate_manager;
  int level_selection_change_timer;
  int last_level, last_max_starting_level;

  last_level = last_max_starting_level = -1;

  SDL_initFramerate(&framerate_manager);
  SDL_setFramerate(&framerate_manager, 60);

  quit = false;

  help_screen_timer = 0;
  anim_timer = 0;
  ui_set_last_user_action(time(NULL));

  if(globals.title_game_mode == GAME_MODE_PYJAMA_PARTY)
    previous_game = NULL;
  else
    previous_game = title_load_playback();

  level_selection_change = 0;

  cave_names = get_cave_names(true);
  assert(cave_names != NULL);
  assert(cave_names[0] != NULL);
  current_cave = NULL;

  { /* Make sure the cave selection is valid. */
    bool found;

    found = false;
    for(int i = 0; found == false && cave_names[i] != NULL; i++)
      if(!strcmp(cave_names[i], globals.cave_selection))
        found = true;

    if(found == false)
      {
        set_cave_selection("a");
      }
  }

  int tmp;

  tmp = globals.level_selection;
  load_cave_data();

  /* Make sure the level selection is in valid range. */
  if(tmp <= current_cave->max_starting_level)
    globals.level_selection = tmp;
  else
    globals.level_selection = current_cave->max_starting_level;


  setup_ui(0);

  sfx_music(MUSIC_TITLE, 1);

  level_selection_change_timer = 0;

  gfx_frame0();

  if(strlen(globals.playback_name) > 0)
    {
      struct playback * playback;
      char fn[256];

      if(current_cave->game_mode == GAME_MODE_PYJAMA_PARTY)
        {
          if(!strncmp(globals.playback_name, "random-", strlen("random-")))
            snprintf(fn, sizeof fn, "party/highscores-%s.dgp", globals.playback_name + strlen("random-"));
          else
            snprintf(fn, sizeof fn, "party/highscores-%s.dgp", globals.playback_name);
        }
      else
        {
          if(!strncmp(globals.playback_name, "random-", strlen("random-")))
            snprintf(fn, sizeof fn, "highscores-%s.dgp", globals.playback_name + strlen("random-"));
          else
            snprintf(fn, sizeof fn, "highscores-%s.dgp", globals.playback_name);
        }

      playback = playback_load(get_save_filename(fn));
      if(playback != NULL)
        {
          struct cave * cave;

          cave = cave_get(playback->game_mode, playback->cave);
          title_game(cave, playback, globals.playback_name);
          playback = playback_free(playback);
        }
      else
        fprintf(stderr, "Playback '%s' not found error. Complete filename is '%s'.\n", globals.playback_name, get_save_filename(fn));
          
      quit = true;
    }

  while(quit == false)
    {
      ui_process_events(false);
      
      level_selection_change_timer++;
      if(level_selection_change_timer >= 5)
        {
          level_selection_change_timer = 0;
          if(level_selection_change)
            if(globals.level_selection + level_selection_change >= 1 && globals.level_selection + level_selection_change <= current_cave->max_starting_level)
              {
                globals.level_selection += level_selection_change;
                on_level_changed();
              }
        }

      if(quit == false)
	{
          time_t timediff, now, last_user_action;
          int timeout;
          int lucy_anim_length;

          timeout = 20 * help_screen_seconds - 6;
          lucy_anim_length = 12;
          now = time(NULL);
          last_user_action = ui_get_last_user_action();
          assert(now >= last_user_action);
          timediff = now - last_user_action;
          if(timediff >= timeout - lucy_anim_length)
            {
              static int change_timer = 0;
              enum UI_COMMAND cmds[4] = { UIC_UP, UIC_DOWN, UIC_LEFT, UIC_RIGHT };

              if(change_timer == 0)
                {
                  ui_call_handler(cmds[get_rand(4)], true, NULL);
                  change_timer = (lucy_anim_length - (timediff - (timeout - lucy_anim_length))) * 4;
                }
              else
                change_timer--;
            }
          if(timediff >= timeout)
            {
              struct playback * pb;
              char * pb_title;
              char * pb_t = "previous game";

              if(previous_game != NULL)
                {
                  pb = previous_game->playback;
                  pb_title = pb_t;
                }
              else
                {
                  pb = NULL;
                  pb_title = NULL;
                }

              title_game(current_cave, pb, pb_title);
              sfx_music(MUSIC_TITLE, 1);
              gfx_frame0();

              ui_set_last_user_action(time(NULL) - timeout / 2);

              if(pb != NULL)
                { /* Playback only once, discard the previous game after playbacking it. */
                  assert(previous_game != NULL);
                  previous_game = gamedata_free(previous_game);
                }
            }

          title_map_tick(mainmenu_map);
          title_help_tick(uiobj_help);

          if(last_level != globals.level_selection || last_max_starting_level != current_cave->max_starting_level)
            {
              last_level = globals.level_selection;
              last_max_starting_level = current_cave->max_starting_level;
              on_level_changed();
            }

          ui_draw(true);

	  SDL_framerateDelay(&framerate_manager);


#ifdef PROFILING
          globals.level_selection = 9;
          play_game();
          quit = true;
#endif
	}
    }

  unload_ui();

  if(previous_game != NULL)
    previous_game = gamedata_free(previous_game);

  cave_names = free_cave_names(cave_names);
}


static void player_death(struct gamedata * gamedata DG_UNUSED, bool sounds)
{
  mainmenu_map->girl->mob->alive = false;
  sfx_pitch(1.0f);
  if(sounds)
    sfx_emit(SFX_EXPLOSION, 0, 0);

  for(int y = -1; y <= 1; y++)
    for(int x = -1; x <= 1; x++)
      if(x + mainmenu_map->girl->mob->x >= 0 && x + mainmenu_map->girl->mob->x < mainmenu_map->width &&
         y + mainmenu_map->girl->mob->y >= 0 && y + mainmenu_map->girl->mob->y < mainmenu_map->height)
        mainmenu_map->data[x + mainmenu_map->girl->mob->x + (y + mainmenu_map->girl->mob->y) * mainmenu_map->width] = MAP_SAND;
}


static void on_title_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    quit = true;
}




static void on_toggle_fullscreen(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      globals.fullscreen = globals.fullscreen == 0 ? 1 : 0;
      reset_ui(true, globals.opengl, 0);
    }
}

static void on_scroll_highscores_up(bool pressed, SDL_Event * event DG_UNUSED)
{
  struct widget * uiobj_highscores;

  uiobj_highscores = widget_find(NULL, "highscores");
  if(uiobj_highscores != NULL)
    if(pressed == true)
      {
        int highscores_first;
      
        highscores_first = widget_get_ulong(uiobj_highscores, "first_highscore");
        if(highscores_first > 0)
          highscores_first -= 20;
        if(highscores_first < 0)
          highscores_first = 0;
        widget_set_ulong(uiobj_highscores, "first_highscore", highscores_first);
      }
}

static void on_scroll_highscores_down(bool pressed, SDL_Event * event DG_UNUSED)
{
  struct widget * uiobj_highscores;

  uiobj_highscores = widget_find(NULL, "highscores");
  if(uiobj_highscores != NULL)
    if(pressed == true)
      {
        size_t size;
        int highscores_first;
  
        highscores_first = widget_get_ulong(uiobj_highscores, "first_highscore");
        highscores_get(current_cave->highscores, &size);
        if(highscores_first + 20 < (int) size)
          widget_set_ulong(uiobj_highscores, "first_highscore", highscores_first + 20);
      }
}

static void on_pause(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      ai_brains_dump();
      ai_brains_save(get_save_filename("lucy.brains"));
      if(get_rand(100) < 5)
        {
          printf(gettext("Brain malfunction!\n"));
          ai_brains_fart();
          ai_brains_dump();
        }
      ui_set_last_user_action(0);
    }
}


static void on_ui_draw(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      map_animations_tick(mainmenu_map);

      {
        static float colors[3] = { 0.0f, /* Start from different positions in the sine curve. */
                                   M_PI * 2.0f / 3.0f,
                                   M_PI * 2.0f / 3.0f * 2.0f };
        
        for(int i = 0; i < 3; i++)
          {
            mainmenu_map->display_colour[i] = (0.1f + sinf(colors[i]) * 0.1f) * 255.0f;
            colors[i] += ((float) i + 1.0f) * 0.0001f;
            if(colors[i] > M_PI * 2.0f)
              colors[i] -= M_PI * 2.0f;
          }
        
        mainmenu_map->display_colour[3] = 0xff;
      }

      gfx_3d(true);
      draw_title_starfield();
      gfx_2d();
    }
  else
    {
      gfx_3d(false);
      draw_title_logo();
      gfx_2d();

      twinkle_fullscreen();
      twinkle_draw();

      help_screen_timer++;
      if(help_screen_timer >= mainmenu_map->frames_per_second * help_screen_seconds)
        {
          int help_screen;
          
          help_screen = widget_get_ulong(uiobj_help, "help_screen");
          help_screen++;
          widget_set_ulong(uiobj_help, "help_screen", help_screen);
          help_screen_timer = 0;
        }
    }
}


static void load_cave_data(void)
{
  current_cave = cave_get(globals.title_game_mode, globals.cave_selection);
  set_max_level_selection(current_cave->max_starting_level);

  set_next_trait_from_this_cave();
  set_max_level_selection(current_cave->max_starting_level); /* Process the checks in the function. */
  globals.level_selection = 1;

  on_cave_changed();
}



static void setup_ui(trait_t traits_to_highlight)
{
  int x, y, w;

  for(int i = 0; i < TRAIT_SIZEOF_; i++)
    uiobj_trait_buttons[i] = NULL;

  sfx_volume(0.05f);
  sfx_music_volume(1.0f);

  mainmenu_map = map_new(SCREEN_WIDTH / 24, SCREEN_HEIGHT / 24);
  assert(mainmenu_map != NULL);

  mainmenu_map->background_type = 2;
  mainmenu_map->frames_per_second = 60;

  mainmenu_map->player_movement = title_map_girl_movement;
  mainmenu_map->player_death    = player_death;

  for(int my = 0; my < mainmenu_map->height; my++)
    for(int mx = 0; mx < mainmenu_map->width; mx++)
      mainmenu_map->data[mx + my * mainmenu_map->width] = MAP_EMPTY;

  sfx_set_listener_position(mainmenu_map->width / 2, mainmenu_map->height / 2);
  
  gfx_frame0();

  draw_title_logo_initialize(mainmenu_map);
  draw_title_starfield_initialize();

  ui_initialize();

  ui_clear_handlers();
  ui_set_handler(UIC_DRAW,              on_ui_draw);
  ui_set_common_handlers();
  ui_set_navigation_handlers();
  ui_set_handler(UIC_EXIT,              on_title_exit);
  ui_set_handler(UIC_CANCEL,            on_title_exit);
  ui_set_handler(UIC_TOGGLE_FULLSCREEN, on_toggle_fullscreen);
  ui_set_handler(UIC_SCROLL_UP,         on_scroll_highscores_up);
  ui_set_handler(UIC_SCROLL_DOWN,       on_scroll_highscores_down);
  ui_set_handler(UIC_PAUSE,             on_pause);

  struct widget * root;
  struct widget * uiobj_highscores;
  struct widget_factory_data uiconfig[] =
    {
      { WFDT_CAVE,        "cave",                   current_cave           },
      { WFDT_MAP,         "map",                    mainmenu_map           },
      { WFDT_ON_ACTIVATE, "on_highscore_activated", on_highscore_activated },
      { WFDT_ON_RELEASE,  "button_newgame",         button_newgame         },
      { WFDT_ON_RELEASE,  "button_cave_left",       button_cave_left       },
      { WFDT_ON_RELEASE,  "button_cave_right",      button_cave_right      },
      { WFDT_ON_RELEASE,  "button_editor",          button_editor          },
      { WFDT_ON_RELEASE,  "button_level_left",      button_level_left      },
      { WFDT_ON_RELEASE,  "button_level_right",     button_level_right     },
      { WFDT_ON_RELEASE,  "button_config",          button_config          },
      { WFDT_ON_RELEASE,  "button_quit",            button_quit            },
      { WFDT_SIZEOF_,     NULL,                     NULL                   }
    };

  assert(current_cave != NULL);

  root = widget_factory(uiconfig, NULL, "title_common");
  assert(root != NULL);
  if(!globals.opengl)
    widget_set_enabled(root, false);

  if(current_cave->game_mode == GAME_MODE_CLASSIC || current_cave->game_mode == GAME_MODE_ADVENTURE)
    widget_factory(uiconfig, root, "title_classic");
  else if(current_cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    widget_factory(uiconfig, root, "title_pyjama_party");

  uiobj_game_mode   = NULL;
  uiobj_highscores  = widget_find(root, "highscores");
  uiobj_partystatus = widget_find(root, "partystatus");
  uiobj_help        = widget_find(root, "help");

  
  y = SCREEN_HEIGHT - (font_height() + 5) - (font_height() + 5) * 4;
  w = 120;
  x = SCREEN_WIDTH / 2 - w - 10 / 2;


  struct widget * last_trait;

  {
    int tx, ty;
    int spacing;
    struct widget * prev;
    struct
    {
      trait_t trait;
      bool classic_mode;
      bool adventure_mode;
      bool pyjamaparty_mode;
    } traits[] =
        {
          { TRAIT_ADVENTURE_MODE, true,  true,  true  },
          { TRAIT_PYJAMA_PARTY,   true,  true,  true  },
          { TRAIT_QUESTS,         false, true,  false },
          { TRAIT_IRON_GIRL,      false, true,  false },
          { TRAIT_KEY,            true,  true,  true  },
          { TRAIT_RIBBON,         false, true,  false },
          { TRAIT_GREEDY,         false, true,  true  },
          { TRAIT_TIME_CONTROL,   false, true,  true  },
          { TRAIT_POWER_PUSH,     false, true,  true  },
          { TRAIT_DIAMOND_PUSH,   false, true,  true  },
          { TRAIT_RECYCLER,       true,  true,  true  },
          { TRAIT_STARS1,         true,  true,  true  },
          { TRAIT_STARS2,         true,  true,  true  },
          { TRAIT_STARS3,         true,  true,  true  },
          { TRAIT_CHAOS,          false, true,  true  },
          { TRAIT_DYNAMITE,       false, true,  true  },
          
          { TRAIT_ALL,            false, false, false }
        };
    int traitcount;

    traitcount = 0;
    for(int i = 0; traits[i].trait != TRAIT_ALL; i++)
      if(traits_get_available() & traits[i].trait)
        traitcount++;

    tx = 10;
    ty = y + 3;
    if(traitcount <= 10)
      {
        spacing = 50;
        if(traitcount <= 5)
          ty += spacing / 2; /* We have only 5 traits, they fit to one row, so we center the row vertically. */
      }
    else if(traitcount <= 12)
      spacing = 44;
    else if(traitcount <= 18)
      spacing = 40;
    else
      spacing = 10;
    assert(traitcount <= 18);

    prev = NULL;
    for(int i = 0; traits[i].trait != TRAIT_ALL; i++)
      if((current_cave->game_mode == GAME_MODE_CLASSIC      && traits[i].classic_mode     == true) ||
         (current_cave->game_mode == GAME_MODE_ADVENTURE    && traits[i].adventure_mode   == true) ||
         (current_cave->game_mode == GAME_MODE_PYJAMA_PARTY && traits[i].pyjamaparty_mode == true)    )
        if(traits_get_available() & traits[i].trait)
          {
            struct widget * b;
          
            b = widget_new_trait_button(root, tx, ty, spacing - 10 + 2, spacing - 10 + 2, traits[i].trait, false, true);
            uiobj_trait_buttons[i] = b;

            if(b != NULL)
              {
                if(traits_to_highlight & traits[i].trait)
                  twinkle_area(widget_absolute_x(b),
                               widget_absolute_y(b),
                               widget_width(b),
                               widget_height(b),
                               100);
                if(prev != NULL)
                  widget_set_navigation_leftright(prev, b);
                prev = b;
              }
          
            tx += spacing;
            if(tx + spacing >= x)
              {
                tx = 10;
                ty += spacing;
              }
          }
    last_trait = prev;
  }

  struct widget * uiobj_newgame;

  uiobj_newgame = widget_find(root, "newgame");
  assert(uiobj_newgame != NULL);
  widget_set_focus(uiobj_newgame);
  if(uiobj_highscores != NULL)
    widget_set_navigation_updown(uiobj_highscores, uiobj_newgame);
  else if(uiobj_partystatus != NULL)
    widget_set_navigation_updown(widget_get_pointer(uiobj_partystatus, "focus_down_object"), uiobj_newgame);
  if(last_trait != NULL)
    widget_set_navigation_left(uiobj_newgame, last_trait);

  uiobj_cave_left = widget_find(root, "cave_left");
  assert(uiobj_cave_left != NULL);


  uiobj_cave_right = widget_find(root, "cave_right");
  assert(uiobj_cave_right != NULL);
  

  uiobj_cave = widget_find(root, "cave");
  assert(uiobj_cave != NULL);

  y += font_height() + 5;

  uiobj_editor = widget_find(root, "editor");
  assert(uiobj_editor != NULL);

  widget_set_navigation_updown(uiobj_newgame, uiobj_editor);
  if(last_trait != NULL)
    widget_set_navigation_left(uiobj_editor, last_trait);

  uiobj_level_left = widget_find(root, "level_left");
  assert(uiobj_level_left != NULL);

  uiobj_level_right = widget_find(root, "level_right");
  assert(uiobj_level_right != NULL);

  
  uiobj_level = widget_find(root, "level");
  assert(uiobj_level != NULL);
  widget_add_flags(uiobj_level, WF_FREE_RAW_IMAGE);

  y += font_height() + 5;

  uiobj_config = widget_find(root, "settings");
  assert(uiobj_config != NULL);

  if(last_trait != NULL)
    widget_set_navigation_leftright(last_trait, uiobj_config);

  {
    int tmpw;
    
    tmpw = 50 - 10 + 2;

    uiobj_next_trait = widget_new_trait_button(root,
                                               x + w + 10 + (widget_width(uiobj_level_left) + widget_width(uiobj_level_right) - tmpw) / 2,
                                               y + 2,
                                               tmpw, tmpw,
                                               next_trait_from_this_cave,
                                               false,
                                               true);
    widget_set_ulong(uiobj_next_trait, "obtained_at_level", next_trait_from_this_cave_level);
    widget_set_navigation_leftright(uiobj_config, uiobj_next_trait);
    widget_set_navigation_updown(uiobj_level_right, uiobj_next_trait);
    widget_set_navigation_down(uiobj_level_left, uiobj_next_trait);

    { /* Game mode switch */
      struct image * img;
      enum GFX_IMAGE g;

      switch(current_cave->game_mode)
        {
        default:
        case GAME_MODE_CLASSIC:      g = GFX_IMAGE_MODE_SWITCH_LEFT;  break;
        case GAME_MODE_ADVENTURE:    g = GFX_IMAGE_MODE_SWITCH_UP;    break;
        case GAME_MODE_PYJAMA_PARTY: g = GFX_IMAGE_MODE_SWITCH_RIGHT; break;
        }
      img = gfx_image(g);
      assert(img != NULL);
      uiobj_game_mode = widget_new_button(root,
                                          widget_x(uiobj_next_trait),
                                          widget_y(uiobj_next_trait) + widget_height(uiobj_next_trait) + 4,
                                          NULL);
      widget_set_ulong(uiobj_game_mode, "alpha", 0x80);
      widget_set_image(uiobj_game_mode, "image", g);
      widget_set_width(uiobj_game_mode, img->width + 2);
      widget_set_height(uiobj_game_mode, img->height + 2);
      widget_set_on_release(uiobj_game_mode, button_game_mode);
      widget_set_tooltip(uiobj_game_mode, gettext("Change game mode"));
      widget_set_navigation_up(uiobj_game_mode, uiobj_next_trait);

      if(traits_get_active() & (TRAIT_ADVENTURE_MODE | TRAIT_PYJAMA_PARTY))
        widget_delete_flags(uiobj_game_mode, WF_HIDDEN);
      else
        widget_add_flags(uiobj_game_mode, WF_HIDDEN);
    }


    int s;

    s = widget_width(uiobj_game_mode);

    uiobj_key = widget_new_trait_key_button(root,
                                            widget_x(uiobj_next_trait) + tmpw - s,
                                            widget_y(uiobj_next_trait) + widget_height(uiobj_next_trait) + 4,
                                            s, s,
                                            current_cave,
                                            globals.level_selection);
    widget_set_navigation_updown(uiobj_next_trait, uiobj_key);
    widget_set_navigation_leftright(uiobj_game_mode, uiobj_key);

    if( ! (traits_get_active() & TRAIT_KEY))
      widget_add_flags(uiobj_key, WF_HIDDEN);
  }


  int thumbx;

  thumbx = x + w + 10 + 30 + 30;
  uiobj_level_thumbnail = widget_new_frame(root,
                                           thumbx, y,
                                           widget_width(root) - thumbx - (24 + 2 /* theme button */),
                                           SCREEN_HEIGHT - y - 1);
  assert(uiobj_level_thumbnail != NULL);
  widget_delete_flags(uiobj_level_thumbnail, WF_DRAW_BORDERS | WF_BACKGROUND | WF_CAST_SHADOW);

  y += font_height() + 5;

  uiobj_quit = widget_find(root, "quit");
  assert(uiobj_quit != NULL);

  if(last_trait != NULL)
    widget_set_navigation_left(uiobj_quit, last_trait);

  widget_set_navigation_leftright(uiobj_quit, uiobj_game_mode);



  struct theme * theme;

  theme = theme_get(THEME_SPECIAL_DAY);
  if(theme != NULL)
    widget_new_theme_info_button(root, SCREEN_WIDTH - 24 - 2, 0, theme);

  event_register(EVENT_TYPE_TRAIT_ACTIVATED,   on_trait_changed);
  event_register(EVENT_TYPE_TRAIT_DEACTIVATED, on_trait_changed);

  on_cave_changed();
}


static void unload_ui(void)
{
  event_unregister(EVENT_TYPE_TRAIT_ACTIVATED,   on_trait_changed);
  event_unregister(EVENT_TYPE_TRAIT_DEACTIVATED, on_trait_changed);

  if(uiobj_level_thumbnail != NULL)
    {
      struct image * img;

      img = widget_get_pointer(uiobj_level_thumbnail, "raw_image");
      if(img != NULL)
        img = image_free(img);
    }

  ui_cleanup();
  draw_title_logo_cleanup();
  mainmenu_map = map_free(mainmenu_map);

  uiobj_level_right = NULL;
}

void title_game_intro(void)
{
#ifndef PROFILING
  bool done;
  FPSmanager framerate_manager;
  struct widget * uiobj_highscores;

  uiobj_highscores = widget_find(NULL, "highscores");

  done = false;

  SDL_initFramerate(&framerate_manager);
  SDL_setFramerate(&framerate_manager, 60);

  widget_set_enabled(widget_root(), false); /* root is the map display */

  int stepsize;

  stepsize = 24;
  for(int x = 0; done == false && x < SCREEN_WIDTH / 2 / stepsize; x++)
    {
      struct widget * down[14] =
        {
          widget_find(NULL, "newgame"),
          uiobj_cave,
          uiobj_cave_left,
          uiobj_cave_right,
          uiobj_level,
          uiobj_level_thumbnail,
          uiobj_level_left,
          uiobj_level_right,
          uiobj_next_trait,
          uiobj_key,
          uiobj_config,
          uiobj_editor,
          uiobj_quit,
          widget_find(NULL, "credits")
        };

      /* do left */
      if(uiobj_highscores != NULL)
        widget_set_x(uiobj_highscores, widget_x(uiobj_highscores) - stepsize);
      if(uiobj_partystatus != NULL)
        widget_set_x(uiobj_partystatus, widget_x(uiobj_partystatus) - stepsize);
      for(int i = 0; i < TRAIT_SIZEOF_; i++)
        if(uiobj_trait_buttons[i] != NULL)
          widget_set_x(uiobj_trait_buttons[i], widget_x(uiobj_trait_buttons[i]) - stepsize);

      /* do right */
      widget_set_x(uiobj_help, widget_x(uiobj_help) + stepsize);
      if(uiobj_game_mode != NULL)
        widget_set_x(uiobj_game_mode, widget_x(uiobj_game_mode) + stepsize);

      /* do down */
      for(int i = 0; i < 14; i++)
        if(down[i] != NULL)
          widget_set_y(down[i], widget_y(down[i]) + stepsize);

      ui_draw(true);

      SDL_Event e;

      while(SDL_PollEvent(&e))
        if(e.type == SDL_QUIT || e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN)
          done = true;

      if(done == false)
        SDL_framerateDelay(&framerate_manager);
   }
#endif
}


static void play_game(void)
{ /* Play a game. */
  bool new_max_starting_level;
  trait_t t1, t2;

  new_max_starting_level = false;

  title_game_intro();

  unload_ui();

  if(previous_game != NULL)
    gamedata_free(previous_game);

  t1 = traits_get_available();
  previous_game = game(current_cave, globals.level_selection, globals.iron_girl_mode, false, NULL);
  t2 = traits_get_available();

  assert(previous_game->playback != NULL);
  if(previous_game->playback != NULL)
    {
#ifndef PROFILING
      if(previous_game->playback->game_mode != GAME_MODE_PYJAMA_PARTY)
        {
          char fn[128];

          snprintf(fn, sizeof fn, "last-%d.dgp", (int) previous_game->playback->game_mode);
          playback_save(get_save_filename(fn), previous_game->playback);
        }
#endif

      struct playback * pb;

      pb = previous_game->playback;
      while(pb != NULL)
        {
          if(pb->level > current_cave->max_starting_level)
            {
              current_cave->max_starting_level = pb->level;
              new_max_starting_level = true;
            }

          pb = pb->next;
        }
      if(new_max_starting_level == true)
        set_max_level_selection(current_cave->max_starting_level);
    }


  sfx_music(MUSIC_TITLE, 1);
  set_next_trait_from_this_cave();
  setup_ui(t2 & ~t1);
  widget_set_ulong(uiobj_help, "help_screen", 0);
  help_screen_timer = 0;
  gfx_frame0();
  ui_set_last_user_action(time(NULL));


  struct widget * uiobj_newgame;

  uiobj_newgame = widget_find(NULL, "newgame");
  if(current_cave->savegame.exists && globals.iron_girl_mode == false)
    {
      struct widget * uiobj_credits;

      uiobj_credits = widget_find(NULL, "credits");
      widget_title_credits_set(uiobj_credits, gettext("The game was saved."));
      widget_set_string(uiobj_newgame, "text", gettext("Play (*)"));
    }
  else
    {
      widget_set_string(uiobj_newgame, "text", gettext("Play"));
      new_highscore(previous_game);
    }


  if(new_max_starting_level == true)
    {
      int padding;

      padding = 10;
      twinkle_area(widget_absolute_x(uiobj_level) - padding,
                   widget_absolute_y(uiobj_level) - padding,
                   widget_width(uiobj_level)      + padding * 2,
                   widget_height(uiobj_level)     + padding * 2,
                   75);
    }

  /* Disable playbacking of previous games in pyjama party mode. */
  if(previous_game->playback != NULL)
    if(previous_game->playback->game_mode == GAME_MODE_PYJAMA_PARTY)
      previous_game = gamedata_free(previous_game);
}


static void button_newgame(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
#ifndef PROFILING
  if(widget_enabled(this) == true)
#endif
    if(button == WIDGET_BUTTON_LEFT)
      {
        if(current_cave->savegame.exists == false || globals.iron_girl_mode == true)
          { /* No savegame there, start a new game immediately. */
            play_game();
          }
        else
          { /* Savegame exists, ask the player what to do. */
            struct widget * window;
            struct widget * tmp;
            struct widget * buttons[3];
            char buf[128];
            int y;

            window = widget_new_window(widget_root(), 230, 150, gettext("A saved game exists"));
            uiobj_newgame_window = window;
            assert(window != NULL);
            widget_set_modal(window);

            y = 24;
            
            snprintf(buf, sizeof buf, "%d", current_cave->savegame.game_level);
            tmp = widget_new_text(window, 20, y, gettext("Level:"));
            tmp = widget_new_text(window, 80, y, buf);

            snprintf(buf, sizeof buf, "%d", current_cave->savegame.girls);
            tmp = widget_new_text(window, 120, y, gettext("Girls:"));
            tmp = widget_new_text(window, 180, y, buf);
            y += widget_height(tmp);

            snprintf(buf, sizeof buf, "%lu", (unsigned long) current_cave->savegame.score);
            tmp = widget_new_text(window, 20, y, gettext("Score:"));
            tmp = widget_new_text(window, 80, y, buf);
            y += widget_height(tmp);

            y += 8;

            buttons[0] = tmp = widget_new_button(window, 0, y, gettext("Continue"));
            widget_set_width(tmp, 120);
            widget_set_x(tmp, (widget_width(window) - widget_width(tmp)) / 2);
            y += widget_height(tmp) + 2;
            widget_set_on_release(tmp, button_newgame_resume);
            widget_set_focus(tmp);

            buttons[1] = tmp = widget_new_button(window, 0, y, gettext("Start New"));
            widget_set_width(tmp, 120);
            widget_set_x(tmp, (widget_width(window) - widget_width(tmp)) / 2);
            widget_set_on_release(tmp, button_newgame_new);
            y += widget_height(tmp) + 2;

            y += 4;

            buttons[2] = tmp = widget_new_button(window, 0, y, gettext("Cancel"));
            widget_set_width(tmp, 120);
            widget_set_x(tmp, (widget_width(window) - widget_width(tmp)) / 2);
            widget_set_on_release(tmp, button_newgame_cancel);

            widget_set_navigation_updown(buttons[0], buttons[1]);
            widget_set_navigation_updown(buttons[1], buttons[2]);

            ui_push_handlers();
            ui_set_handler(UIC_CANCEL, button_newgame_cancel2);
          }
      }
}

static void button_newgame_resume(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  ui_pop_handlers();
  uiobj_newgame_window = widget_delete(uiobj_newgame_window);
  play_game();
}

static void button_newgame_new(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  assert(current_cave->savegame.exists == true);
  ui_pop_handlers();
  uiobj_newgame_window = widget_delete(uiobj_newgame_window);

  /* Build gamedata for a new highscore entry. */
  struct gamedata * gamedata;

  gamedata = gamedata_new(current_cave, globals.iron_girl_mode, traits_get(current_cave->game_mode));
  gamedata->playback       = current_cave->savegame.playback;
  gamedata->score          = current_cave->savegame.score;
  gamedata->diamond_score  = current_cave->savegame.diamond_score;
  gamedata->diamonds       = current_cave->savegame.diamonds;
  gamedata->starting_girls = current_cave->savegame.starting_girls;
  new_highscore(gamedata);
  gamedata = gamedata_free(gamedata);
  
  /* Throw away the current savegame.
   * Note that the playback is not free'd here, because it is done in highscore_new() called by new_highscore() above.
   */
  current_cave->savegame.exists = false;
  if(current_cave->game_mode == GAME_MODE_PYJAMA_PARTY)
    free(current_cave->savegame.pyjama_party_girls);

  play_game();
}

static void button_newgame_cancel(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  ui_pop_handlers();
  uiobj_newgame_window = widget_delete(uiobj_newgame_window);

  struct widget * uiobj_newgame;

  uiobj_newgame = widget_find(NULL, "newgame");
  if(uiobj_newgame != NULL)
    widget_set_focus(uiobj_newgame);
}

static void button_newgame_cancel2(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed)
    button_newgame_cancel(NULL, WIDGET_BUTTON_LEFT);
}


static void button_config(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  settings(on_settings_changed);
}

static void on_settings_changed(bool gfx_restart, bool sfx_restart, bool new_opengl)
{
  if(quit == false)
    {
      if(sfx_restart)
        {
          sfx_volume(0.05f);
          sfx_music_volume(1.0f);
          sfx_music(MUSIC_TITLE, 1);
        }

      if(gfx_restart)
        {
          help_screen_timer = mainmenu_map->frames_per_second * help_screen_seconds;
          reset_ui(true, new_opengl, 0);
        }
      
      if(!gfx_restart)
        { /* The UI looks different depending on game mode, but too lazy to change the settings() to tell us when it has. */
          unload_ui();
          setup_ui(0);
        }

      widget_set_focus(uiobj_config);
    }
}


static void button_editor(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    if(globals.cave_selection[0] != '/')
      {
        ui_state_push();
        sfx_music_stop();
        map_editor(current_cave, globals.level_selection);
        sfx_music(MUSIC_TITLE, 1);
        ui_state_pop();
        gfx_frame0();
        ui_set_last_user_action(time(NULL));
      }
}



static void button_quit(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      quit = true;
    }
}

static void button_game_mode(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct widget * window, * prev;
  int x, y, w, padding;
  struct 
  {
    struct widget * widget;
    char *          name;
    enum GAME_MODE  game_mode;
    trait_t         trait_req;
  } buttons[3] =
      {
        { NULL, NULL, GAME_MODE_CLASSIC,      0                    },
        { NULL, NULL, GAME_MODE_ADVENTURE,    TRAIT_ADVENTURE_MODE },
        { NULL, NULL, GAME_MODE_PYJAMA_PARTY, TRAIT_PYJAMA_PARTY   }
      };

  buttons[0].name = gettext("Classic");
  buttons[1].name = gettext("Adventure");
  buttons[2].name = gettext("Pyjama Party");
  
  padding = 12;
  x = padding;
  y = font_height() + padding;
  w = 180;

  window = widget_new_window(widget_root(), w, 10, gettext("Game mode:"));

  prev = NULL;
  for(int i = 0; i < 3; i++)
    if(buttons[i].trait_req == 0 || (traits_get(globals.title_game_mode) & buttons[i].trait_req))
      {
        buttons[i].widget = widget_new_button(window, x, y, buttons[i].name);
        if(globals.title_game_mode == buttons[i].game_mode)
          widget_set_focus(buttons[i].widget);
        widget_set_width(buttons[i].widget, w - padding * 2);
        widget_set_ulong(buttons[i].widget, "game_mode", buttons[i].game_mode);
        widget_set_on_release(buttons[i].widget, on_change_game_mode);

        y += widget_height(buttons[i].widget);

        if(prev != NULL)
          widget_set_navigation_updown(prev, buttons[i].widget);

        prev = buttons[i].widget;
      }
  widget_set_height(window, y + padding);
  widget_set_x(window, widget_absolute_x(this) - widget_width(window) / 2);
  widget_set_y(window, widget_absolute_y(this) - widget_height(window));
  widget_set_modal(window);
}

static void on_change_game_mode(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  globals.title_game_mode = widget_get_ulong(this, "game_mode");
  widget_delete(widget_parent(this));

  reset_ui(false, globals.opengl, 0);
}

static void on_cave_changed(void)
{
  if(uiobj_cave != NULL)
    {
      char * cave;
      int caven;

      cave = globals.cave_selection;
      caven = get_cave_number(cave);

      if(caven > 0)
        widget_set_enabled(uiobj_cave_left, true);
      else
        widget_set_enabled(uiobj_cave_left, false);
      
      if(cave_names != NULL && cave_names[caven + 1] != NULL)
        widget_set_enabled(uiobj_cave_right, true);
      else
        widget_set_enabled(uiobj_cave_right, false);
      
      if(cave[0] == '/')
        {
          if(uiobj_editor != NULL)
            {
              widget_set_string(uiobj_editor, "text", gettext("Editor N/A"));
              widget_set_enabled(uiobj_editor, false);
            }
        }
      else
        {
          if(uiobj_editor != NULL)
            {
              widget_set_string(uiobj_editor, "text", gettext("Editor"));
              widget_set_enabled(uiobj_editor, true);
            }
        }

      widget_set_string(uiobj_cave, "text", gettext("Cave: %s"), cave_displayname(cave));

      assert(uiobj_next_trait != NULL);
      if(uiobj_next_trait != NULL)
        {
          int x, y, w, h;

          x = widget_x(uiobj_next_trait);
          y = widget_y(uiobj_next_trait);
          w = widget_width(uiobj_next_trait);
          h = widget_height(uiobj_next_trait);
          widget_delete(uiobj_next_trait);

          uiobj_next_trait = widget_new_trait_button(widget_root(), x, y, w, h, next_trait_from_this_cave, false, true);
          widget_set_ulong(uiobj_next_trait, "obtained_at_level", next_trait_from_this_cave_level);

          widget_set_enabled(uiobj_next_trait, next_trait_from_this_cave != 0 ? true : false);

          widget_set_navigation_leftright(uiobj_config, uiobj_next_trait);
          widget_set_navigation_updown(uiobj_level_right, uiobj_next_trait);
          widget_set_navigation_up(uiobj_game_mode, uiobj_next_trait);
          widget_set_navigation_updown(uiobj_next_trait, uiobj_key);
          widget_set_navigation_down(uiobj_level_left, uiobj_next_trait);
        }

      struct widget * uiobj_highscores;
      struct widget * uiobj_newgame;

      uiobj_highscores = widget_find(NULL, "highscores");
      uiobj_newgame = widget_find(NULL, "newgame");
      if(uiobj_highscores != NULL)
        widget_set_pointer(uiobj_highscores, "cave", current_cave);

      if(current_cave->savegame.exists && globals.iron_girl_mode == false)
        widget_set_string(uiobj_newgame, "text", gettext("Play (*)"));
      else
        widget_set_string(uiobj_newgame, "text", gettext("Play"));
      
      on_level_changed();
    }
}

static void on_level_changed(void)
{
  trait_t traits;
  struct widget * uiobj_newgame;

  traits = traits_get(current_cave->game_mode);

  uiobj_newgame = widget_find(NULL, "newgame");
  if(uiobj_newgame != NULL)
    {
      int n;
      bool ok;

      ok     = false;
      n      = (globals.level_selection - 1) % 5;

      if(n == 0)
        ok = true;
      else if(n == 1 && (traits & TRAIT_STARS1))
        ok = true;
      else if(n == 2 && (traits & TRAIT_STARS2))
        ok = true;
      else if(n == 3 && (traits & TRAIT_STARS3))
        ok = true;

      widget_set_enabled(uiobj_newgame, ok);
    }

  if(globals.level_selection > 1)
    widget_set_enabled(uiobj_level_left, true);
  else
    widget_set_enabled(uiobj_level_left, false);

  if(globals.level_selection < current_cave->max_starting_level)
    {
      widget_set_enabled(uiobj_level_right, true);
      if(traits & TRAIT_KEY)
        widget_add_flags(uiobj_key, WF_HIDDEN);
    }
  else
    {
      widget_set_enabled(uiobj_level_right, false);
      if(traits & TRAIT_KEY)
        {
          widget_delete_flags(uiobj_key, WF_HIDDEN);
          widget_set_pointer(uiobj_key, "cave", current_cave);
          widget_set_ulong(uiobj_key, "level", globals.level_selection);

          int padding;

          padding = 10;
          twinkle_area(widget_absolute_x(uiobj_key) - padding,
                       widget_absolute_y(uiobj_key) - padding,
                       widget_width(uiobj_key)      + padding * 2,
                       widget_height(uiobj_key)     + padding * 2,
                       10);
        }
    }

  if(uiobj_level != NULL)
    widget_set_string(uiobj_level, "text", gettext("Level: %d / %d"), (int) globals.level_selection, (int) current_cave->max_starting_level);

  struct map * map;

  map = map_get(globals.cave_selection, globals.level_selection);
  if(map != NULL)
    {
      struct image * img;

      img = map_thumbnail_image(map);
      if(img != NULL)
        {
          struct image * old;

          old = widget_get_pointer(uiobj_level_thumbnail, "raw_image");
          if(old != NULL)
            old = image_free(old);
          widget_set_pointer(uiobj_level_thumbnail, "raw_image", img);
        }

      map = map_free(map);
    }
}

static void button_cave_left(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  int caven;

  caven = get_cave_number(globals.cave_selection);
  if(caven > 0)
    {
      set_cave_selection(cave_names[caven - 1]);

      struct widget * uiobj_highscores;
      
      uiobj_highscores = widget_find(NULL, "highscores");
      if(uiobj_highscores != NULL)
        widget_set_ulong(uiobj_highscores, "first_highscore", 0);
      
      load_cave_data();
    }
}


static void button_cave_right(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  int caven;

  caven = get_cave_number(globals.cave_selection);
  if(cave_names != NULL && cave_names[caven + 1] != NULL)
    {
      set_cave_selection(cave_names[caven + 1]);

      struct widget * uiobj_highscores;

      uiobj_highscores = widget_find(NULL, "highscores");
      if(uiobj_highscores != NULL)
        widget_set_ulong(uiobj_highscores, "first_highscore", 0);

      load_cave_data();
    }
}



static void button_level_left(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(globals.level_selection > 1)
    {
      if(button == WIDGET_BUTTON_RIGHT)
        {
          int n;

          n = (globals.level_selection - 1) % 5;
          if(n > 0)
            globals.level_selection = globals.level_selection - n;
          else
            globals.level_selection = globals.level_selection - 5;

          if(globals.level_selection < 1)
            globals.level_selection = 1;
        }
      else if(button == WIDGET_BUTTON_LEFT)
        {
          globals.level_selection--;
        }
      on_level_changed();
    }
}

static void button_level_right(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_RIGHT)
    {
      int nsel;

      nsel = globals.level_selection - 1;
      nsel = nsel - (nsel % 5) + 5 + 1;
      if(nsel <= current_cave->max_starting_level)
        {
          globals.level_selection = nsel;
          on_level_changed();
        }
    }
  else if(button == WIDGET_BUTTON_LEFT)
    {
      if(globals.level_selection < current_cave->max_starting_level)
        {
          globals.level_selection++;
          on_level_changed();
        }
    }
}


static void on_highscore_activated(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  int selected_highscore;

  selected_highscore = widget_get_ulong(this, "selected_highscore");
  if(selected_highscore >= 0)
    {
      struct highscore_entry ** highscores;
      size_t highscores_size;

      highscores = highscores_get(current_cave->highscores, &highscores_size);
      if(highscores != NULL)
        if(selected_highscore < (int) highscores_size)
          widget_new_highscore_einfo(widget_root(), highscores[selected_highscore]);
    }
}




static void reset_ui(bool reset_gfx, bool new_opengl, trait_t traits_to_highlight)
{
  load_cave_data();

  unload_ui();

  if(reset_gfx)
    {
      gfx_cleanup();
      globals.opengl = new_opengl;
      gfx_initialize();
    }

  setup_ui(traits_to_highlight);
}





static int get_cave_number(const char * cave)
{
  int n;
  
  n = -1;
  for(int i = 0; n == -1 && cave_names[i] != NULL; i++)
    if(!strcmp(cave, cave_names[i]))
      n = i;

  assert(n >= 0);
  
  return n;
}


void set_max_level_selection(int new_max)
{
  if(new_max > current_cave->level_count)
    new_max = current_cave->level_count;

  /* Last 20 random maps must be played within one game. */
  if(!strcmp(globals.cave_selection, "/random") && new_max > 81)
    new_max = 81;

  current_cave->max_starting_level = new_max;

  if(uiobj_level_right != NULL)
    widget_set_enabled(uiobj_level_right, true);
}


static void set_next_trait_from_this_cave(void)
{ /* Determine the next possible trait obtainable from this cave. */
  trait_t allt;
  
  next_trait_from_this_cave       = 0;
  next_trait_from_this_cave_level = 0;
  allt = traits_get_available();

  for(int i = 1; next_trait_from_this_cave == 0 && i <= current_cave->level_count; i++)
    {
      trait_t tmp;

      tmp = traits_level_gives(current_cave, i);
      if( (tmp & allt) == 0)
        {
          next_trait_from_this_cave       = tmp;
          next_trait_from_this_cave_level = i;
        }
    }
}


static void on_trait_changed(int64_t changes)
{
  trait_t traits;
  bool settings_changed;

  traits = traits_get_active();
  settings_changed = false;

  /* Switch game mode if a game mode trait is changed. */
  if(changes & TRAIT_ADVENTURE_MODE)
    {
      settings_changed = true;
      if(traits & TRAIT_ADVENTURE_MODE)
        {
          globals.title_game_mode = GAME_MODE_ADVENTURE;
        }
      else
        {
          if(globals.title_game_mode == GAME_MODE_ADVENTURE)
            globals.title_game_mode = GAME_MODE_CLASSIC;
        }
    }
  if(changes & TRAIT_PYJAMA_PARTY)
    {
      settings_changed = true;
      if(traits & TRAIT_PYJAMA_PARTY)
        {
          globals.title_game_mode = GAME_MODE_PYJAMA_PARTY;
        }
      else
        {
          if(globals.title_game_mode == GAME_MODE_PYJAMA_PARTY)
            globals.title_game_mode = GAME_MODE_CLASSIC;
        }
    }

  reset_ui(false, globals.opengl, changes);

  if(settings_changed)
    twinkle_area(widget_absolute_x(uiobj_config),
                 widget_absolute_y(uiobj_config),
                 widget_width(uiobj_config),
                 widget_height(uiobj_config),
                 100);
}


static void new_highscore(struct gamedata * gamedata)
{
  int highscore_position;
  struct highscorelist * hl;

  assert(gamedata->playback != NULL);

  hl = current_cave->highscores;
  hl->total.caves_entered      += 1;
  hl->total.levels_completed   += playback_get_levels_completed(gamedata->playback);
  hl->total.diamonds_collected += gamedata->diamonds;
  hl->total.girls_died         += playback_get_girls_died(gamedata->playback);
  highscore_position = highscore_new(hl, gamedata, NULL);
  highscores_save(hl, current_cave->game_mode, NULL);

  struct widget * uiobj_highscores;
        
  uiobj_highscores = widget_find(NULL, "highscores");
  if(uiobj_highscores != NULL)
    if(highscore_position >= 0)
      {
        int amount, padding;

        amount = widget_get_ulong(uiobj_highscores, "amount");

        padding = 6;
        twinkle_area(widget_absolute_x(uiobj_highscores),
                     widget_absolute_y(uiobj_highscores) + ((2 + highscore_position) % amount) * font_height() - padding / 2,
                     widget_width(uiobj_highscores),
                     font_height() + padding,
                     75);

        highscore_position = highscore_position - (highscore_position % amount);
        widget_set_ulong(uiobj_highscores, "first_highscore", highscore_position);
      }
}

