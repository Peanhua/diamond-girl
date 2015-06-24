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

#include "diamond_girl.h"
#include "ui.h"
#include "widget.h"
#include "globals.h"
#include "gfx.h"
#include "image.h"
#include "sfx.h"
#include "stack.h"
#include "traits.h"
#include "themes.h"
#include "gc.h"
#include "widget_factory.h"

#include <assert.h>
#include <errno.h>
#include <libintl.h>
#include <string.h>

static void on_settings_exit(bool pressed, SDL_Event * event);

static void on_window_unload(struct widget * this);
static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_select_controller(struct widget * this);
static void on_delete_binding(struct widget * this, enum WIDGET_BUTTON button);
static void on_add_binding(struct widget * this, enum WIDGET_BUTTON button);
static void on_cancel_binding(struct widget * this);
static void on_binding_execute(bool pressed, SDL_Event * event);


static struct widget * obj_window;
static struct widget * obj_opengl;
static struct widget * obj_fullscreen;
static struct widget * obj_special_days;
static struct widget * obj_game_mode;
static struct widget * obj_iron_girl_mode;
static struct widget * obj_sfx;
static struct widget * obj_music;
static struct widget * obj_volume;
static struct widget * obj_language;
static struct widget * obj_controls[6];
static struct widget * obj_controllers;
static struct ui_binding ** bindings;
static struct widget * obj_binding_window;
static int             binding_target;

static void (*on_settings_changed)(bool gfx_restart, bool sfx_restart, bool new_opengl) = NULL;


enum ACTION { ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_MANIPULATE, ACTION_ALT_MANIPULATE, ACTION_SIZEOF_ };

