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

#include "cave.h"
#include "diamond_girl.h"
#include "game.h"
#include "gfx.h"
#include "globals.h"
#include "widget.h"
#include "widget_factory.h"
#include <assert.h>
#include <json.h>
#include <libintl.h>


static char current_filename[1024];
static bool fatal_error;

static struct widget * process(struct widget_factory_data * data, struct widget * parent, struct json_object * jsonobj);
static struct widget * process_array(struct widget_factory_data * data, struct widget * parent, struct json_object * jsonobj);
static struct widget * process_object(struct widget_factory_data * data, struct widget * parent, struct json_object * jsonobj);
static void *          find_var(struct widget_factory_data * data, enum WIDGET_FACTORY_DATA_TYPE type, char * name);
static char *          parse_text(struct widget_factory_data * data, const char * text);

struct widget * widget_factory(struct widget_factory_data * data, struct widget * parent, const char * filename)
{
  struct widget * rv;
  char * json;
  int    json_size;

  rv          = NULL;
  fatal_error = false;
  snprintf(current_filename, sizeof current_filename, "ui/%s.json", filename);

  if(read_file(get_data_filename(current_filename), &json, &json_size) == true)
    {
      enum json_tokener_error e;
      struct json_object * obj;

      obj = json_tokener_parse_verbose(json, &e);
      if(obj != NULL)
        {
          rv = process(data, parent, obj);

          if(fatal_error == true)
            if(rv != NULL)
              rv = widget_delete(rv);
        }
      else
        {
          fprintf(stderr, "%s: Failed to parse: %d: %s\n", get_data_filename(current_filename), (int) e, json_tokener_error_desc(e));
        }
      free(json);
    }
  else
    fprintf(stderr, "%s: Failed to read '%s'.", __FUNCTION__, get_data_filename(current_filename));

  assert(rv != NULL);

  return rv;
}


static struct widget * process(struct widget_factory_data * data, struct widget * parent, struct json_object * jsonobj)
{
  struct widget * rv;

  rv = NULL;

  assert(jsonobj != NULL);
  if(fatal_error == false && jsonobj != NULL)
    {
      enum json_type type;

      type = json_object_get_type(jsonobj);
      switch(type)
        {
        case json_type_object:
          rv = process_object(data, parent, jsonobj);
          break;
        case json_type_array:
          rv = process_array(data, parent, jsonobj);
          break;
        default:
          fprintf(stderr, "unknown type\n");
          assert(0);
          break;
        }
    }

  return rv;
}


static struct widget * process_array(struct widget_factory_data * data, struct widget * parent, struct json_object * jsonobj)
{
  struct widget * rv;

  rv = NULL;
  assert(jsonobj != NULL);
  if(fatal_error == false && jsonobj != NULL)
    {
      struct array_list * array;

      array = json_object_get_array(jsonobj);
      assert(array != NULL);
      if(array != NULL)
        {
          int len;

          len = json_object_array_length(jsonobj);
          for(int i = 0; i < len; i++)
            {
              struct json_object * tmp;

              tmp = json_object_array_get_idx(jsonobj, i);
              assert(tmp != NULL);
              if(tmp != NULL)
                {
                  struct widget * w;

                  w = process(data, parent, tmp);
                  if(rv == NULL) /* return the first */
                    rv = w;
                }
            }
        }
    }

  return rv;
}


