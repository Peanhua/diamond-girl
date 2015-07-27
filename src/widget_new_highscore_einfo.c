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
#include "cave.h"
#include "widget.h"
#include "highscore.h"
#include "title.h"
#include "sfx.h"
#include "map.h"
#include "playback.h"
#include "gfx.h"
#include <assert.h>
#include <libintl.h>

static struct widget * window;

static void on_replay_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_unload(struct widget * this);
static void on_window_close_clicked(struct widget * this, enum WIDGET_BUTTON button);
static void on_window_exit(bool pressed, SDL_Event * event);

struct widget * widget_new_highscore_einfo(struct widget * parent, struct highscore_entry * entry)
{
  int x[2], y;

  if(font_width(gettext("Filename:")) > 100)
    window = widget_new_window(parent, 460, 280, NULL);
  else
    window = widget_new_window(parent, 400, 280, NULL);
  assert(window != NULL);

  x[0] = 10;
  x[1] = x[0] + widget_width(window) / 2;
  y = 20;

  struct tm * tm;
  int deaths;
  int time_played;
  int diamonds;
  int start_level;
  int levels_completed;
  struct playback * playback;
  char tbuf[64];
  int col, labelwidth;

  labelwidth = font_width(gettext("Filename:")) + 10;
  if(labelwidth < 110)
    labelwidth = 110;

  deaths           = 0;
  time_played      = 0;
  diamonds         = entry->diamonds_collected;
  start_level      = 0;
  levels_completed = 0;
  playback         = highscore_playback(entry, globals.title_game_mode);

  if(playback != NULL)
    {
      deaths           = playback_get_girls_died(playback);
      start_level      = playback->level;
      levels_completed = playback_get_levels_completed(playback);
      time_played      = playback_get_time_played(playback);
    }

  col = 0;
  tm = localtime(&entry->timestamp);
  widget_new_text(window, x[col], y, gettext("Played on:"));
  strftime(tbuf, sizeof tbuf, gettext("%Y-%m-%d %H:%M"), tm);
  widget_new_text(window, x[col]  + labelwidth, y, tbuf);

  y += font_height();

  col = 0;
  widget_new_text(window, x[col], y, gettext("Cave:"));
  snprintf(tbuf, sizeof tbuf, "%s", cave_displayname(entry->cave));
  widget_new_text(window, x[col] + labelwidth, y, tbuf);

  y += font_height();

  col = 0;
  widget_new_text(window, x[col], y, gettext("Mode:"));
  switch(globals.title_game_mode)
    {
    case GAME_MODE_CLASSIC:      strcpy(tbuf, gettext("Classic"));      break;
    case GAME_MODE_ADVENTURE:    strcpy(tbuf, gettext("Adventure"));    break;
    case GAME_MODE_PYJAMA_PARTY: strcpy(tbuf, gettext("Pyjama Party")); break;
    }
  widget_new_text(window, x[col] + labelwidth, y, tbuf);

  y += font_height();

  if(globals.title_game_mode != GAME_MODE_PYJAMA_PARTY)
    {
      col = 0;
      widget_new_text(window, x[col], y, gettext("Filename:"));
      if(playback != NULL)
        snprintf(tbuf, sizeof tbuf, "highscores%s-%d", cave_filename(playback->cave), entry->playback_id);
      else
        strcpy(tbuf, gettext("N/A"));
      widget_new_text(window, x[col] + labelwidth, y, tbuf);
  
      y += font_height();
    }