void settings(void (*settings_changed_cb)(bool gfx_restart, bool sfx_restart, bool new_opengl))
{
  struct widget_factory_data wfd[] = 
    {
      { WFDT_ON_RELEASE, "on_window_close_clicked", on_window_close_clicked },
      { WFDT_SIZEOF_,    NULL,                      NULL                    }
    };

  assert(on_settings_changed == NULL);
  on_settings_changed = settings_changed_cb;
  
  obj_window = widget_factory(wfd, NULL, "settings");
  assert(obj_window != NULL);
  widget_set_on_unload(obj_window, on_window_unload);

  int x, y;
  int rowheight;
  struct stack * controllers;

  obj_controllers = widget_find(obj_window, "controllers");
  controllers = stack_new();
  assert(controllers != NULL);
  if(controllers != NULL)
    {
      struct ui_widget_select_option * o;

      o = widget_new_select_option((void *) 0, gettext("Keyboard"));
      stack_push(controllers, o);

      o = widget_new_select_option((void *) 1, gettext("Mouse"));
      stack_push(controllers, o);
      
      for(int i = 0; i < globals.joysticks_count; i++)
        {
          char buf[128];
          
          snprintf(buf, sizeof buf, gettext("Joystick#%d:%s"), i + 1, SDL_JoystickName(i));
          o = widget_new_select_option((void *) (ptrdiff_t) (i + 2), buf);
          stack_push(controllers, o);
        }

      widget_set_select_options(obj_controllers, controllers);
      
      widget_set_pointer(obj_controllers, "on_select", on_select_controller);
    }

  x = 10;
  y = 62;


  /* row 1 */
  struct widget * obj_delete_up, * obj_add_up;

  obj_controls[ACTION_UP] = widget_new_select(obj_window, x + 110, y, 120, 0, NULL);
  widget_set_ulong(obj_controls[ACTION_UP], "control", ACTION_UP);
  rowheight = widget_height(obj_controls[ACTION_UP]) + 2;
  widget_set_navigation_updown(obj_controllers, obj_controls[ACTION_UP]);

  obj_delete_up = widget_new_button(obj_window, x + 110 + 120, y, "-");
  widget_set_width(obj_delete_up, widget_height(obj_delete_up));
  widget_set_ulong(obj_delete_up, "control", ACTION_UP);
  widget_set_on_release(obj_delete_up, on_delete_binding);
  widget_set_navigation_leftright(obj_controls[ACTION_UP], obj_delete_up);
  widget_set_navigation_up(obj_delete_up, obj_controllers);

  obj_add_up = widget_new_button(obj_window, x + 110 + 120 + widget_width(obj_delete_up), y, "+");
  widget_set_width(obj_add_up, widget_height(obj_add_up));
  widget_set_ulong(obj_add_up, "control", ACTION_UP);
  widget_set_on_release(obj_add_up, on_add_binding);
  widget_set_navigation_leftright(obj_delete_up, obj_add_up);
  widget_set_navigation_up(obj_add_up, obj_controllers);

  obj_sfx = widget_new_checkbox(obj_window, 500, y, globals.use_sfx);
  widget_set_navigation_leftright(obj_add_up, obj_sfx);

  y += rowheight;

  /* row 2 */
  struct widget * obj_delete_left, * obj_add_left;

  obj_controls[ACTION_LEFT] = widget_new_select(obj_window, x + 110, y, 120, 0, NULL);
  widget_set_ulong(obj_controls[ACTION_LEFT], "control", ACTION_LEFT);
  widget_set_navigation_updown(obj_controls[ACTION_UP], obj_controls[ACTION_LEFT]);

  obj_delete_left = widget_new_button(obj_window, x + 110 + 120, y, "-");
  widget_set_width(obj_delete_left, widget_height(obj_delete_left));
  widget_set_ulong(obj_delete_left, "control", ACTION_LEFT);
  widget_set_on_release(obj_delete_left, on_delete_binding);
  widget_set_navigation_leftright(obj_controls[ACTION_LEFT], obj_delete_left);
  widget_set_navigation_updown(obj_delete_up, obj_delete_left);

  obj_add_left = widget_new_button(obj_window, x + 110 + 120 + widget_width(obj_delete_left), y, "+");
  widget_set_width(obj_add_left, widget_height(obj_add_left));
  widget_set_ulong(obj_add_left, "control", ACTION_LEFT);
  widget_set_on_release(obj_add_left, on_add_binding);
  widget_set_navigation_leftright(obj_delete_left, obj_add_left);
  widget_set_navigation_updown(obj_add_up, obj_add_left);

  obj_music = widget_new_checkbox(obj_window, 500, y, globals.use_music);
  widget_set_navigation_leftright(obj_add_left, obj_music);
  widget_set_navigation_updown(obj_sfx, obj_music);

  y += rowheight;

  /* row 3 */
  struct widget * obj_delete_right, * obj_add_right;

  obj_controls[ACTION_RIGHT] = widget_new_select(obj_window, x + 110, y, 120, 0, NULL);
  widget_set_ulong(obj_controls[ACTION_RIGHT], "control", ACTION_RIGHT);
  widget_set_navigation_updown(obj_controls[ACTION_LEFT], obj_controls[ACTION_RIGHT]);

  obj_delete_right = widget_new_button(obj_window, x + 110 + 120, y, "-");
  widget_set_width(obj_delete_right, widget_height(obj_delete_right));
  widget_set_ulong(obj_delete_right, "control", ACTION_RIGHT);
  widget_set_on_release(obj_delete_right, on_delete_binding);
  widget_set_navigation_leftright(obj_controls[ACTION_RIGHT], obj_delete_right);
  widget_set_navigation_updown(obj_delete_left, obj_delete_right);

  obj_add_right = widget_new_button(obj_window, x + 110 + 120 + widget_width(obj_delete_right), y, "+");
  widget_set_width(obj_add_right, widget_height(obj_add_right));
  widget_set_ulong(obj_add_right, "control", ACTION_RIGHT);
  widget_set_on_release(obj_add_right, on_add_binding);
  widget_set_navigation_leftright(obj_delete_right, obj_add_right);
  widget_set_navigation_updown(obj_add_left, obj_add_right);

  //  widget_new_text(obj_window, 310, y, "Volume");
  obj_volume = widget_new_slider(obj_window, 450, y, 140, globals.volume, 0, 100);
  widget_set_string(obj_volume, "id", "slider_volume");
  widget_set_navigation_leftright(obj_add_right, obj_volume);
  widget_set_navigation_updown(obj_music, obj_volume);

  y += rowheight;

  /* row 4 */
  struct widget * obj_delete_down, * obj_add_down;

  obj_controls[ACTION_DOWN] = widget_new_select(obj_window, x + 110, y, 120, 0, NULL);
  widget_set_ulong(obj_controls[ACTION_DOWN], "control", ACTION_DOWN);
  widget_set_navigation_updown(obj_controls[ACTION_RIGHT], obj_controls[ACTION_DOWN]);

  obj_delete_down = widget_new_button(obj_window, x + 110 + 120, y, "-");
  widget_set_width(obj_delete_down, widget_height(obj_delete_down));
  widget_set_ulong(obj_delete_down, "control", ACTION_DOWN);
  widget_set_on_release(obj_delete_down, on_delete_binding);
  widget_set_navigation_leftright(obj_controls[ACTION_DOWN], obj_delete_down);
  widget_set_navigation_updown(obj_delete_right, obj_delete_down);

  obj_add_down = widget_new_button(obj_window, x + 110 + 120 + widget_width(obj_delete_down), y, "+");
  widget_set_width(obj_add_down, widget_height(obj_add_down));
  widget_set_ulong(obj_add_down, "control", ACTION_DOWN);
  widget_set_on_release(obj_add_down, on_add_binding);
  widget_set_navigation_leftright(obj_delete_down, obj_add_down);
  widget_set_navigation_updown(obj_add_right, obj_add_down);

  obj_opengl = widget_new_checkbox(obj_window, 500, y, globals.opengl);
  widget_set_navigation_leftright(obj_add_down, obj_opengl);
  widget_set_navigation_updown(obj_volume, obj_opengl);
#ifndef WITH_OPENGL
  widget_set_enabled(obj_opengl, false);
#endif

  y += rowheight;

  /* row 5 */
  struct widget * obj_delete_none, * obj_add_none;

  obj_controls[ACTION_MANIPULATE] = widget_new_select(obj_window, x + 110, y, 120, 0, NULL);
  widget_set_ulong(obj_controls[ACTION_MANIPULATE], "control", ACTION_MANIPULATE);
  widget_set_navigation_updown(obj_controls[ACTION_DOWN], obj_controls[ACTION_MANIPULATE]);

  obj_delete_none = widget_new_button(obj_window, x + 110 + 120, y, "-");
  widget_set_width(obj_delete_none, widget_height(obj_delete_none));
  widget_set_ulong(obj_delete_none, "control", ACTION_MANIPULATE);
  widget_set_on_release(obj_delete_none, on_delete_binding);
  widget_set_navigation_leftright(obj_controls[ACTION_MANIPULATE], obj_delete_none);
  widget_set_navigation_updown(obj_delete_down, obj_delete_none);

  obj_add_none = widget_new_button(obj_window, x + 110 + 120 + widget_width(obj_delete_none), y, "+");
  widget_set_width(obj_add_none, widget_height(obj_add_none));
  widget_set_ulong(obj_add_none, "control", ACTION_MANIPULATE);
  widget_set_on_release(obj_add_none, on_add_binding);
  widget_set_navigation_leftright(obj_delete_none, obj_add_none);
  widget_set_navigation_updown(obj_add_down, obj_add_none);

  obj_fullscreen = widget_new_checkbox(obj_window, 500, y, globals.fullscreen);
  widget_set_navigation_leftright(obj_add_none, obj_fullscreen);
  widget_set_navigation_updown(obj_opengl, obj_fullscreen);

  y += rowheight;


  /* row 6 */
  struct widget * obj_delete_alt, * obj_add_alt;

  obj_controls[ACTION_ALT_MANIPULATE] = widget_new_select(obj_window, x + 110, y, 120, 0, NULL);
  widget_set_ulong(obj_controls[ACTION_ALT_MANIPULATE], "control", ACTION_ALT_MANIPULATE);
  widget_set_navigation_updown(obj_controls[ACTION_MANIPULATE], obj_controls[ACTION_ALT_MANIPULATE]);

  obj_delete_alt = widget_new_button(obj_window, x + 110 + 120, y, "-");
  widget_set_width(obj_delete_alt, widget_height(obj_delete_alt));
  widget_set_ulong(obj_delete_alt, "control", ACTION_ALT_MANIPULATE);
  widget_set_on_release(obj_delete_alt, on_delete_binding);
  widget_set_navigation_leftright(obj_controls[ACTION_ALT_MANIPULATE], obj_delete_alt);
  widget_set_navigation_updown(obj_delete_none, obj_delete_alt);

  obj_add_alt = widget_new_button(obj_window, x + 110 + 120 + widget_width(obj_delete_alt), y, "+");
  widget_set_width(obj_add_alt, widget_height(obj_add_alt));
  widget_set_ulong(obj_add_alt, "control", ACTION_ALT_MANIPULATE);
  widget_set_on_release(obj_add_alt, on_add_binding);
  widget_set_navigation_leftright(obj_delete_alt, obj_add_alt);
  widget_set_navigation_updown(obj_add_none, obj_add_alt);

  {
    bool have_enabled_themes;
    struct stack * themes;
    
    have_enabled_themes = false;
    themes = gc_get_stack(GCT_THEME);
    for(unsigned int i = 0; have_enabled_themes == false && i < themes->size; i++)
      {
        struct theme * t;

        t = themes->data[i];
        if(t != NULL)
          if(t->enabled == true)
            have_enabled_themes = true;
      }

    obj_special_days = widget_new_checkbox(obj_window, 500, y, have_enabled_themes);
    widget_set_navigation_leftright(obj_add_alt, obj_special_days);
    widget_set_navigation_updown(obj_fullscreen, obj_special_days);
  }

  y += rowheight;

  /* row 7 */
  struct stack * languages;

  obj_language = NULL;
  languages = stack_new();
  if(languages != NULL)
    {
      struct ui_widget_select_option * o;
      int ind;

      o = widget_new_select_option("en_US", gettext("English"));
      stack_push(languages, o);

      o = widget_new_select_option("fi_FI", gettext("Finnish"));
      stack_push(languages, o);

      if(!strcmp(globals.language, "fi"))
        ind = 1;
      else
        ind = 0;
      
      widget_new_text(obj_window, 310, y, gettext("Language"));
      obj_language = widget_new_select(obj_window, 450, y, 140, ind, languages);
      widget_set_navigation_updown(obj_special_days, obj_language);
    }

  y += rowheight;
  
  
  /* row 8 */
  obj_game_mode = NULL;
  if(traits_get_available() & (TRAIT_ADVENTURE_MODE | TRAIT_PYJAMA_PARTY))
    {
      struct stack * s;

      widget_new_text(obj_window, 310, y, gettext("Game mode"));

      s = stack_new();
      assert(s != NULL);
      if(s != NULL)
        {
          struct
          {
            enum GAME_MODE mode;
            char *         name;
            trait_t        trait;
          } modes[3] =
              {
                { GAME_MODE_CLASSIC,      NULL, 0                    },
                { GAME_MODE_ADVENTURE,    NULL, TRAIT_ADVENTURE_MODE },
                { GAME_MODE_PYJAMA_PARTY, NULL, TRAIT_PYJAMA_PARTY   }
              };
          int ind, tmp;

          modes[0].name = gettext("Classic");
          modes[1].name = gettext("Adventure");
          modes[2].name = gettext("Pyjama Party");

          ind = 0;
          tmp = 0;
          for(int i = 0; i < 3; i++)
            if(modes[i].trait == 0 || (traits_get_active() & modes[i].trait))
              {
                struct ui_widget_select_option * o;

                o = widget_new_select_option((void *) 0, modes[i].name);
                stack_push(s, o);

                if(globals.title_game_mode == modes[i].mode)
                  ind = tmp;
                tmp++;
              }

          obj_game_mode = widget_new_select(obj_window, 450, y, 140, ind, s);
          widget_set_navigation_updown(obj_language, obj_game_mode);
        }
    }
  
  y += rowheight;

  /* row 9 */
  obj_iron_girl_mode = NULL;
  if(traits_get_active() & TRAIT_IRON_GIRL)
    {
      widget_new_text(obj_window, 310, y, gettext("Iron Girl mode"));
      obj_iron_girl_mode = widget_new_checkbox(obj_window, 500, y, globals.iron_girl_mode);
      if(obj_game_mode != NULL)
        widget_set_navigation_updown(obj_game_mode, obj_iron_girl_mode);
      else
        widget_set_navigation_updown(obj_fullscreen, obj_iron_girl_mode);
    }

  y += rowheight;


  struct widget * obj_close;

  y += 20;
  obj_close = widget_find(obj_window, "close");
  widget_set_x(obj_close, (widget_width(obj_window) - widget_width(obj_close)) / 2);
  widget_set_on_release(obj_close, on_window_close_clicked);
  widget_set_focus(obj_close);

  widget_set_navigation_updown(obj_controls[ACTION_ALT_MANIPULATE], obj_close);
  widget_set_navigation_down(obj_delete_alt,                obj_close);
  widget_set_navigation_down(obj_add_alt,                   obj_close);
  if(obj_iron_girl_mode != NULL)
    widget_set_navigation_down(obj_iron_girl_mode, obj_close);
  else if(obj_game_mode != NULL)
    widget_set_navigation_down(obj_game_mode, obj_close);
  else
    widget_set_navigation_down(obj_language, obj_close);

  widget_set_height(obj_window, y + widget_height(obj_close) + 10);
  widget_set_y(obj_window, (SCREEN_HEIGHT - widget_height(obj_window)) / 2);


  /* Replace cancel and exit handlers. */
  ui_push_handlers();
  ui_set_handler(UIC_EXIT,   on_settings_exit);
  ui_set_handler(UIC_CANCEL, on_settings_exit);

  bindings = ui_get_bindings(0);
  if(obj_controllers != NULL)
    on_select_controller(obj_controllers);
}


