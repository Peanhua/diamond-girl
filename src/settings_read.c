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
#include "globals.h"
#include "ui.h"
#include "traits.h"
#include "themes.h"
#include "cave.h"
#include "gc.h"
#include "stack.h"

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <json.h>
#include <unistd.h>


static bool cb_bool(struct json_object * value, void * value_ptr);
static bool cb_int(struct json_object * value, void * value_ptr);
static bool cb_opengl_pot(struct json_object * value, void * value_ptr);
static bool cb_sound_volume(struct json_object * value, void * value_ptr);
static bool cb_cave_selection(struct json_object * value, void * value_ptr);
static bool cb_cave_selection_level(struct json_object * value, void * value_ptr);
static bool cb_game_mode(struct json_object * value, void * value_ptr);
static bool cb_title_midarea(struct json_object * value, void * value_ptr);
static bool cb_locale(struct json_object * value, void * value_ptr);
static bool cb_bindings(struct json_object * value, void * value_ptr);
static bool cb_themes(struct json_object * value, void * value_ptr);


static char filename[1024];


bool settings_read(void)
{
  bool rv;
  bool bind_defaults;
  bool nonexistent_ok;

  rv             = true;
  bind_defaults  = true;
  nonexistent_ok = false;
  
  /* Read and delete the old versions first if it exists. */
  settings_read_v1();

  if(settings_read_v2())
    {
      bind_defaults = false;
      nonexistent_ok = true; // If we managed to read the old .lua file, then failing to read the new .json file should not cause any error messages.
    }


  /* Read the new version if it exists. */
  char * json;
  int    json_size;

  snprintf(filename, sizeof filename, "%s", get_save_filename("settings.json"));
  if(read_file(filename, &json, &json_size) == true)
    {
      enum json_tokener_error e;
      struct json_object * obj;

      obj = json_tokener_parse_verbose(json, &e);
      if(obj != NULL)
        {
          bool ok;
          struct json_object_iterator it;
          struct json_object_iterator itEnd;
          int active_questline;

          ok    = true;
          it    = json_object_iter_begin(obj);
          itEnd = json_object_iter_end(obj);

          active_questline = globals.active_questline;

          while(ok == true && !json_object_iter_equal(&it, &itEnd))
            {
              const char *         name;
              struct json_object * value;
              enum json_type       type;
              bool                 dummy_boolean;
              
              name  = json_object_iter_peek_name(&it);
              value = json_object_iter_peek_value(&it);
              type = json_object_get_type(value);

              struct
              {
                const char *   name;
                enum json_type type;
                bool (*callback)(struct json_object * value, void * value_ptr);
                void *         value_ptr;
              } stuff[] =
                  {
                    { "Fullscreen",                 json_type_boolean, cb_bool,                 &globals.fullscreen                   },
                    { "OpenGL",                     json_type_boolean, cb_bool,                 &globals.opengl                       },
                    { "OpenGL_POT",                 json_type_boolean, cb_opengl_pot,           NULL                                  },               
                    { "OpenGL_compressed_textures", json_type_boolean, cb_bool,                 &globals.opengl_compressed_textures   },
                    { "SFX",                        json_type_boolean, cb_bool,                 &globals.use_sfx                      },
                    { "Music",                      json_type_boolean, cb_bool,                 &globals.use_music                    },
                    { "SFX_max_channels",           json_type_int,     cb_int,                  &globals.max_channels                 },
                    { "Sound_volume",               json_type_int,     cb_sound_volume,         NULL                                  },
                    { "Joysticks",                  json_type_boolean, cb_bool,                 &globals.use_joysticks                },
                    { "CaveSelection",              json_type_string,  cb_cave_selection,       NULL                                  },
                    { "CaveSelectionLevel",         json_type_int,     cb_cave_selection_level, NULL                                  },
                    { "ShowFPS",                    json_type_boolean, cb_bool,                 &globals.fps_counter_active           },
                    { "GameMode",                   json_type_string,  cb_game_mode,            NULL                                  },
                    { "ActiveQuestline",            json_type_int,     cb_int,                  &active_questline                     },
                    { "TitleMidarea",               json_type_string,  cb_title_midarea,        NULL                                  },
                    { "IronGirlMode",               json_type_boolean, cb_bool,                 &globals.iron_girl_mode               },
                    { "Locale",                     json_type_string,  cb_locale,               NULL                                  },
                    { "MapTilting",                 json_type_boolean, cb_bool,                 &globals.map_tilting                  },
                    { "SmoothClassicMode",          json_type_boolean, cb_bool,                 &globals.smooth_classic_mode          },
                    { "Bindings",                   json_type_array,   cb_bindings,             NULL                                  },
                    { "Themes",                     json_type_array,   cb_themes,               NULL                                  },
                    { "WriteSettings",              json_type_boolean, cb_bool,                 &globals.write_settings               },
                    { "TestsCompleted",             json_type_boolean, cb_bool,                 &dummy_boolean                        }, // Ignored.
                    { NULL,                         json_type_int,     NULL,                    NULL                                  }
                  };
              
              int ind;

              ind = -1;
              for(int i = 0; ind == -1 && stuff[i].name != NULL; i++)
                if(!strcmp(stuff[i].name, name))
                  ind = i;

              if(ind >= 0)
                {
                  if(stuff[ind].type == type)
                    {
                      ok = stuff[ind].callback(value, stuff[ind].value_ptr);

                      /* Don't bind the default keybindings if we were able to read them from the settings.json. */
                      if(ok == true && !strcmp(name, "Bindings"))
                        bind_defaults = false;
                    }
                  else
                    {
                      fprintf(stderr, "%s: Incorrect value type %s for '%s', expected %s.\n", filename, json_type_to_name(type), name, json_type_to_name(stuff[ind].type));
                      ok = false;
                    }
                }
              else
                {
                  fprintf(stderr, "%s: Failed to parse: unknown variable '%s'.\n", filename, name);
                  ok = false;
                }

              json_object_iter_next(&it);
            }

          if(ok == true)
            globals.active_questline = active_questline;
          else
            rv = false;
        }
      else
        {
          fprintf(stderr, "%s: Failed to parse: %d: %s\n", filename, (int) e, json_tokener_error_desc(e));
        }
      free(json);
    }
  else
    {
      if(nonexistent_ok == false)
        fprintf(stderr, "%s: Failed to read '%s'.\n", __FUNCTION__, filename);
    }


  if(bind_defaults)
    ui_bindings_default(true);

  if(globals.iron_girl_mode)
    theme_set(THEME_TRAIT, "iron-girl");
  
  return rv;
}


