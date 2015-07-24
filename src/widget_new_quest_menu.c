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

#include "event.h"
#include "font.h"
#include "gfx.h"
#include "globals.h"
#include "image.h"
#include "quest.h"
#include "random.h"
#include "treasure.h"
#include "widget.h"
#include <assert.h>
#include <libintl.h>

static void on_quest_diary_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quest_treasures_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quest_examine_notes_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quest_examine_treasure_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quest_visit_library_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_quest_visit_cafe_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_questline_selected(struct widget * this);
static void refresh(struct widget * window);
static void on_quest_state_changed(void * user_data, int64_t value);

struct widget * widget_new_quest_menu(struct widget * parent, int x, int y)
{
  struct widget * window;

  window = widget_new_frame(parent, x, y, 600, 380);
  if(window != NULL)
    {
      widget_delete_flags(window, WF_DRAW_BORDERS | WF_CAST_SHADOW | WF_BACKGROUND);
      refresh(window);

      event_register(EVENT_TYPE_QUEST_STATE_CHANGED, on_quest_state_changed, window);
    }

  return window;
}




static void on_quest_diary_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  quest_diary(globals.questlines[globals.active_questline]);
}

static void on_quest_treasures_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct questline * ql;

  ql = globals.questlines[globals.active_questline];
  if(questline_treasure_get_collected_count(ql) == 0)
    ql = NULL;
  
  quest_treasures(ql, NULL);
}

static void on_quest_examine_notes_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  quest_action(globals.questlines[globals.active_questline],
               QUEST_ACTION_EXAMINE_NOTES);
}

static void on_quest_examine_treasure_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  quest_action(globals.questlines[globals.active_questline],
               QUEST_ACTION_EXAMINE_OBJECT);
}

static void on_quest_visit_library_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  quest_action(globals.questlines[globals.active_questline],
               QUEST_ACTION_VISIT_LIBRARY);
}

static void on_quest_visit_cafe_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  quest_action(globals.questlines[globals.active_questline],
               QUEST_ACTION_VISIT_CAFE);
}



static void on_questline_selected(struct widget * this)
{
  unsigned int item_index;

  item_index = widget_get_ulong(this, "selected_index");
  assert(item_index < globals.questlines_size);
  if(item_index < globals.questlines_size)
    {
      struct stack * options;
      struct ui_widget_select_option * o;
      struct questline * ql;
      
      options = widget_get_select_options(this);
      o = options->data[item_index];
      ql = o->data;
      assert(ql != NULL);

      for(unsigned int i = 0; ql != NULL && i < globals.questlines_size; i++)
        if(globals.questlines[i] == ql)
          {
            globals.active_questline = i;
            ql = NULL;
          }
      assert(ql == NULL);

      struct widget * window;

      window = widget_get_widget_pointer(this, "window");
      refresh(window);
      widget_set_focus(widget_find(window, "obj_questline")); // Using widget_find() to find this, because the call to refresh() has deleted this (and created new one).
    }
}