static void on_settings_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    widget_delete(obj_window);
}


static void on_select_controller(struct widget * this)
{
  int cind;
  struct stack * options[6];

  for(int i = 0; i < ACTION_SIZEOF_; i++)
    options[i] = stack_new();

  cind = widget_get_ulong(this, "selected_index");
  bindings = ui_get_bindings(cind);

  if(bindings != NULL)
    {
      for(int i = 0; bindings[i] != NULL; i++)
        if(cind == 0)
          {
            struct ui_widget_select_option * o;

            o = widget_new_select_option(bindings[i], SDL_GetKeyName(bindings[i]->event.key.keysym.sym));
            if(o != NULL)
              {
                switch(bindings[i]->command)
                  {
                  case UIC_UP:             stack_push(options[ACTION_UP],             o); break;
                  case UIC_DOWN:           stack_push(options[ACTION_DOWN],           o); break;
                  case UIC_LEFT:           stack_push(options[ACTION_LEFT],           o); break;
                  case UIC_RIGHT:          stack_push(options[ACTION_RIGHT],          o); break;
                  case UIC_MANIPULATE:     stack_push(options[ACTION_MANIPULATE],     o); break;
                  case UIC_ALT_MANIPULATE: stack_push(options[ACTION_ALT_MANIPULATE], o); break;
                  default: break;
                  }
              }
            else
              fprintf(stderr, "%s:%s(): Failed to allocate memory: %s\n", __FILE__, __FUNCTION__, strerror(errno));
          }
        else if(cind == 1)
          ;
        else
          {
            struct stack * s;
            struct ui_widget_select_option * o;

            o = widget_new_select_option(bindings[i], "");
            assert(o != NULL);
            if(o != NULL)
              {
                switch(bindings[i]->command)
                  {
                  case UIC_UP:
                  case UIC_DOWN:
                  case UIC_LEFT:
                  case UIC_RIGHT:
                    snprintf(o->text, sizeof o->text, gettext("axis %d%c"), bindings[i]->event.jaxis.axis, bindings[i]->event.jaxis.value < 0 ? '-' : '+');
                    break;
                  case UIC_MANIPULATE:
                  case UIC_ALT_MANIPULATE:
                    snprintf(o->text, sizeof o->text, gettext("butn %d"),   bindings[i]->event.jbutton.button);
                    break;
                  default:
                    break;
                  }
                switch(bindings[i]->command)
                  {
                  case UIC_UP:             s = options[ACTION_UP];             break;
                  case UIC_DOWN:           s = options[ACTION_DOWN];           break;
                  case UIC_LEFT:           s = options[ACTION_LEFT];           break;
                  case UIC_RIGHT:          s = options[ACTION_RIGHT];          break;
                  case UIC_MANIPULATE:     s = options[ACTION_MANIPULATE];     break;
                  case UIC_ALT_MANIPULATE: s = options[ACTION_ALT_MANIPULATE]; break;
                  default:                 s = NULL;                           break;
                  }
                if(s != NULL)
                  {
                    o->data = bindings[i];
                    stack_push(s, o);
                  }
                else
                  free(o);
              }
          }
    }

  for(int i = 0; i < ACTION_SIZEOF_; i++)
    {
      struct stack * old;

      old = widget_get_pointer(obj_controls[i], "options");
      if(old != NULL)
        stack_free(old);

      if(options[i]->size == 0)
        {
          struct ui_widget_select_option * o;
          
          o = widget_new_select_option(NULL, gettext("(unset)"));
          stack_push(options[i], o);
        }

      widget_set_pointer(obj_controls[i], "options",        options[i]);
      widget_set_ulong(obj_controls[i],   "selected_index", 0);
      widget_set_string(obj_controls[i],  "text", "%s",     ((struct ui_widget_select_option *) options[i]->data[0])->text);
    }
}