static bool cb_bool(struct json_object * value, void * value_ptr)
{
  bool * ptr;

  assert(value != NULL);
  assert(value_ptr != NULL);

  ptr = value_ptr;
  
  if(json_object_get_boolean(value))
    *ptr = true;
  else
    *ptr = false;

  return true;
}


static bool cb_int(struct json_object * value, void * value_ptr)
{
  int * ptr;

  assert(value != NULL);
  assert(value_ptr != NULL);
  
  ptr = value_ptr;
  *ptr = json_object_get_int(value);

  return true;
}


static bool cb_opengl_pot(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;

  rv = cb_bool(value, &globals.opengl_power_of_two_textures);

  if(rv)
    globals.opengl_power_of_two_textures_benchmarked = true;

  return rv;
}


static bool cb_sound_volume(struct json_object * value, void * value_ptr DG_UNUSED)
{
  assert(value != NULL);

  globals.volume = json_object_get_int(value);

  if(globals.volume > 100)
    globals.volume = 100;

  return true;
}


static bool cb_cave_selection(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;
  char ** cave_names;
  const char * name;

  assert(value != NULL);

  rv = false;
  name = json_object_get_string(value);
  cave_names = get_cave_names(true);
  assert(cave_names != NULL);
  for(int i = 0; rv == false && cave_names[i] != NULL; i++)
    if(!strcmp(cave_names[i], name))
      {
        set_cave_selection(cave_names[i]);
        rv = true;
      }

  cave_names = free_cave_names(cave_names);

  if(rv == false)
    {
      fprintf(stderr, "Warning, ignoring CaveSelection '%s' because it does not exist.\n", name);
      rv = true; /* This is not a fatal error. */
    }
  

  return rv;
}


static bool cb_cave_selection_level(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;
  int n;

  n = -1;
  rv = cb_int(value, &n);
  if(rv)
    if(n >= 1)
      globals.level_selection = n;

  return rv;
}


static bool cb_game_mode(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;
  const char * modename;

  assert(value != NULL);

  rv = true;
  modename = json_object_get_string(value);

  if(!strcmp(modename, "classic"))
    globals.title_game_mode = GAME_MODE_CLASSIC;
  else if((traits_get_active() & TRAIT_ADVENTURE_MODE) && !strcmp(modename, "adventure"))
    globals.title_game_mode = GAME_MODE_ADVENTURE;
  else if((traits_get_active() & TRAIT_PYJAMA_PARTY) && !strcmp(modename, "pyjama_party"))
    globals.title_game_mode = GAME_MODE_PYJAMA_PARTY;
  else
    {
      rv = false;
      fprintf(stderr, "Incorrect game mode '%s' in settings.json.\n", modename);
    }

  return rv;
}



