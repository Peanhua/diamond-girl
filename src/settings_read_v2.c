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
#include "globals.h"

#ifdef HAVE_LUA

#include "ui.h"
#include "traits.h"
#include "themes.h"
#include "cave.h"
#include "gc.h"
#include "stack.h"

#include <errno.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <assert.h>
#include <unistd.h>

/* Lua functions */
static int lf_config_fullscreen(lua_State * lstate);
static int lf_config_sound(lua_State * lstate);
static int lf_config_joysticks(lua_State * lstate);
static int lf_config_map_selection(lua_State * lstate); /* deprecated */
static int lf_config_cave_selection(lua_State * lstate);
static int lf_config_opengl(lua_State * lstate);
static int lf_config_bind_key(lua_State * lstate);
static int lf_config_bind_joystick_axis(lua_State * lstate);
static int lf_config_bind_joystick_button(lua_State * lstate);
static int lf_config_show_fps(lua_State * lstate);
static int lf_config_write_settings(lua_State * lstate);
static int lf_enable_theme(lua_State * lstate);
static int lf_disable_theme(lua_State * lstate);
static int lf_config_game_mode(lua_State * lstate);
static int lf_config_map_tilting(lua_State * lstate);
static int lf_config_smooth_classic_mode(lua_State * lstate);


static bool fasterslower_bound;

bool settings_read_v2(void)
{
  bool rv;

  rv = false;
  fasterslower_bound = false;
  
  if(access(get_save_filename("settings.lua"), R_OK) == 0)
    {
      lua_State * lstate;

      lstate = luaL_newstate();
      
      luaL_openlibs(lstate);
      
      lua_register(lstate, "configFullscreen",         lf_config_fullscreen);
      lua_register(lstate, "configSound",              lf_config_sound);
      lua_register(lstate, "configJoysticks",          lf_config_joysticks);
      lua_register(lstate, "configMapSelection",       lf_config_map_selection); /* deprecated */
      lua_register(lstate, "configCaveSelection",      lf_config_cave_selection);
      lua_register(lstate, "configOpenGL",             lf_config_opengl);
      lua_register(lstate, "configBindKey",            lf_config_bind_key);
      lua_register(lstate, "configBindJoystickAxis",   lf_config_bind_joystick_axis);
      lua_register(lstate, "configBindJoystickButton", lf_config_bind_joystick_button);
      lua_register(lstate, "configShowFPS",            lf_config_show_fps);
      lua_register(lstate, "configWriteSettings",      lf_config_write_settings);
      lua_register(lstate, "configGameMode",           lf_config_game_mode);
      lua_register(lstate, "configMapTilting",         lf_config_map_tilting);
      lua_register(lstate, "configSmoothClassicMode",  lf_config_smooth_classic_mode);
      lua_register(lstate, "enableTheme",              lf_enable_theme);
      lua_register(lstate, "disableTheme",             lf_disable_theme);
      
      if(luaL_dofile(lstate, get_save_filename("settings.lua")) == 0)
        { /* Successfully loaded. */
          rv = true;

          if(fasterslower_bound == false)
            {
              SDL_Event e;
              
              /* Add faster/slower keybindings if they were not bound in the settings.lua.
               * This is done because the trait "time_control" utilizes these two bindings,
               * and the default prior the trait was not to have anything bound on to these.
               */
              
              e.type           = SDL_KEYDOWN;
              e.key.keysym.sym = SDLK_PAGEUP;
              e.key.keysym.mod = KMOD_NONE;
              ui_bind(UIC_FASTER, &e);
              
              e.type           = SDL_KEYDOWN;
              e.key.keysym.sym = SDLK_PAGEDOWN;
              e.key.keysym.mod = KMOD_NONE;
              ui_bind(UIC_SLOWER, &e);
            }
        }
      else
        {
          fprintf(stderr, "Failed to load '%s': %s\n", get_save_filename("settings.lua"), lua_tostring(lstate, -1));
          lua_pop(lstate, 1);
        }

      lua_close(lstate);

      /* Remove the old file, we will be writing in different name when we save. */
      unlink(get_save_filename("settings.lua"));
    }

  return rv;
}