static struct widget * process_object(struct widget_factory_data * data, struct widget * parent, struct json_object * jsonobj)
{
  struct json_object_iterator it;
  struct json_object_iterator itEnd;
  struct json_object * children;
  bool ok;
  enum WTYPE
  {
    WTYPE_BUTTON,
    WTYPE_FRAME,
    WTYPE_GRID,
    WTYPE_GRID_ROW,
    WTYPE_MAP,
    WTYPE_QUEST_MENU,
    WTYPE_SELECT,
    WTYPE_SPACER,
    WTYPE_TEXT,
    WTYPE_TITLE_CREDITS,
    WTYPE_TITLE_HELP,
    WTYPE_TITLE_HIGHSCORES,
    WTYPE_TITLE_PARTYSTATUS,
    WTYPE_WINDOW,
    WTYPE_SIZEOF_
  };
  struct
  {
    enum WTYPE     type;
    char *         name;
  } wtypes[] =
      {
        { WTYPE_BUTTON,            "button"            },
        { WTYPE_FRAME,             "frame"             },
        { WTYPE_GRID,              "grid"              },
        { WTYPE_GRID_ROW,          "row"               },
        { WTYPE_MAP,               "map"               },
        { WTYPE_QUEST_MENU,        "quest_menu"        },
        { WTYPE_SELECT,            "select"            },
        { WTYPE_SPACER,            "spacer"            },
        { WTYPE_TEXT,              "text"              },
        { WTYPE_TITLE_CREDITS,     "title_credits"     },
        { WTYPE_TITLE_HELP,        "title_help"        },
        { WTYPE_TITLE_HIGHSCORES,  "title_highscores"  },
        { WTYPE_TITLE_PARTYSTATUS, "title_partystatus" },
        { WTYPE_WINDOW,            "window"            },
        { WTYPE_SIZEOF_,           NULL                }
      };
  struct
  {
    struct widget * widget;
    enum WTYPE      type;
    char            id[256];
    int             x, y;
    int             width, height;
    int             padding_y;
    bool            h_align;
    char            title[256];
    char            tooltip[256];
    bool            modal;    /* Set the widget to be modal. */
    bool            focus;    /* Set focus to the widget. */
    bool            no_focus; /* Set the widget to be non-focusable. */
    char            on_activate_at[256];
    char            on_activate[256];
    char            on_draw[256];
    char            on_focus[256];
    char            on_mouse_move[256];
    char            on_release[256];
    char            on_unload[256];
    struct stack *  nav_up;
    struct stack *  nav_down;
    struct stack *  nav_left;
    struct stack *  nav_right;
  } this;
  

  it       = json_object_iter_begin(jsonobj);
  itEnd    = json_object_iter_end(jsonobj);
  children = NULL;
  ok       = true;

  this.widget      = NULL;
  this.type        = WTYPE_SIZEOF_;
  strcpy(this.id, "");
  this.x           = 0;
  this.y           = 0;
  this.width       = 0;
  this.height      = 0;
  this.padding_y   = 0;
  this.h_align     = false;
  strcpy(this.title,   "");
  strcpy(this.tooltip, "");
  this.modal       = false;
  this.focus       = false;
  this.no_focus    = false;
  strcpy(this.on_activate_at, "");
  strcpy(this.on_activate,    "");
  strcpy(this.on_draw,        "");
  strcpy(this.on_focus,       "");
  strcpy(this.on_mouse_move,  "");
  strcpy(this.on_release,     "");
  strcpy(this.on_unload,      "");
  this.nav_up    = stack_new();
  this.nav_down  = stack_new();
  this.nav_left  = stack_new();
  this.nav_right = stack_new();
  