static bool cb_title_midarea(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;
  const char * id;

  assert(value != NULL);

  rv = true;
  id = json_object_get_string(value);

  if(!strcmp(id, "arcade"))
    globals.title_midarea = 0;
  else if(!strcmp(id, "quests"))
    globals.title_midarea = 1;
  else
    {
      rv = false;
      fprintf(stderr, "Incorrect id '%s' for TitleMidarea in settings.json.\n", id);
    }

  return rv;
}



static bool cb_locale(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;
  const char * ll;

  ll = json_object_get_string(value);
  rv = set_language(ll);
  if(rv == true)
    globals.locale_save = true;
  else
    fprintf(stderr, "Unsupported locale '%s' in settings.json.\n", ll);
  
  return rv;
}



static bool cb_themes(struct json_object * value, void * value_ptr DG_UNUSED)
{
  /* Example input:
   * "Themes": [
   *   { "Name": "easter", "Enabled": true },
   *   { "Name": "iron-girl", "Enabled": true },
   *   ...
   * ]
   */

  bool rv;
  struct array_list * array;

  rv = true;

  array = json_object_get_array(value);
  assert(array != NULL);
  if(array != NULL)
    {
      struct stack * themes;
      int array_len;

      themes = gc_get_stack(GCT_THEME);

      array_len = json_object_array_length(value);
      for(int i = 0; rv == true && i < array_len; i++)
        {
          struct json_object * row;

          row = json_object_array_get_idx(value, i);
          assert(row != NULL);
          if(row != NULL)
            {
              char * name;
              bool   enabled;

              name    = NULL;
              enabled = true;

              /* Go through row and find 'Name' and 'Enabled': */
              struct json_object_iterator it;
              struct json_object_iterator itEnd;

              it    = json_object_iter_begin(row);
              itEnd = json_object_iter_end(row);
              while(!json_object_iter_equal(&it, &itEnd))
                {
                  const char *         v_name;
                  struct json_object * v_value;
                  
                  v_name  = json_object_iter_peek_name(&it);
                  v_value = json_object_iter_peek_value(&it);
#ifndef NDEBUG
                  enum json_type       v_type;
                  v_type = json_object_get_type(v_value);
#endif

                  if(!strcmp(v_name, "Name"))
                    {
                      assert(v_type == json_type_string);
                      name = strdup(json_object_get_string(v_value));
                    }
                  else if(!strcmp(v_name, "Enabled"))
                    {
                      assert(v_type == json_type_boolean);
                      enabled = json_object_get_boolean(v_value);
                    }

                  json_object_iter_next(&it);
                }

              /* Utilize the found 'name' and 'enabled' to enable/disable the theme: */
              if(name != NULL)
                {
                  if(strcmp(name, "default"))
                    {
                      for(unsigned int j = 0; name != NULL && j < themes->size; j++)
                        {
                          struct theme * t;
                          
                          t = themes->data[j];
                          if(t != NULL)
                            if(!strcmp(t->name, name))
                              {
                                t->enabled = enabled;
                                free(name);
                                name = NULL;
                              }
                        }

                      if(name != NULL)
                        {
                          rv = false;
                          fprintf(stderr, "%s: Referenced unknown theme '%s'.\n", filename, name);
                        }
                    }
                  else
                    {
                      rv = false;
                      fprintf(stderr, "%s: Error: Tried to disable default theme.", filename);
                    }

                  free(name);
                }
              else
                {
                  rv = false;
                  fprintf(stderr, "%s: Could not find value for 'Name' in 'Themes' -array.\n", filename);
                }
            }
        }
    }

  return rv;
}