static void on_window_unload(struct widget * this DG_UNUSED)
{
  int new_sfx, new_music, new_fullscreen, new_opengl, new_specialdays, old_specialdays;
  enum GAME_MODE new_game_mode;

  new_sfx         = widget_get_long(obj_sfx,            "checked");
  new_music       = widget_get_long(obj_music,          "checked");
  globals.volume  = widget_get_long(obj_volume,         "value");
  new_fullscreen  = widget_get_long(obj_fullscreen,     "checked");
  new_opengl      = widget_get_long(obj_opengl,         "checked");
  new_specialdays = widget_get_ulong(obj_special_days,  "checked");
  old_specialdays = widget_get_ulong(obj_special_days,  "initial_state_checked");

  new_game_mode = globals.title_game_mode;
  if(obj_game_mode != NULL)
    {
      int tmp;
      
      tmp = widget_get_ulong(obj_game_mode, "selected_index");
      if(tmp == 0)
        {
          new_game_mode = GAME_MODE_CLASSIC;
        }
      else if(tmp == 1)
        {
          if(traits_get_active() & TRAIT_ADVENTURE_MODE)
            new_game_mode = GAME_MODE_ADVENTURE;
          else
            new_game_mode = GAME_MODE_PYJAMA_PARTY;
        }
      else if(tmp == 2)
        {
          new_game_mode = GAME_MODE_PYJAMA_PARTY;
        }
    }

  bool gfx_restart, sfx_restart;

  gfx_restart = false;
  sfx_restart = false;

  if(new_sfx != globals.use_sfx || new_music != globals.use_music)
    sfx_restart = true;
  if(new_fullscreen != globals.fullscreen || new_opengl != globals.opengl)
    gfx_restart = true;
  if(new_specialdays != old_specialdays)
    {
      struct stack * themes;

      themes = gc_get_stack(GCT_THEME);
      for(unsigned int i = 0; i < themes->size; i++)
        {
          struct theme * t;

          t = themes->data[i];
          if(t != NULL)
            t->enabled = new_specialdays;
        }
      gfx_restart = true;
      sfx_restart = true;
    }
  if(new_game_mode != globals.title_game_mode)
    {
      globals.title_game_mode = new_game_mode;
      gfx_restart = true;
      sfx_restart = true;
    }

  

  { /* Language */
    struct stack * options;

    options = widget_get_pointer(obj_language, "options");
    assert(options != NULL);
    if(options != NULL)
      {
        unsigned int ind;
        char * loc;
        
        ind = widget_get_ulong(obj_language, "selected_index");
        assert(ind < options->size);
        loc = ((struct ui_widget_select_option *) options->data[ind])->data;

        if(strncmp(loc, globals.language, 2)) // Assume the language identifier is 2 characters long. 
          {
            set_language(loc);
            globals.locale_save = true;
            gfx_restart = true;
          }
      }
  }

  

  if(obj_iron_girl_mode != NULL)
    {
      int new_irongirl;

      new_irongirl = widget_get_long(obj_iron_girl_mode, "checked") ? true : false;
      if(new_irongirl != globals.iron_girl_mode)
        {
          globals.iron_girl_mode = new_irongirl;
          if(globals.iron_girl_mode)
            theme_set(THEME_TRAIT, "iron-girl");
          else
            theme_set(THEME_TRAIT, NULL);
          gfx_restart = true;
          sfx_restart = true;
        }
    }


  /* GFX system restart needs to be done elsewhere, it's not always the same (not checked). */
  globals.fullscreen = new_fullscreen;

  /* SFX system restart. */
  if(sfx_restart)
    {
      sfx_cleanup();
      globals.use_sfx = new_sfx;
      globals.use_music = new_music;
      sfx_initialize();
    }

  if(on_settings_changed != NULL)
    {
      on_settings_changed(gfx_restart, sfx_restart, new_opengl);
      on_settings_changed = NULL;
    }

  ui_pop_handlers();
}