  while(ok == true && !json_object_iter_equal(&it, &itEnd))
    { /* Gather data for the widget to be built. */
      const char *         name;
      struct json_object * value;
      enum json_type       type;
      
      name  = json_object_iter_peek_name(&it);
      value = json_object_iter_peek_value(&it);
      type = json_object_get_type(value);
      if(!strcmp(name, "type"))
        { /* Handle the type separately. */
          assert(this.type == WTYPE_SIZEOF_);
          if(type == json_type_string)
            { /* Match the string to the enum. */
              const char * stringvalue;

              stringvalue = json_object_get_string(value);
              for(int i = 0; this.type == WTYPE_SIZEOF_ && wtypes[i].type != WTYPE_SIZEOF_; i++)
                if(!strcmp(stringvalue, wtypes[i].name))
                  this.type = wtypes[i].type;
              if(this.type == WTYPE_SIZEOF_)
                {
                  fprintf(stderr, "%s: Unknown type '%s'.\n", get_data_filename(current_filename), stringvalue);
                  ok = false;
                }
            }
          else
            {
              fprintf(stderr, "%s: Illegal value for 'type', expected 'string', got '%s'.\n", get_data_filename(current_filename), json_type_to_name(type));
              ok = false;
            }
        }
      else if(!strcmp(name, "children"))
        { /* Children. */
          children = json_object_get(value);
        }
      else
        { /* Rest are handled here. */
          int pw, ph;

          if(parent != NULL)
            {
              pw = widget_width(parent);
              ph = widget_height(parent);
            }
          else
            {
              pw = SCREEN_WIDTH;
              ph = SCREEN_HEIGHT;
            }

          struct
          {
            char *         name;
            enum json_type type;
            void *         value;           /* The value is stored in here. */
            size_t         value_length;    /* The length of the space in *value, if applicaple. */
            int            relative_number; /* If the value is a percentage, use this as the base of 100%. */
          } inputs[] =
              {
                { "id",               json_type_string,  this.id,             sizeof this.id,             0  },
                { "x",                json_type_int,     &this.x,             1,                          pw },
                { "y",                json_type_int,     &this.y,             1,                          ph },
                { "width",            json_type_int,     &this.width,         1,                          pw },
                { "height",           json_type_int,     &this.height,        1,                          ph },
                { "title",            json_type_string,  this.title,          sizeof this.title,          0  },
                { "tooltip",          json_type_string,  this.tooltip,        sizeof this.tooltip,        0  },
                { "modal",            json_type_boolean, &this.modal,         1,                          0  },
                { "focus",            json_type_boolean, &this.focus,         1,                          0  },
                { "no-focus",         json_type_boolean, &this.no_focus,      1,                          0  },
                { "padding-y",        json_type_int,     &this.padding_y,     1,                          0  },
                { "h-align",          json_type_boolean, &this.h_align,       1,                          0  },
                { "on-activate-at",   json_type_string,  this.on_activate_at, sizeof this.on_activate_at, 0  },
                { "on-activate",      json_type_string,  this.on_activate,    sizeof this.on_activate,    0  },
                { "on-draw",          json_type_string,  this.on_draw,        sizeof this.on_draw,        0  },
                { "on-focus",         json_type_string,  this.on_focus,       sizeof this.on_focus,       0  },
                { "on-mouse-move",    json_type_string,  this.on_mouse_move,  sizeof this.on_mouse_move,  0  },
                { "on-release",       json_type_string,  this.on_release,     sizeof this.on_release,     0  },
                { "on-unload",        json_type_string,  this.on_unload,      sizeof this.on_unload,      0  },
                { "navigation-up",    json_type_array,   this.nav_up,         1,                          0  },
                { "navigation-down",  json_type_array,   this.nav_down,       1,                          0  },
                { "navigation-left",  json_type_array,   this.nav_left,       1,                          0  },
                { "navigation-right", json_type_array,   this.nav_right,      1,                          0  },
                { NULL,               json_type_int,     NULL,                0,                          0  }
              };
          bool done;

          done = false;
          for(int i = 0; ok == true && done == false && inputs[i].name != NULL; i++)
            if(!strcmp(name, inputs[i].name))
              {
                done = true;
                switch(inputs[i].type)
                  {
                  case json_type_array:
                    if(type == json_type_array)
                      { /* push the array contents as strings into the given stack */
                        struct array_list * array;

                        array = json_object_get_array(value);
                        assert(array != NULL);
                        if(array != NULL)
                          {
                            int len;

                            len = json_object_array_length(value);
                            for(int j = 0; j < len; j++)
                              {
                                struct json_object * tmp;

                                tmp = json_object_array_get_idx(value, j);
                                assert(tmp != NULL);
                                if(tmp != NULL)
                                  stack_push(inputs[i].value, strdup(json_object_get_string(tmp)));
                              }
                          }
                      }
                    else
                      {
                        fprintf(stderr, "%s: Illegal value for '%s', expected '%s', got '%s'.\n",
                                get_data_filename(current_filename),
                                inputs[i].name,
                                json_type_to_name(inputs[i].type),
                                json_type_to_name(type));
                        ok = false;
                      }
                    break;
                  case json_type_string:
                    if(type == json_type_string)
                      {
                        const char * stringvalue;
                        char * parsed;

                        stringvalue = json_object_get_string(value);
                        parsed = parse_text(data, stringvalue);
                        if(parsed != NULL)
                          {
                            assert(strlen(parsed) < inputs[i].value_length);
                            snprintf(inputs[i].value, inputs[i].value_length, "%s", parsed);
                            free(parsed);
                          }
                      }
                    else
                      {
                        fprintf(stderr, "%s: Illegal value for '%s', expected '%s', got '%s'.\n",
                                get_data_filename(current_filename),
                                inputs[i].name,
                                json_type_to_name(inputs[i].type),
                                json_type_to_name(type));
                        ok = false;
                      }
                    break;
                  case json_type_int:
                    {
                      int * valueptr;
                        
                      valueptr = inputs[i].value;
                      if(type == json_type_int)
                        {
                          *valueptr = json_object_get_int(value);
                        }
                      else if(type == json_type_string)
                        {
                          const char * stringvalue;
                          int n;
                          char * endptr;
                          
                          stringvalue = json_object_get_string(value);
                          n = strtoul(stringvalue, &endptr, 0);
                          if(*endptr == '\0')
                            {
                              *valueptr = n;
                            }
                          else if(!strcmp(endptr, "%"))
                            {
                              *valueptr = n * inputs[i].relative_number / 100;
                            }
                          else
                            {
                              fprintf(stderr, "%s: Illegal value '%s' for '%s'.\n", get_data_filename(current_filename), stringvalue, inputs[i].name);
                              ok = false;
                            }
                        }
                      else
                        {
                          fprintf(stderr, "%s: Illegal value for '%s', expected '%s', got '%s'.\n",
                                  get_data_filename(current_filename),
                                  inputs[i].name,
                                  json_type_to_name(inputs[i].type),
                                  json_type_to_name(type));
                          ok = false;
                        }
                    }
                    break;
                  case json_type_boolean:
                    if(type == json_type_boolean)
                      {
                        bool * valueptr;
                        
                        valueptr = inputs[i].value;
                        if(json_object_get_boolean(value))
                          *valueptr = true;
                        else
                          *valueptr = false;
                      }
                    else
                      {
                        fprintf(stderr, "%s: Illegal value for '%s', expected '%s', got '%s'.\n",
                                get_data_filename(current_filename),
                                inputs[i].name,
                                json_type_to_name(inputs[i].type),
                                json_type_to_name(type));
                        ok = false;
                      }
                    break;
                  default:
                    fprintf(stderr, "%s: Unknown type '%s'.\n", 
                            get_data_filename(current_filename),
                            json_type_to_name(type));
                    ok = false;
                    break;
                  }
              }
        }

      json_object_iter_next(&it);
    }