static void refresh(struct widget * window)
{
  if(window == NULL)
    return;
  
  char buf[256];
  int y;

  y = 0;

  
  /* Remove old widgets. */
  char * ids[] = { "cafe", "notes", "diary", "library", "stand", "latest", "message", "questline_completion_text", "questline_label", "obj_questline", NULL };

  for(int i = 0; ids[i] != NULL; i++)
    {
      struct widget * tmp;
      
      tmp = widget_find(window, ids[i]);
      if(tmp != NULL)
        widget_delete(tmp);
    }
  widget_set_widget_pointer(window, "focus_down_object", NULL);

  /* Setup new widgets. */
  if(traits_get(globals.title_game_mode) & TRAIT_QUESTS)
    {
      if(globals.questlines_size > 0)
        {
          const int padding = 20;
          struct questline * questline;
          struct widget * obj_questline_label;
          struct widget * obj_questline;
          struct widget * obj_notes;
          struct widget * obj_diary;
          struct widget * obj_cafe;
          struct widget * obj_library;
          struct widget * obj_stand;
          struct widget * obj_latest;
          
          obj_questline = obj_notes = obj_diary = obj_cafe = obj_library = obj_stand = obj_latest = NULL;

          questline = globals.questlines[globals.active_questline];

          /* Questline label */
          obj_questline_label = widget_new_text(window, 20, 0, gettext("Questline:"));
          widget_set_string(obj_questline_label, "id", "questline_label");

          /* Questline select */
          obj_questline = widget_new_select(window, widget_x(obj_questline_label) + widget_width(obj_questline_label) + padding, 0, 350, 0, NULL);
          if(obj_questline != NULL)
            {
              widget_set_string(obj_questline, "id", "obj_questline");
              widget_set_pointer(obj_questline, "on_select", 'P', on_questline_selected);
              widget_set_widget_pointer(obj_questline, "window", window);
  
              /* Populate the questlines -select list. */
              struct stack * qlines;
              int ind;

              qlines = stack_new();
              ind = 0;
              for(unsigned int i = 0; i < globals.questlines_size; i++)
                if(globals.questlines[i]->current_phase != QUESTLINE_PHASE_NONE)
                  {
                    struct ui_widget_select_option * option;
                  
                    option = widget_new_select_option(globals.questlines[i], questline_name(globals.questlines[i]));
                    stack_push(qlines, option);

                    if(i == globals.active_questline)
                      widget_set_ulong(obj_questline, "selected_index", ind);
                    ind++;
                  }
              widget_set_select_options(obj_questline, qlines);
            }

          { /* Questline completion */
            unsigned int count;
            struct quest * q;
            
            count = questline->current_quest + 1;
            q = questline->quests->data[questline->current_quest];
            if(q->completed == 0)
              count--;

            snprintf(buf, sizeof buf, "%u%%", count * 100 / questline->quests->size);

            struct widget * w;
            w = widget_new_text(window, widget_x(obj_questline) + widget_width(obj_questline) + padding * 2, 0, buf);
            widget_set_string(w, "id", "questline_completion_text");
          }
          
          
          
          /* Diary */
          int tmpy;

          obj_diary = widget_new_button(window, 20, font_height() + 30, "");
          widget_set_string(obj_diary, "id", "diary");
          widget_set_image(obj_diary, "image", GFX_IMAGE_DIARY);
          widget_delete_flags(obj_diary, WF_BACKGROUND);
          widget_add_flags(obj_diary, WF_ONLY_FOCUS_BORDERS);
          widget_set_tooltip(obj_diary, gettext("My Diary"));
          widget_set_on_release(obj_diary, on_quest_diary_clicked);
          widget_to_image_size(obj_diary, gfx_image(GFX_IMAGE_BOOKPILE));
          tmpy = widget_y(obj_diary) + widget_height(obj_diary);
          if(tmpy > y)
            y = tmpy;

          /* Notes */
          obj_notes = widget_new_button(window, 170, font_height() + 30, "");
          widget_set_string(obj_notes, "id", "notes");
          widget_set_image(obj_notes, "image", GFX_IMAGE_NOTES);
          widget_delete_flags(obj_notes, WF_BACKGROUND);
          widget_add_flags(obj_notes, WF_ONLY_FOCUS_BORDERS);
          snprintf(buf, sizeof buf, gettext("Notes of %s"), questline->ancient_person.name);
          widget_set_tooltip(obj_notes, strdup(buf));
          widget_set_on_release(obj_notes, on_quest_examine_notes_clicked);
          widget_to_image_size(obj_notes, gfx_image(GFX_IMAGE_NOTES));
          tmpy = widget_y(obj_notes) + widget_height(obj_notes);
          if(tmpy > y)
            y = tmpy;
          
          /* Cafe */
          obj_cafe = widget_new_button(window, 320, font_height() + 30, "");
          widget_set_string(obj_cafe, "id", "cafe");
          widget_set_image(obj_cafe, "image", GFX_IMAGE_CAFE);
          widget_delete_flags(obj_cafe, WF_BACKGROUND);
          widget_add_flags(obj_cafe, WF_ONLY_FOCUS_BORDERS);
          widget_set_tooltip(obj_cafe, gettext("Cafe"));
          widget_set_on_release(obj_cafe, on_quest_visit_cafe_clicked);
          widget_to_image_size(obj_cafe, gfx_image(GFX_IMAGE_CAFE));
          tmpy = widget_y(obj_cafe) + widget_height(obj_cafe);
          if(tmpy > y)
            y = tmpy;
          
      
      
          /* Library */
          obj_library = widget_new_button(window, 0, font_height() + 30, "");
          widget_set_string(obj_library, "id", "library");
          widget_set_tooltip(obj_library, gettext("Library"));
          widget_set_image(obj_library, "image", GFX_IMAGE_BOOKPILE);
          widget_delete_flags(obj_library, WF_BACKGROUND);
          widget_add_flags(obj_library, WF_ONLY_FOCUS_BORDERS);
          widget_set_on_release(obj_library, on_quest_visit_library_clicked);
          widget_to_image_size(obj_library, gfx_image(GFX_IMAGE_BOOKPILE));

          tmpy = widget_y(obj_library) + widget_height(obj_library);
          if(tmpy > y)
            y = tmpy;

          widget_set_x(obj_library, widget_width(window) - widget_width(obj_library) - 20);


          /* Treasure stand */
          bool has_treasures;

          has_treasures = false;
          for(unsigned int i = 0; has_treasures == false && i < globals.questlines_size; i++)
            {
              struct questline * ql;

              ql = globals.questlines[i];
              if(questline_treasure_get_collected_count(ql) > 0)
                has_treasures = true;
            }
          if(has_treasures == true)
            {
              obj_stand = widget_new_button(window, 5, y, "");
              widget_set_string(obj_stand, "id", "stand");
              widget_set_tooltip(obj_stand, gettext("My collection"));
              widget_set_image(obj_stand, "image", GFX_IMAGE_TREASURESTAND);
              widget_delete_flags(obj_stand, WF_BACKGROUND);
              widget_add_flags(obj_stand, WF_ONLY_FOCUS_BORDERS);
              widget_to_image_size(obj_stand, gfx_image(GFX_IMAGE_TREASURESTAND));
              widget_set_on_release(obj_stand, on_quest_treasures_clicked);
          
              tmpy = widget_y(obj_stand) + widget_height(obj_stand);
              if(tmpy > y)
                y = tmpy;
            }

              
          if(questline->current_quest > 0 || questline->current_phase == QUESTLINE_PHASE_TREASURE_COLLECTED)
            {
              /* Latest find */
              struct quest * q;
              bool gravestone;

              gravestone = false;
              if(questline->current_phase == QUESTLINE_PHASE_OPENED)
                q = questline->quests->data[questline->current_quest - 1];
              else
                {
                  q = questline->quests->data[questline->current_quest];
                  if(questline->current_quest + 1 == questline->quests->size)
                    gravestone = true; // Last quest.
                }

              if(q->treasure_sold == false)
                {
                  obj_latest = widget_new_button(window, 0, 0, "");
                  widget_set_string(obj_latest, "id", "latest");
                  widget_delete_flags(obj_latest, WF_BACKGROUND);
                  widget_add_flags(obj_latest, WF_ONLY_FOCUS_BORDERS);
                  if(gravestone == true)
                    {
                      widget_set_image(obj_latest, "image", GFX_IMAGE_GRAVESTONE);
                      widget_to_image_size(obj_latest, gfx_image(GFX_IMAGE_GRAVESTONE));
                    }
                  else
                    {
                      widget_set_image_pointer(obj_latest, "raw_image", gfx_image_treasure(q->treasure_object, false));
                      widget_add_flags(obj_latest, WF_SCALE_RAW_IMAGE);
                      
                      double w, h;

                      widget_to_image_size(obj_latest, gfx_image_treasure(q->treasure_object, false));
                      w = widget_width(obj_latest);
                      h = widget_height(obj_latest);
                      max_image_size(&w, 100, &h, 150);
                      widget_set_width(obj_latest, w);
                      widget_set_height(obj_latest, h);
                    }

                  snprintf(buf, sizeof buf, gettext("Latest find: %s %s"),
                           treasure_material_name(q->treasure_object->material),
                           treasure_type_name(q->treasure_object->type));
                  widget_set_tooltip(obj_latest, strdup(buf));
                  widget_set_on_release(obj_latest, on_quest_examine_treasure_clicked);

                  widget_set_x(obj_latest, widget_width(window) - widget_width(obj_latest) - 20);
                  widget_set_y(obj_latest, y - widget_height(obj_latest));
          
                  tmpy = widget_y(obj_latest) + widget_height(obj_latest);
                  if(tmpy > y)
                    y = tmpy;
                }
            }

          /* Navigation stuff */
          if(obj_notes != NULL && obj_diary != NULL && obj_library != NULL)
            {
              if(obj_questline != NULL)
                {
                  widget_set_navigation_up(obj_notes, obj_questline);
                  widget_set_navigation_updown(obj_questline, obj_diary);
                  widget_set_navigation_up(obj_cafe, obj_questline);
                  widget_set_navigation_up(obj_library, obj_questline);
                }

              widget_set_navigation_leftright(obj_diary, obj_notes);
              widget_set_navigation_leftright(obj_notes, obj_cafe);
              widget_set_navigation_leftright(obj_cafe, obj_library);

              if(obj_stand == NULL)
                {
                  widget_set_widget_pointer(window, "focus_down_object", obj_diary);
                }
              else
                {
                  widget_set_navigation_down(obj_notes, obj_stand);
                  widget_set_navigation_updown(obj_diary, obj_stand);
                  widget_set_navigation_down(obj_cafe, obj_stand);
                  widget_set_widget_pointer(window, "focus_down_object", obj_stand);
          
                  if(obj_latest != NULL)
                    {
                      widget_set_navigation_leftright(obj_stand, obj_latest);
                      widget_set_navigation_updown(obj_library, obj_latest);
                    }
                }
            }
        }
      else
        {
          struct widget * b;
      
          b = widget_new_text(window, 5, font_height() * 2, gettext("No questlines available."));
          widget_set_string(b, "id", "message");
          widget_center_horizontally(b);
          y = widget_y(b) + widget_height(b);
        }
    }
  else
    {
      struct widget * b;
      char * text;

      text = NULL;
      switch(globals.title_game_mode)
        {
        case GAME_MODE_CLASSIC:      text = gettext("Quests are not available in classic game mode.");      break;
        case GAME_MODE_ADVENTURE:    text = gettext("Quests trait is not active.");                         break;
        case GAME_MODE_PYJAMA_PARTY: text = gettext("Quests are not available in pyjama party game mode."); break;
        }
      assert(text != NULL);
        
      b = widget_new_text(window, 5, font_height() * 2, text);
      widget_set_string(b, "id", "message");
      widget_center_horizontally(b);
      y = widget_y(b) + widget_height(b);
    }

  widget_set_height(window, y);
}