  {
    int hours, minutes, seconds;


    hours = time_played / (60 * 60);
    minutes = (time_played / 60) % 60;
    seconds = time_played % 60;
    col = 0;
    widget_new_text(window, x[col], y, gettext("Length:"));
    if(hours > 0)
      {
        snprintf(tbuf, sizeof tbuf, ngettext("1 hours", "%d hours", hours), hours);
        snprintf(tbuf + strlen(tbuf), sizeof tbuf - strlen(tbuf), ", ");
        snprintf(tbuf + strlen(tbuf), sizeof tbuf - strlen(tbuf), ngettext("1 minute", "%d minutes", minutes), minutes);
      }
    else if(minutes > 0)
      {
        snprintf(tbuf, sizeof tbuf, ngettext("1 minute", "%d minutes", minutes), minutes);
        snprintf(tbuf + strlen(tbuf), sizeof tbuf - strlen(tbuf), ", ");
        snprintf(tbuf + strlen(tbuf), sizeof tbuf - strlen(tbuf), ngettext("1 second", "%d seconds", seconds), seconds);
      }
    else
      snprintf(tbuf, sizeof tbuf, ngettext("1 second", "%d seconds", seconds), seconds);

    widget_new_text(window, x[col] + labelwidth, y, tbuf);
  
    y += font_height();
  }
  
  y += font_height() / 2;

  
  int bottomlabelwidths[2];

  bottomlabelwidths[0] = 110;
  
  bottomlabelwidths[1] = font_width(gettext("Start level:")) + 10;
  if(bottomlabelwidths[1] < 110)
    bottomlabelwidths[1] = 110;
  
  col = 0;
  widget_new_text(window, x[col], y, gettext("Score:"));
  snprintf(tbuf, sizeof tbuf, "%6d", entry->score);
  widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);

  col = 1;
  widget_new_text(window, x[col], y, gettext("Start level:"));
  if(start_level > 0)
    snprintf(tbuf, sizeof tbuf, "%2d", start_level);
  else
    snprintf(tbuf, sizeof tbuf, "%2s", gettext("N/A"));
  widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);

  y += font_height();

  col = 0;
  widget_new_text(window, x[col], y, gettext("Diamonds:"));
  snprintf(tbuf, sizeof tbuf, "%6d", diamonds); /* For pre-v3 highscorelists this shows 0 because there's no data. */
  widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);

  col = 1;
  widget_new_text(window, x[col], y, gettext("End level:"));
  snprintf(tbuf, sizeof tbuf, "%2d", entry->level); 
  widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);

  y += font_height();

  col = 0;
  if(globals.title_game_mode == GAME_MODE_PYJAMA_PARTY)
    {
      widget_new_text(window, x[col], y, gettext("Girls:"));
      snprintf(tbuf, sizeof tbuf, "%6d", entry->starting_girls);
      widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);
    }
  else
    {
      widget_new_text(window, x[col], y, gettext("Deaths:"));
      if(deaths > 0)
        snprintf(tbuf, sizeof tbuf, "%6d", deaths);
      else
        snprintf(tbuf, sizeof tbuf, "%6s", gettext("N/A"));
      widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);
    }

  col = 1;
  widget_new_text(window, x[col], y, gettext("Completed:"));
  if(start_level > 0)
    snprintf(tbuf, sizeof tbuf, "%2d", levels_completed);
  else
    snprintf(tbuf, sizeof tbuf, "%2s", gettext("N/A"));
  widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);

  y += font_height();


  if(entry->diamond_score > 0)
    {
      col = 0;
      widget_new_text(window, x[col], y, gettext("Diam.score:"));
      snprintf(tbuf, sizeof tbuf, "%6d", entry->diamond_score);
      widget_new_text(window, x[col] + bottomlabelwidths[col], y, tbuf);

      y += font_height();
    }

  if(entry->traits)
    {
      col = 0;
      widget_new_text(window, x[col], y, gettext("Traits:"));

      //obj = widget_new_trait_list(x[col] + 10, y, widget_width(window) - (x[col] + 10), entry->traits, entry->traits);
      int xpos, size, padding;
      
      xpos = 0;
      size = 24;
      padding = 2;
      for(int i = 0; i < TRAIT_SIZEOF_; i++)
        {
          trait_t trait;

          trait = traits_sorted[i];
          if(entry->traits & trait)
            {
              struct widget * obj;
              char * name;

              obj = widget_new_trait_button(window, x[col] + bottomlabelwidths[col] + xpos * (size + padding), y, size, size, trait, true, false);
              name = trait_get_name(trait);
              if(name != NULL)
                widget_set_tooltip(obj, name);

              xpos++;
              if(xpos >= (250 / (size + padding)))
                {
                  xpos = 0;
                  y += size + padding;
                }
            }
        }
      
      if(xpos > 0)
        y += size + padding;
    }

  if(strlen(entry->notes) > 0)
    {
      y += font_height() / 2;
      col = 0;
      widget_new_text(window, x[col], y, gettext("Notes:"));
      y += font_height();
      widget_new_text(window, x[col] + 10, y, entry->notes);
      y += font_height();
    }

  y += font_height();

  struct widget * breplay, * bclose;

  breplay = NULL;
  if(globals.title_game_mode != GAME_MODE_PYJAMA_PARTY)
    {
      breplay = widget_new_button(window, 0, y, gettext("Replay"));
      widget_set_width(breplay, 100);
      widget_set_x(breplay, (widget_width(window) - widget_width(breplay)) / 2);
      widget_set_on_release(breplay, on_replay_clicked);
      widget_set_pointer(breplay, "highscore_entry", 'P', entry);
      if(playback == NULL)
        widget_set_enabled(breplay, false);
      y += widget_height(breplay) + 4;
    }

  bclose = widget_new_button(window, 0, y, gettext("Close"));
  widget_set_width(bclose, 100);
  widget_set_x(bclose, (widget_width(window) - widget_width(bclose)) / 2);
  widget_set_on_release(bclose, on_window_close_clicked);
  if(breplay != NULL)
    widget_set_navigation_updown(breplay, bclose);
  y += widget_height(bclose);
  
  widget_set_widget_pointer(window, "previously_selected_object", widget_focus());
  widget_set_focus(bclose);

  widget_set_height(window, y + 10);


  /* This is a modal window. */
  widget_set_modal(window);
  /* Replace cancel and exit handlers. */
  ui_push_handlers();
  ui_set_handler(UIC_EXIT,   on_window_exit);
  ui_set_handler(UIC_CANCEL, on_window_exit);
  widget_set_on_unload(window, on_window_unload);

  return window;
}