  if(ok == true)
    { /* Build the widget. */
      bool set_width; /* Whether the width needs to be set separately after widget creation. */
      bool set_height;

      set_width  = false;
      set_height = false;
      switch(this.type)
        {
        case WTYPE_BUTTON:
          this.widget = widget_new_button(parent, this.x, this.y, gettext(this.title));
          set_width  = true;
          set_height = true;
          break;
        case WTYPE_FRAME:
          this.widget = widget_new_frame(parent, this.x, this.y, this.width, this.height);
          break;
        case WTYPE_GRID:
          this.widget = widget_new_grid(parent, this.x, this.y);
          set_width  = true;
          set_height = true;
          break;
        case WTYPE_GRID_ROW:
          this.widget = widget_new_grid_row(parent, this.y);
          set_width  = true;
          set_height = true;
          break;
        case WTYPE_MAP:
          {
            struct cave * cave;
            struct map * map;

            cave = find_var(data, WFDT_CAVE, "cave");
            map  = find_var(data, WFDT_MAP, "map");
            
            this.widget = widget_new_map(parent, this.x, this.y, this.width, this.height, map, cave->game_mode);
          }
          break;
        case WTYPE_QUEST_MENU:
          this.widget = widget_new_quest_menu(parent, this.x, this.y);
          break;
        case WTYPE_SELECT:
          this.widget = widget_new_select(parent, this.x, this.y, this.width, 0, NULL);
          break;
        case WTYPE_SPACER:
          this.widget = widget_new_spacer(parent, this.width, this.height);
          break;
        case WTYPE_TEXT:
          this.widget = widget_new_text(parent, this.x, this.y, gettext(this.title));
          set_width  = true;
          set_height = true;
          break;
        case WTYPE_TITLE_CREDITS:
          this.widget = widget_new_title_credits(parent, this.x, this.y, this.width);
          break;
        case WTYPE_TITLE_HELP:
          this.widget = widget_new_title_help(parent, this.x, this.y, this.width, this.height);
          break;
        case WTYPE_TITLE_HIGHSCORES:
          {
            struct cave * cave;

            cave = find_var(data, WFDT_CAVE, "cave");
            this.widget = widget_new_title_highscores(parent, this.x, this.y, cave);
            set_width  = true;
            set_height = true;
          }
          break;
        case WTYPE_TITLE_PARTYSTATUS:
          this.widget = widget_new_title_partystatus(parent, this.x, this.y);
          set_width  = true;
          set_height = true;
          break;
        case WTYPE_WINDOW:
          this.widget = widget_new_window(parent, this.width, this.height, gettext(this.title));
          break;
        case WTYPE_SIZEOF_:
          ok = false;
          break;
        }

      if(ok == true && set_width == true && this.width > 0)
        widget_set_width(this.widget, this.width);

      if(ok == true && set_height == true && this.height > 0)
        widget_set_height(this.widget, this.height);

      if(ok == true && strlen(this.id) > 0)
        {
          assert(widget_find(NULL, this.id) == NULL);
          widget_set_string(this.widget, "id", "%s", this.id);
        }

      if(ok == true && this.modal == true)
        widget_set_modal(this.widget);

      if(ok == true && this.focus == true)
        widget_set_focus(this.widget);

      if(ok == true && this.no_focus == true)
        widget_set_no_focus(this.widget, false); /* Sets non-recursively. */

      if(ok == true && this.padding_y > 0)
        widget_set_ulong(this.widget, "padding-y", this.padding_y);

      if(ok == true && this.h_align == true)
        widget_add_flags(this.widget, WF_ALIGN_CENTER);

      if(ok == true && strlen(this.on_activate_at) > 0)
        {
          ui_func_on_activate_at_t f;
            
          f = find_var(data, WFDT_ON_ACTIVATE_AT, this.on_activate_at);
          if(f != NULL)
            widget_set_on_activate_at(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.on_activate) > 0)
        {
          ui_func_on_activate_t f;
            
          f = find_var(data, WFDT_ON_ACTIVATE, this.on_activate);
          if(f != NULL)
            widget_set_on_activate(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.on_draw) > 0)
        {
          ui_func_on_draw_t f;
            
          f = find_var(data, WFDT_ON_DRAW, this.on_draw);
          if(f != NULL)
            widget_set_on_draw(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.on_focus) > 0)
        {
          ui_func_on_focus_t f;
            
          f = find_var(data, WFDT_ON_FOCUS, this.on_focus);
          if(f != NULL)
            widget_set_on_focus(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.on_mouse_move) > 0)
        {
          ui_func_on_mouse_move_t f;
            
          f = find_var(data, WFDT_ON_MOUSE_MOVE, this.on_mouse_move);
          if(f != NULL)
            widget_set_on_mouse_move(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.on_release) > 0)
        {
          ui_func_on_activate_t f;
            
          f = find_var(data, WFDT_ON_RELEASE, this.on_release);
          if(f != NULL)
            widget_set_on_release(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.on_unload) > 0)
        {
          ui_func_on_unload_t f;
            
          f = find_var(data, WFDT_ON_UNLOAD, this.on_unload);
          if(f != NULL)
            widget_set_on_unload(this.widget, f);
          else
            ok = false;
        }

      if(ok == true && strlen(this.tooltip) > 0)
        widget_set_tooltip(this.widget, strdup(gettext(this.tooltip)));
    }


