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

#include "ai.h"
#include "cave.h"
#include "diamond_girl.h"
#include "event.h"
#include "gc.h"
#include "gfx.h"
#include "girl.h"
#include "globals.h"
#include "highscore.h"
#include "names.h"
#include "quest.h"
#include "random.h"
#include "sfx.h"
#include "stack.h"
#include "themes.h"
#include "title.h"
#include "trader.h"
#include "traits.h"
#include "twinkle.h"
#include "ui.h"

#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mikmod.h>


static bool cmdline_play_sfx(const char * name);
static void joysticks_initialize(void);
static void on_trait_activated(void * user_data, int64_t traits);
static void on_trait_deactivated(void * user_data, int64_t traits);

struct globals globals;


int main(int argc, char * argv[])
{
  int rv;

  rv = EXIT_FAILURE;

  /* Setup localization stuff: */
#ifdef WIN32
#undef LOCALEDIR
  char LOCALEDIR[MAX_PATH];
  snprintf(LOCALEDIR, sizeof LOCALEDIR, "%s", get_data_filename("locale"));
#endif
  
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  globals.locale_save = false;
  globals.locale      = NULL;
  globals.language    = NULL;
  set_language(setlocale(LC_MESSAGES, NULL));

  

  MikMod_RegisterAllDrivers();
  MikMod_RegisterAllLoaders();

  gc_initialize();
  seed_rand(time(NULL));

  globals.volume    = 100;
  globals.title_game_mode = GAME_MODE_CLASSIC;
  globals.iron_girl_mode  = false;

  globals.questlines_size  = 0;
  globals.active_questline = MAX_QUESTLINES;

  for(int i = 0; i < MAX_TRADERS; i++)
    globals.traders[i] = NULL;
  globals.active_trader    = 0;

  globals.skip_intro       = false;
  globals.use_sfx          = true;
  globals.use_music        = true;
  globals.max_channels     = 16;
  globals.use_joysticks    = true;
  globals.fullscreen       = false;
#ifdef WITH_OPENGL
  globals.opengl           = true;
#else
  globals.opengl           = false;
#endif
  globals.opengl_power_of_two_textures             = false;
  globals.opengl_power_of_two_textures_benchmarked = false;
  globals.opengl_compressed_textures               = false;
  globals.opengl_max_texture_size                  = 0;
  globals.opengl_swap_control                      = true;
  globals.opengl_1_4                               = false;
  globals.opengl_1_5                               = false;
  globals.opengl_disable_shaders                   = false;
  globals.opengl_shaders                           = 0;
  globals.gfx_girl_animation                       = true;
  globals.gfx_diamond_animation                    = true;
  globals.cave_selection            = NULL;
  set_cave_selection("beginners_cave");
  globals.level_selection           = 1;
  globals.screen_capture_path       = NULL;
  globals.screen_capture_frameskip  = 1;
  strcpy(globals.playback_name, "");
  strcpy(globals.play_sfx_name, "");
  globals.ai_hilite_current_target  = false;
  globals.smooth_classic_mode       = true;
  globals.write_settings            = true;
  globals.read_only                 = false;
  globals.fps_counter_active        = false;
  globals.map_tilting               = true;
#ifndef NDEBUG
  globals.backtrace                 = false;
  globals.quit_after_initialization = false;
  globals.save_sound_effects        = false;
#endif
#ifdef PROFILING
  globals.read_only  = true;
  globals.skip_intro = true;
#endif
  
  globals.joysticks       = NULL;
  globals.joysticks_count = 0;

  globals.pyjama_party_girls_size = 0;
  globals.pyjama_party_girls      = NULL;

  names_initialize();
  events_initialize();
  
  event_register(EVENT_TYPE_TRAIT_ACTIVATED, on_trait_activated, NULL);
  event_register(EVENT_TYPE_TRAIT_DEACTIVATED, on_trait_deactivated, NULL);

  if(setup_user_homedir() == true)
    {
      themes_initialize();

      ui_bindings_initialize();
      ui_bindings_default(false);
      
      settings_default();
      traits_initialize();

      if(check_installation() == true)
        {
          if(settings_read() == true)
            {
              if(parse_commandline_arguments(argc, argv))
                {
                  Uint32 flags;

                  if(globals.language != NULL && strcmp(globals.language, "en"))
                    {
                      char * lang;
                      
                      lang = gettext("_current_locale");
                      if(strcmp(lang, globals.language))
                        {
                          fprintf(stderr, "Locale '%s' is not supported, falling back to default.\n", globals.locale);
                          set_language(NULL);
                        }
                    }
                  printf("Using language: %s\n", globals.language);
#ifndef NDEBUG
                  printf("Locale '%s' from '%s'\n", globals.locale, LOCALEDIR);
#endif
                  if(globals.use_joysticks == true)
                    flags = SDL_INIT_JOYSTICK;
                  else
                    flags = 0;
                  if(SDL_Init(flags) == 0)
                    {
                      if(strlen(globals.play_sfx_name) > 0)
                        {
                          if(cmdline_play_sfx(globals.play_sfx_name) == true)
                            rv = EXIT_SUCCESS;
                        }
                      else
                        {
                          ai_brains_load(get_save_filename("lucy.brains"));
                          pyjama_party_load();
                          joysticks_initialize();
              
                          if(gfx_initialize())
                            {
                              rv = EXIT_SUCCESS;

                              questline_load();
                              twinkle_initialize();
                              sfx_initialize();

#ifndef NDEBUG
                              if(globals.quit_after_initialization == false)
                                {
#endif
                                  if(globals.skip_intro == false)
                                    intro();

                                  title();
#ifndef NDEBUG
                                }
#endif

                              if(globals.read_only == false)
                                {
                                  pyjama_party_save();
                                  ai_brains_save(get_save_filename("lucy.brains"));
                                  settings_write();

                                  questline_save();
                                  traits_save();
                                }
                      
                              twinkle_cleanup();
                              sfx_cleanup();
                            }
                          else
                            rv = EXIT_FAILURE;
                  
                          gfx_cleanup();

                          if(globals.joysticks != NULL)
                            {
                              for(int i = 0; i < globals.joysticks_count; i++)
                                if(globals.joysticks[i] != NULL)
                                  SDL_JoystickClose(globals.joysticks[i]);
                              free(globals.joysticks);
                            }

                          ai_brains_cleanup();
                        }

                      SDL_Quit();
                    }
                  else
                    {
                      fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
                      rv = EXIT_FAILURE;
                    }
                }
              else
                {
                  rv = EXIT_FAILURE;
                }
            }
          else
            {
              rv = EXIT_FAILURE;
            }
        }
      else
        {
          fprintf(stderr, "Installation check failed.\n");
          rv = EXIT_FAILURE;
        }

      traits_cleanup();
      themes_cleanup();
      ui_bindings_cleanup();
    }
  else
    {
      fprintf(stderr, "Failed to setup savedir.\n");
      rv = EXIT_FAILURE;
    }

  for(int i = 0; i < MAX_TRADERS; i++)
    if(globals.traders[i] != NULL)
      globals.traders[i] = trader_free(globals.traders[i]);
  
  free(globals.locale);
  free(globals.language);
  free(globals.screen_capture_path);
  free(globals.cave_selection);

  cave_cleanup();
  events_cleanup();
  names_cleanup();
  gc_cleanup();

  return rv;
}