static void on_window_close_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      on_settings_exit(true, NULL);
    }
}

static void on_add_binding(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      int controller;

      binding_target = widget_get_ulong(this, "control");
      controller = widget_get_ulong(obj_controllers, "selected_index");

      obj_binding_window = widget_new_window(this, 400, 70, gettext("New binding"));
      widget_set_modal(NULL);
      widget_set_modal(obj_binding_window);
      widget_set_on_unload(obj_binding_window, on_cancel_binding);
      switch(binding_target)
        {
        case ACTION_UP:
        case ACTION_DOWN:
        case ACTION_LEFT:
        case ACTION_RIGHT:
          switch(controller)
            {
            case 0: /* Keyboard */
              widget_new_text(obj_binding_window, 10, 20, gettext("Press any key."));
              break;
            case 1: /* Mouse */
              widget_new_text(obj_binding_window, 10, 20, gettext("Binding to mouse is not supported."));
              break;
            default: /* 2..N Joysticks */
              {
                char buf[128];

                snprintf(buf, sizeof buf, gettext("Move joystick#%d in some direction,"), controller - 1);
                widget_new_text(obj_binding_window, 10, 20, buf);
              }
              break;
            }
          break;
        case ACTION_MANIPULATE:
          switch(controller)
            {
            case 0: /* Keyboard */
              widget_new_text(obj_binding_window, 10, 20, gettext("Press any key."));
              break;
            case 1: /* Mouse */
              widget_new_text(obj_binding_window, 10, 20, gettext("Binding to mouse is not supported."));
              break;
            default: /* 2..N Joysticks */
              {
                char buf[128];

                snprintf(buf, sizeof buf, gettext("Press any button on joystick#%d."), controller - 1);
                widget_new_text(obj_binding_window, 10, 20, buf);
              }
              break;
            }
          break;
        case ACTION_ALT_MANIPULATE:
          switch(controller)
            {
            case 0: /* Keyboard */
              widget_new_text(obj_binding_window, 10, 20, gettext("Press any key."));
              break;
            case 1: /* Mouse */
              widget_new_text(obj_binding_window, 10, 20, gettext("Binding to mouse is not supported."));
              break;
            default: /* 2..N Joysticks */
              {
                char buf[128];

                snprintf(buf, sizeof buf, gettext("Press any button on joystick#%d."), controller - 1);
                widget_new_text(obj_binding_window, 10, 20, buf);
              }
              break;
            }
          break;
        default:
          assert(0);
          break;
        }

      ui_push_handlers();
      ui_clear_handlers();
      switch(controller)
        {
        case 0:  ui_set_handler(UIC_CATCH_ALL_KEYBOARD,  on_binding_execute); break;
        case 1:  ui_set_handler(UIC_CATCH_ALL_MOUSE,     on_binding_execute); break;
        default: ui_set_handler(UIC_CATCH_ALL_JOYSTICKS, on_binding_execute); break;
        }
    }
}