  if(ok == true)
    {
      assert(this.widget != NULL);

      /* Process possible children. */
      if(children != NULL)
        {
          process(data, this.widget, children);

          if(this.type == WTYPE_GRID || this.type == WTYPE_GRID_ROW)
            widget_resize_to_fit_children(this.widget);
        }

      /* Process navigation links. */
      bool found;

      found = false;
      for(unsigned int i = 0; found == false && i < this.nav_up->size; i++)
        {
          struct widget * tmp;

          tmp = widget_find(NULL, this.nav_up->data[i]);
          if(tmp != NULL)
            {
              widget_set_navigation_updown(tmp, this.widget);
              found = true;
            }
        }

      found = false;
      for(unsigned int i = 0; found == false && i < this.nav_down->size; i++)
        {
          struct widget * tmp;

          tmp = widget_find(NULL, this.nav_down->data[i]);
          if(tmp != NULL)
            {
              widget_set_navigation_updown(this.widget, tmp);
              found = true;
            }
        }

      found = false;
      for(unsigned int i = 0; found == false && i < this.nav_left->size; i++)
        {
          struct widget * tmp;

          tmp = widget_find(NULL, this.nav_left->data[i]);
          if(tmp != NULL)
            {
              widget_set_navigation_leftright(tmp, this.widget);
              found = true;
            }
        }

      found = false;
      for(unsigned int i = 0; found == false && i < this.nav_right->size; i++)
        {
          struct widget * tmp;

          tmp = widget_find(NULL, this.nav_right->data[i]);
          if(tmp != NULL)
            {
              widget_set_navigation_leftright(this.widget, tmp);
              found = true;
            }
        }
    }
  else
    {
      if(this.widget != NULL)
        this.widget = widget_delete(this.widget);
      fatal_error = true;
    }