static bool cmdline_play_sfx(const char * name)
{
  struct
  {
    enum SFX sfx;
    char *   name;
  } sounds[] = {
    { SFX_BOULDER_FALL,     "boulder_fall"     },
    { SFX_BOULDER_MOVE,     "boulder_move"     },
    { SFX_DIAMOND_FALL,     "diamond_fall"     },
    { SFX_DIAMOND_COLLECT,  "diamond_collect"  },
    { SFX_MOVE_EMPTY,       "move_empty"       },
    { SFX_MOVE_SAND,        "move_sand"        },
    { SFX_COLLECT_TREASURE, "collect_treasure" },
    { SFX_AMEBA,            "ameba"            },
    { SFX_EXPLOSION,        "explosion"        },
    { SFX_SMALL_EXPLOSION,  "small_explosion"  },
    { SFX_TIME,             "time"             },
    { SFX_ENTER_EXIT,       "enter_exit"       },
    { SFX_INTRO,            "intro"            },
    { SFX_SIZEOF_,          NULL               }
  };
  enum SFX sfx;
  bool rv;

  rv = false;
  sfx = SFX_SIZEOF_;
  for(int i = 0; sfx == SFX_SIZEOF_ && sounds[i].sfx != SFX_SIZEOF_; i++)
    if(!strcmp(sounds[i].name, name))
      sfx = sounds[i].sfx;

  if(sfx != SFX_SIZEOF_)
    {
      float len;

      sfx_initialize();

      len = sfx_length(sfx);

      printf("Playback %.2f seconds:", (double) len);
      fflush(stdout);

      sfx_volume(1.0);
      sfx_set_listener_position(0, 0);
      sfx_emit(sfx, 0, 0);
                              
      while(len > 0.0f)
        {
          float sleeplen;

          sleeplen = 0.5f;
          if(sleeplen > len)
            sleeplen = len;
          usleep(1000000.0f * sleeplen);
          printf(".");
          fflush(stdout);
          len -= sleeplen;
        }
      printf("\n");
      usleep(1000000.0f * 0.2f);
      sfx_cleanup();
      rv = true;
    }
  else
    {
      printf("Error, unknown sound '%s'.\n", name);
      printf("Valid sounds are: ");
      for(int i = 0; sounds[i].sfx != SFX_SIZEOF_; i++)
        {
          if(i > 0)
            printf(", ");
          printf("%s", sounds[i].name);
        }
      printf("\n");
    }

  return rv;
}