static int lf_config_fullscreen(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.fullscreen = lua_toboolean(lstate, 1) ? true : false;
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_sound(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.use_sfx   = lua_toboolean(lstate, 1);
          globals.use_music = lua_toboolean(lstate, 1);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else if(argc == 3 || argc == 4)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.use_sfx = lua_toboolean(lstate, 1);
          if(lua_isboolean(lstate, 2))
            {
              globals.use_music = lua_toboolean(lstate, 2);
              if(lua_isnumber(lstate, 3))
                {
                  globals.max_channels = lua_tonumber(lstate, 3);
                  if(argc == 4)
                    {
                      if(lua_isnumber(lstate, 4))
                        {
                          globals.volume = lua_tonumber(lstate, 4);
                          if(globals.volume > 100)
                            globals.volume = 100;
                        }
                      else
                        {
                          lua_pushstring(lstate, "Expected argument #4 to be a number, got something else.");
                          lua_error(lstate);
                        }
                    }
                }
              else
                {
                  lua_pushstring(lstate, "Expected argument #3 to be a number, got something else.");
                  lua_error(lstate);
                }
            }
          else
            {
              lua_pushstring(lstate, "Expected argument #2 to be a boolean, got something else.");
              lua_error(lstate);
            }
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_joysticks(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.use_joysticks = lua_toboolean(lstate, 1);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_map_selection(lua_State * lstate)
{ /* deprecated */
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1 || argc == 2)
    {
      if(lua_isnumber(lstate, 1))
        {
          int n;
          char ** cave_names;
          bool ok;

          n = lua_tonumber(lstate, 1);
          if(n == -1)
            n = 0;

          cave_names = get_cave_names(true);
          assert(cave_names != NULL);
          ok = false;
          for(int i = 0; ok == false && cave_names[i] != NULL; i++)
            if(n == i)
              ok = true;

          if(ok == true)
            {
              set_cave_selection(cave_names[n]);
              if(argc == 2)
                {
                  if(lua_isnumber(lstate, 2))
                    {
                      n = lua_tonumber(lstate, 2);
                      if(n >= 1)
                        globals.level_selection = n;
                    }
                  else
                    {
                      lua_pushstring(lstate, "Expected argument #2 to be a number, got something else.");
                      lua_error(lstate);
                    }
                }
            }
          else
            {
              lua_pushstring(lstate, "Failed to find the cave specified.");
              lua_error(lstate);
            }
          cave_names = free_cave_names(cave_names);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a number, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_cave_selection(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1 || argc == 2)
    {
      if(lua_isstring(lstate, 1))
        {
          const char * tmp;
          
          tmp = lua_tostring(lstate, 1);
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              char ** cave_names;
              bool found;

              cave_names = get_cave_names(false);
              assert(cave_names != NULL);
              found = false;
              for(int i = 0; found == false && cave_names[i] != NULL; i++)
                if(!strcmp(cave_names[i], tmp))
                  found = true;

              assert(found == true);

              if(found == true)
                {
                  set_cave_selection(tmp);

                  if(argc == 2)
                    {
                      if(lua_isnumber(lstate, 2))
                        {
                          int n;
                          
                          n = lua_tonumber(lstate, 2);
                          if(n >= 1)
                            globals.level_selection = n;
                        }
                      else
                        {
                          lua_pushstring(lstate, "Expected argument #2 to be a number, got something else.");
                          lua_error(lstate);
                        }
                    }
                }
              else
                {
                  lua_pushstring(lstate, "Failed to find the given cave.");
                  lua_error(lstate);
                }

              cave_names = free_cave_names(cave_names);
            }
          else
            {
              lua_pushstring(lstate, "Failed to get string from argument #1.");
              lua_error(lstate);
            }
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a string, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_opengl(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 2 || argc == 3)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.opengl = lua_toboolean(lstate, 1) ? true : false;
          if(lua_isboolean(lstate, 2))
            {
              globals.opengl_power_of_two_textures             = lua_toboolean(lstate, 2);
              globals.opengl_power_of_two_textures_benchmarked = true;

              if(argc == 3)
                {
                  if(lua_isboolean(lstate, 3))
                    {
                      globals.opengl_compressed_textures = lua_toboolean(lstate, 3);
                    }
                  else
                    {
                      lua_pushstring(lstate, "Expected argument #3 to be a boolean, got something else.");
                      lua_error(lstate);
                    }
                }
            }
          else
            {
              lua_pushstring(lstate, "Expected argument #2 to be a boolean, got something else.");
              lua_error(lstate);
            }
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}

static enum UI_COMMAND get_uic(const char * name)
{
  enum UI_COMMAND uic;

  uic = UIC_SIZEOF_;
  for(int i = 0; uic == UIC_SIZEOF_ && i < UIC_SIZEOF_; i++)
    if(!strcmp(name, ui_command_name(i)))
      uic = i;

  return uic;
}

static int lf_config_bind_key(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 3)
    {
      const char * uic_name;
      enum UI_COMMAND uic;

      uic_name = luaL_optstring(lstate, 1, NULL);
      uic = get_uic(uic_name);
      if(uic < UIC_SIZEOF_)
        {
          SDL_Event event;
          const char * key_name;

          event.type = SDL_KEYDOWN;

          key_name = luaL_optstring(lstate, 2, NULL);
          event.key.keysym.sym = SDLK_UNKNOWN;
          for(SDLKey i = SDLK_FIRST; event.key.keysym.sym == SDLK_UNKNOWN && i < SDLK_LAST; i++)
            {
              if(!strcmp(key_name, ui_keyboard_key_name(i)))
                event.key.keysym.sym = i;
            }
          if(event.key.keysym.sym != SDLK_UNKNOWN)
            {
              const char * modifiers_list;

              event.key.keysym.mod = 0;
              modifiers_list = luaL_optstring(lstate, 3, NULL);
              if(strstr(modifiers_list, "Lctrl") != NULL)
                event.key.keysym.mod |= KMOD_LCTRL;
              if(strstr(modifiers_list, "Rctrl") != NULL)
                event.key.keysym.mod |= KMOD_RCTRL;
              if(strstr(modifiers_list, "Lshift") != NULL)
                event.key.keysym.mod |= KMOD_LSHIFT;
              if(strstr(modifiers_list, "Rshift") != NULL)
                event.key.keysym.mod |= KMOD_RSHIFT;
              if(strstr(modifiers_list, "Lalt") != NULL)
                event.key.keysym.mod |= KMOD_LALT;
              if(strstr(modifiers_list, "Ralt") != NULL)
                event.key.keysym.mod |= KMOD_RALT;

              ui_bind(uic, &event);

              if(uic == UIC_FASTER || uic == UIC_SLOWER)
                fasterslower_bound = true;
            }
          else
            {
              lua_pushstring(lstate, "Unknown key given in argument #2.");
              lua_error(lstate);
            }
        }
      else
        {
          lua_pushstring(lstate, "Unknown command given in argument #1.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}

static int lf_config_bind_joystick_axis(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 4)
    {
      const char * uic_name;
      enum UI_COMMAND uic;

      uic_name = luaL_optstring(lstate, 1, NULL);
      uic = get_uic(uic_name);
      if(uic < UIC_SIZEOF_)
        {
          SDL_Event event;

          event.type = SDL_JOYAXISMOTION;

          event.jaxis.which = luaL_optint(lstate, 2, 0);
          event.jaxis.axis  = luaL_optint(lstate, 3, 0);
          event.jaxis.value = luaL_optint(lstate, 4, 0);
          ui_bind(uic, &event);

          if(uic == UIC_FASTER || uic == UIC_SLOWER)
            fasterslower_bound = true;
        }
      else
        {
          lua_pushstring(lstate, "Unknown command given in argument #1.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }
  
  return 0;
}

static int lf_config_bind_joystick_button(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 3)
    {
      const char * uic_name;
      enum UI_COMMAND uic;

      uic_name = luaL_optstring(lstate, 1, NULL);
      uic = get_uic(uic_name);
      if(uic < UIC_SIZEOF_)
        {
          SDL_Event event;

          event.type = SDL_JOYBUTTONDOWN;

          event.jbutton.which  = luaL_optint(lstate, 2, 0);
          event.jbutton.button = luaL_optint(lstate, 3, 0);
          ui_bind(uic, &event);

          if(uic == UIC_FASTER || uic == UIC_SLOWER)
            fasterslower_bound = true;
        }
      else
        {
          lua_pushstring(lstate, "Unknown command given in argument #1.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }
  
  return 0;
}


static int lf_config_show_fps(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.fps_counter_active = lua_toboolean(lstate, 1);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_write_settings(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.write_settings = lua_toboolean(lstate, 1);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}



static int lf_config_game_mode(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1 || argc == 2)
    {
      const char * modename;

      modename = luaL_optstring(lstate, 1, NULL);
      if(!strcmp(modename, "classic"))
        globals.title_game_mode = GAME_MODE_CLASSIC;
      else if((traits_get_active() & TRAIT_ADVENTURE_MODE) && !strcmp(modename, "adventure"))
        globals.title_game_mode = GAME_MODE_ADVENTURE;
      else if((traits_get_active() & TRAIT_PYJAMA_PARTY) && !strcmp(modename, "pyjama_party"))
        globals.title_game_mode = GAME_MODE_PYJAMA_PARTY;
      else
        {
          lua_pushstring(lstate, "Incorrect argument #1 for configGameMode().");
          lua_error(lstate);
        }

      if(argc == 2)
        {
          if(lua_isboolean(lstate, 2))
            {
              globals.iron_girl_mode = lua_toboolean(lstate, 2);
            }
          else
            {
              lua_pushstring(lstate, "Expected argument #2 to be a boolean, got something else.");
              lua_error(lstate);
            }
        }
    }
  else
    {
      lua_pushstring(lstate, "Incorrect number of arguments.");
      lua_error(lstate);
    }

  return 0;
}


static int lf_config_map_tilting(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.map_tilting = lua_toboolean(lstate, 1);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }

  return 0;
}


static int lf_disable_theme(lua_State * lstate)
{
  int argc;
  struct stack * themes;

  themes = gc_get_stack(GCT_THEME);
  
  argc = lua_gettop(lstate);
  if(argc == 0)
    { /* Disable all. */
      for(unsigned int i = 0; i < themes->size; i++)
        {
          struct theme * t;

          t = themes->data[i];
          if(t != NULL)
            if(strcmp(t->name, "default"))
              t->enabled = false;
        }
    }
  else if(argc > 0)
    { /* Disable one or more. */
      for(int i = 0; i < argc; i++)
        {
          const char * theme;

          theme = luaL_optstring(lstate, 1 + i, NULL);
          if(strcmp(theme, "default"))
            {
              for(unsigned int j = 0; theme != NULL && j < themes->size; j++)
                {
                  struct theme * t;

                  t = themes->data[j];
                  if(t != NULL)
                    if(!strcmp(t->name, theme))
                      {
                        t->enabled = false;
                        theme = NULL;
                      }
                }
            }
          else
            {
              lua_pushstring(lstate, "Tried to disable default theme.");
              lua_error(lstate);
            }
        }
    }
  
  return 0;
}


static int lf_enable_theme(lua_State * lstate)
{
  int argc;
  struct stack * themes;

  themes = gc_get_stack(GCT_THEME);
  
  argc = lua_gettop(lstate);
  if(argc == 0)
    { /* Enable all. */
      for(unsigned int i = 0; i < themes->size; i++)
        {
          struct theme * t;

          t = themes->data[i];
          if(t != NULL)
            t->enabled = true;
        }
    }
  else if(argc > 0)
    { /* Enable one or more. */
      for(int i = 0; i < argc; i++)
        {
          const char * theme;

          theme = luaL_optstring(lstate, 1 + i, NULL);
          for(unsigned int j = 0; theme != NULL && j < themes->size; j++)
            {
              struct theme * t;
              
              t = themes->data[i];
              if(t != NULL)
                if(!strcmp(t->name, theme))
                  {
                    t->enabled = true;
                    theme = NULL;
                  }
            }
        }
    }

  return 0;
}


static int lf_config_smooth_classic_mode(lua_State * lstate)
{
  int argc;

  argc = lua_gettop(lstate);
  if(argc == 1)
    {
      if(lua_isboolean(lstate, 1))
        {
          globals.smooth_classic_mode = lua_toboolean(lstate, 1);
        }
      else
        {
          lua_pushstring(lstate, "Expected argument #1 to be a boolean, got something else.");
          lua_error(lstate);
        }
    }

  return 0;
}



#else

/* !HAVE_LUA */
/* Dummy function without LUA: */
bool settings_read_v2(void)
{
  return false;
}


#endif