  char * tmp;

  do { tmp = stack_pop(this.nav_up); free(tmp); } while(tmp != NULL);
  this.nav_up    = stack_free(this.nav_up);

  do { tmp = stack_pop(this.nav_down); free(tmp); } while(tmp != NULL);
  this.nav_down  = stack_free(this.nav_down);

  do { tmp = stack_pop(this.nav_left); free(tmp); } while(tmp != NULL);
  this.nav_left  = stack_free(this.nav_left);

  do { tmp = stack_pop(this.nav_right); free(tmp); } while(tmp != NULL);
  this.nav_right = stack_free(this.nav_right);

  return this.widget;
}


static void * find_var(struct widget_factory_data * data, enum WIDGET_FACTORY_DATA_TYPE type, char * name)
{
  void * var;

  var = NULL;
  for(int i = 0; var == NULL && data[i].type != WFDT_SIZEOF_; i++)
    if(!strcmp(data[i].name, name))
      {
        if(data[i].type == type)
          var = data[i].value;
        else
          fprintf(stderr, "%s: Incorrect variable type for '%s'.\n", get_data_filename(current_filename), name);
      }

  if(var == NULL)
    fprintf(stderr, "%s: No such variable: %s\n", get_data_filename(current_filename), name);

  return var;
}



static char * parse_text(struct widget_factory_data * data, const char * text)
{
  char buf[1024 * 4];
  char * pos;
  
  pos = buf;
  while(*text != '\0')
    {
      if(*text == '$' && *(text + 1) == '{')
        {
          char varname[256];
          char * v;

          text += 2;
          v = varname;
          while(*text != '\0' && *text != '}')
            {
              *v = *text;
              v++;
              text++;
            }
          *v = '\0';

          if(*text == '}')
            text++;

          char * varvalue;
          
          varvalue = find_var(data, WFDT_STRING, varname);
          if(varvalue != NULL)
            while(*varvalue != '\0')
              {
                *pos = *varvalue;
                pos++;
                varvalue++;
              }
        }
      else
        {
          *pos = *text;
          pos++;
          text++;
        }
    }

  *pos = '\0';


  char * rv;

  rv = strdup(buf);

  return rv;
}