static void on_binding_execute(bool pressed, SDL_Event * event)
{
  if(pressed == true)
    if(event->type == SDL_KEYDOWN         ||
       /*event->type == SDL_MOUSEBUTTONDOWN ||
         event->type == SDL_MOUSEMOTION     ||*/
       event->type == SDL_JOYAXISMOTION   ||
       event->type == SDL_JOYBUTTONDOWN     )
      if(event->type != SDL_JOYAXISMOTION || event->jaxis.value == -32768 || event->jaxis.value == 32767)
        {
          enum UI_COMMAND uic;
          struct ui_binding * new_binding;

          obj_binding_window = widget_delete(obj_binding_window);
          
          switch(binding_target)
            {
            case ACTION_UP:             uic = UIC_UP;             break;
            case ACTION_LEFT:           uic = UIC_LEFT;           break;
            case ACTION_RIGHT:          uic = UIC_RIGHT;          break;
            case ACTION_DOWN:           uic = UIC_DOWN;           break;
            case ACTION_MANIPULATE:     uic = UIC_MANIPULATE;     break;
            case ACTION_ALT_MANIPULATE: uic = UIC_ALT_MANIPULATE; break;
            default:                    uic = UIC_SIZEOF_;        break; /* Without this useless default, gcc complained:
                                                                          * error: ‘uic’ may be used uninitialized in this function */
            }
          assert(uic != UIC_SIZEOF_);

          /* Switch to display the joystick the user just used. */
          if(event->type == SDL_JOYAXISMOTION)
            widget_set_ulong(obj_controllers, "selected_index", 2 + event->jaxis.which);
          else if(event->type == SDL_JOYBUTTONDOWN)
            widget_set_ulong(obj_controllers, "selected_index", 2 + event->jbutton.which);

          struct ui_binding * binding;

          binding = ui_find_binding(event, NULL);
          if(binding == NULL)
            {
              new_binding = ui_bind(uic, event);

              struct stack * options;
              int new_ind;

              bindings = ui_get_bindings(widget_get_ulong(obj_controllers, "selected_index"));
              on_select_controller(obj_controllers);

              options = widget_get_pointer(obj_controls[binding_target], "options");
              new_ind = -1;
              for(unsigned int i = 0; new_ind == -1 && i < options->size; i++)
                {
                  struct ui_widget_select_option * o;

                  o = options->data[i];
                  if(o->data == new_binding)
                    new_ind = i;
                }

              if(new_ind >= 0)
                {
                  widget_set_ulong(obj_controls[binding_target], "selected_index", new_ind);
                  widget_set_string(obj_controls[binding_target], "text", "%s", ((struct ui_widget_select_option *) options->data[new_ind])->text);
                }
            }
          else
            {
              char buf[1024];

              snprintf(buf, sizeof buf, gettext("Error, the binding you requested is taken.\nIt is being used for: %s"), ui_command_name(binding->command));
              widget_new_message_window(gettext("Error"), buf);
            }
        }
}


static void on_cancel_binding(struct widget * this DG_UNUSED)
{
  ui_pop_handlers();
  widget_set_modal(obj_window);
}


static void on_delete_binding(struct widget * this, enum WIDGET_BUTTON button)
{
  if(button == WIDGET_BUTTON_LEFT)
    {
      struct ui_binding * binding;
      struct ui_widget_select_option * o;
      struct stack * options;
      int selected_index;
      int control;

      control = widget_get_ulong(this, "control");

      options = widget_get_pointer(obj_controls[control], "options");
      selected_index = widget_get_ulong(obj_controls[control], "selected_index");
      assert(selected_index >= 0);
      assert(selected_index < (int) options->size);
      o = options->data[selected_index];
      binding = o->data;
      if(binding != NULL)
        if(binding->editable == true)
          {
            binding = ui_unbind(binding);
            
            bindings = ui_get_bindings(widget_get_ulong(obj_controllers, "selected_index"));
            on_select_controller(obj_controllers);
          }
    }
}