static void on_quest_state_changed(void * user_data, int64_t value DG_UNUSED)
{
  refresh(user_data);
}



void widget_quest_menu_setup_navigation(struct widget * quest_menu, struct widget * widget_down)
{
  struct widget * obj_cafe;

  obj_cafe = widget_find(quest_menu, "cafe");
  if(obj_cafe != NULL)
    {
      struct widget * obj_diary;
      struct widget * obj_latest;
      struct widget * obj_library;
      struct widget * obj_notes;
      struct widget * obj_stand;

      obj_diary   = widget_find(quest_menu, "diary");
      obj_latest  = widget_find(quest_menu, "latest");
      obj_library = widget_find(quest_menu, "library");
      obj_notes   = widget_find(quest_menu, "notes");
      obj_stand   = widget_find(quest_menu, "stand");
      if(obj_stand == NULL)
        {
          widget_set_navigation_down(obj_cafe,    widget_down);
          widget_set_navigation_down(obj_diary,   widget_down);
          widget_set_navigation_down(obj_library, widget_down);
          widget_set_navigation_down(obj_notes,   widget_down);
        }
      else
        {
          widget_set_navigation_down(obj_stand, widget_down);
          if(obj_latest != NULL)
            widget_set_navigation_down(obj_latest, widget_down);
        }
    }
}