static void joysticks_initialize(void)
{
  if(globals.use_joysticks == true)
    {
      globals.joysticks_count = SDL_NumJoysticks();
      if(globals.joysticks_count > 0)
        {
          globals.joysticks = malloc(globals.joysticks_count * sizeof(SDL_Joystick *));
          globals.joysticks_analog = malloc(globals.joysticks_count * sizeof(bool));
          globals.joysticks_digital_counter = malloc(globals.joysticks_count * sizeof(int));
          assert(globals.joysticks != NULL && globals.joysticks_analog != NULL && globals.joysticks_digital_counter != NULL);
          if(globals.joysticks != NULL)
            for(int i = 0; i < globals.joysticks_count; i++)
              {
                globals.joysticks[i] = SDL_JoystickOpen(i);
                globals.joysticks_analog[i] = false;
                globals.joysticks_digital_counter[i] = 0;
              }
        }
    }
}

static void on_trait_activated(void * user_data DG_UNUSED, int64_t traits)
{
  if(traits & TRAIT_PYJAMA_PARTY)
    {
      globals.pyjama_party_girls_size = 1;
      globals.pyjama_party_girls = malloc(sizeof(struct girl *) * globals.pyjama_party_girls_size);
      assert(globals.pyjama_party_girls != NULL);
      for(int i = 0; i < globals.pyjama_party_girls_size; i++)
        globals.pyjama_party_girls[i] = girl_new();
      assert(globals.pyjama_party_girls_size >= 1);
      globals.pyjama_party_girls[0]->traits = traits_get_active();
      strcpy(globals.pyjama_party_girls[0]->name, "Lucy");
    }

  if(traits & TRAIT_QUESTS)
    {
      struct questline * ql;
          
      ql = questline_generate(QUEST_TYPE_RELATIVE);
      if(ql != NULL)
        {
          globals.questlines[globals.questlines_size] = ql;
          globals.active_questline = globals.questlines_size;
          globals.questlines_size++;

          if(ql->current_quest == 0)
            {
              struct quest * first;
          
              assert(ql->quests->size > 0);
              first = ql->quests->data[0];
              quest_open(first);
            }
        }
      
          
      ql = questline_generate(QUEST_TYPE_CHILDHOOD_DREAM);
      if(ql != NULL)
        {
          globals.questlines[globals.questlines_size] = ql;
          globals.questlines_size++;
        }
    }
}

static void on_trait_deactivated(void * user_data DG_UNUSED, int64_t traits)
{
  if(traits & TRAIT_PYJAMA_PARTY)
    {
      char ** caves;

      caves = get_cave_names(false);
      for(int i = 0; caves[i] != NULL; i++)
        {
          struct cave * cave;

          cave = cave_get(GAME_MODE_PYJAMA_PARTY, caves[i]);
          if(cave != NULL)
            {
              highscores_delete(cave->highscores);
              highscores_free(cave->highscores);
              cave->highscores = highscores_new();
              highscores_load(cave->highscores, cave->game_mode, cave->name);
            }
        }
      caves = free_cave_names(caves);

      unlink(get_save_filename("party/order.bz2"));

      for(int i = 0; i < globals.pyjama_party_girls_size; i++)
        {
          char fn[1024];

          snprintf(fn, sizeof fn, "party/%s.bz2", globals.pyjama_party_girls[i]->name);
          unlink(get_save_filename(fn));
          globals.pyjama_party_girls[i] = girl_free(globals.pyjama_party_girls[i]);
        }
      free(globals.pyjama_party_girls);

      globals.pyjama_party_girls_size = 0;
      globals.pyjama_party_girls      = NULL;
    }
}