static bool cb_bindings(struct json_object * value, void * value_ptr DG_UNUSED)
{
  bool rv;
  struct array_list * array;

  rv = true;

  assert(value != NULL);
  array = json_object_get_array(value);
  assert(array != NULL);
  if(array != NULL)
    {
      int array_len;

      array_len = json_object_array_length(value);
      for(int i = 0; rv == true && i < array_len; i++)
        {
          struct json_object * row;

          row = json_object_array_get_idx(value, i);
          assert(row != NULL);
          if(row != NULL)
            {
              const int notset = 99999;
              char * b_type;
              enum UI_COMMAND b_command;
              char * b_key;
              char * b_modifiers;
              int    b_joystick;
              int    b_button;
              int    b_axis;
              int    b_value;

              b_type      = NULL;
              b_command   = UIC_SIZEOF_;
              b_key       = NULL;
              b_modifiers = NULL;
              b_joystick  = notset;
              b_button    = notset;
              b_axis      = notset;
              b_value     = notset;

              /* Go through row and find 'Type' and 'Command' (and then the rest of the data depending on the Type): */
              struct json_object_iterator it;
              struct json_object_iterator itEnd;

              it    = json_object_iter_begin(row);
              itEnd = json_object_iter_end(row);
              while(!json_object_iter_equal(&it, &itEnd))
                {
                  const char *         v_name;
                  struct json_object * v_value;
                  
                  v_name  = json_object_iter_peek_name(&it);
                  v_value = json_object_iter_peek_value(&it);
#ifndef NDEBUG
                  enum json_type       v_type;
                  v_type = json_object_get_type(v_value);
#endif

                  if(!strcmp(v_name, "Type"))
                    {
                      assert(v_type == json_type_string);
                      b_type = strdup(json_object_get_string(v_value));
                    }
                  else if(!strcmp(v_name, "Command"))
                    {
                      const char * c;

                      assert(v_type == json_type_string);
                      c = json_object_get_string(v_value);
                      for(int j = 0; b_command == UIC_SIZEOF_ && j < UIC_SIZEOF_; j++)
                        if(!strcmp(c, ui_command_name(j)))
                          b_command = j;

                      if(b_command == UIC_SIZEOF_)
                        {
                          fprintf(stderr, "%s: Unknown command '%s' in Bindings.\n", filename, c);
                          rv = false;
                        }
                    }
                  else if(!strcmp(v_name, "Key"))
                    {
                      assert(v_type == json_type_string);
                      b_key = strdup(json_object_get_string(v_value));
                    }
                  else if(!strcmp(v_name, "Modifiers"))
                    {
                      assert(v_type == json_type_string);
                      b_modifiers = strdup(json_object_get_string(v_value));
                    }
                  else if(!strcmp(v_name, "Joystick"))
                    {
                      assert(v_type == json_type_int);
                      b_joystick = json_object_get_int(v_value);
                    }
                  else if(!strcmp(v_name, "Button"))
                    {
                      assert(v_type == json_type_int);
                      b_button = json_object_get_int(v_value);
                    }
                  else if(!strcmp(v_name, "Axis"))
                    {
                      assert(v_type == json_type_int);
                      b_axis = json_object_get_int(v_value);
                    }
                  else if(!strcmp(v_name, "Value"))
                    {
                      assert(v_type == json_type_int);
                      b_value = json_object_get_int(v_value);
                    }

                  json_object_iter_next(&it);
                }

              /* Utilize the data found. */
              if(b_type != NULL)
                {
                  if(!strcmp(b_type, "key"))
                    {
                      SDL_Event event;

                      event.type = SDL_KEYDOWN;
                      event.key.keysym.sym = SDLK_UNKNOWN;
                      event.key.keysym.mod = 0;

                      assert(b_key != NULL);
                      for(SDLKey j = SDLK_FIRST; event.key.keysym.sym == SDLK_UNKNOWN && j < SDLK_LAST; j++)
                        if(!strcmp(b_key, ui_keyboard_key_name(j)))
                          event.key.keysym.sym = j;

                      if(b_modifiers != NULL)
                        {
                          if(strstr(b_modifiers, "Lctrl") != NULL)
                            event.key.keysym.mod |= KMOD_LCTRL;
                          if(strstr(b_modifiers, "Rctrl") != NULL)
                            event.key.keysym.mod |= KMOD_RCTRL;
                          if(strstr(b_modifiers, "Lshift") != NULL)
                            event.key.keysym.mod |= KMOD_LSHIFT;
                          if(strstr(b_modifiers, "Rshift") != NULL)
                            event.key.keysym.mod |= KMOD_RSHIFT;
                          if(strstr(b_modifiers, "Lalt") != NULL)
                            event.key.keysym.mod |= KMOD_LALT;
                          if(strstr(b_modifiers, "Ralt") != NULL)
                            event.key.keysym.mod |= KMOD_RALT;
                        }
                      
                      if(event.key.keysym.sym != SDLK_UNKNOWN)
                        ui_bind(b_command, &event);
                    }
                  else if(!strcmp(b_type, "joystick_button"))
                    {
                      assert(b_joystick != notset);
                      assert(b_button != notset);

                      SDL_Event event;

                      event.type = SDL_JOYBUTTONDOWN;
                      event.jbutton.which  = b_joystick;
                      event.jbutton.button = b_button;
                      ui_bind(b_command, &event);
                    }
                  else if(!strcmp(b_type, "joystick_axis"))
                    {
                      assert(b_joystick != notset);
                      assert(b_axis != notset);

                      SDL_Event event;

                      event.type = SDL_JOYAXISMOTION;
                      event.jaxis.which = b_joystick;
                      event.jaxis.axis  = b_axis;
                      event.jaxis.value = b_value;
                      ui_bind(b_command, &event);
                    }
                  else
                    {
                      fprintf(stderr, "%s: Unknown type '%s' in Bindings.\n", filename, b_type);
                      rv = false;
                    }
                }
              else
                {
                  fprintf(stderr, "%s: A binding has no type set in Bindings.\n", filename);
                  rv = false;
                }

              free(b_type);
              free(b_key);
              free(b_modifiers);
            }
        }
    }

  return rv;
}