static void on_window_unload(struct widget * this DG_UNUSED)
{
  ui_pop_handlers();
}

static void on_window_exit(bool pressed, SDL_Event * event DG_UNUSED)
{
  if(pressed == true)
    {
      struct widget * w;

      w = widget_get_widget_pointer(window, "previously_selected_object");

      widget_delete(window);

      if(w != NULL)
        widget_set_focus(w);
    }
}

static void on_window_close_clicked(struct widget * this DG_UNUSED, enum WIDGET_BUTTON button DG_UNUSED)
{
  on_window_exit(true, NULL);
}




static void on_replay_clicked(struct widget * this, enum WIDGET_BUTTON button DG_UNUSED)
{
  struct highscore_entry * entry;
  struct playback * playback;

  entry = widget_get_pointer(this, "highscore_entry", 'P');
  assert(entry != NULL);

  playback = highscore_playback(entry, globals.title_game_mode);
  if(playback != NULL)
    {
      char * pberr;

      pberr = playback_check(playback);
      if(pberr == NULL)
        {
          char playback_title[64];
          struct cave * cave;
          char * cave_fn;
          
          cave = cave_get(globals.title_game_mode, entry->cave);
          cave_fn = cave_filename(entry->cave);
          while(*cave_fn == '-') /* Skip leading "-" of the cave filename. */
            cave_fn++;
          if(strlen(cave_fn) > 0)
            snprintf(playback_title, sizeof playback_title, "%s-", cave_fn);
          else
            strcpy(playback_title, "");
          snprintf(playback_title + strlen(playback_title), sizeof playback_title - strlen(playback_title), "%d", entry->playback_id);
          title_game(cave, playback, playback_title);
          
          sfx_volume(0.05f);
          sfx_music(MUSIC_TITLE, 1);
          ui_set_last_user_action(time(NULL));
        }
      else
        {
          widget_new_message_window("ERROR", NULL, pberr);
          free(pberr);
        }
    }
}
