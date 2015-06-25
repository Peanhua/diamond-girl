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

#ifndef WIDGET_H_
#define WIDGET_H_

#include "ui.h"
#include "traits.h"
#include "stack.h"

#include <stdlib.h>
#include <stdbool.h>

/* Forward declarations */
struct image;
struct stack;
struct theme;
struct highscore_entry;
enum GFX_IMAGE;
struct cave;


/* The state of the widget */
enum WIDGET_STATE
  {
    WIDGET_STATE_NORMAL,
    WIDGET_STATE_FOCUSED,
    WIDGET_STATE_ACTIVATED
  };

/* High level widget types */
enum WIDGET_TYPE
  {
    WT_FRAME,
    WT_WINDOW,
    WT_BUTTON,
    WT_CHECKBOX,
    WT_TEXT,
    WT_MAP,
    WT_TITLE_HELP,
    WT_TITLE_HIGHSCORES,
    WT_TITLE_CREDITS,
    WT_SELECT,
    WT_TOOLTIP,
    WT_SLIDER,
    WT_LIST,
    WT_GRID,
    WT_GRID_ROW
  };

/* Widget flags */
#define WF_ALIGN_CENTER      (1<< 0)
#define WF_ALIGN_IMAGE_RIGHT (1<< 1)
#define WF_CAST_SHADOW       (1<< 2)
#define WF_DRAW_BORDERS      (1<< 3)
#define WF_BACKGROUND        (1<< 4)
#define WF_FREE_RAW_IMAGE    (1<< 5)
#define WF_FOCUSABLE         (1<< 6) /* If set, the widget can be focused. */
#define WF_HIDDEN            (1<< 7) /* If set, the widget is not drawn, recursively. The widget remains active. */
#define WF_SCALE_RAW_IMAGE   (1<< 8) /* If set, the raw_image is drawn scaled to fit. */
#define WF_DESTROY           (1<< 9) /* If set, the widget is destroyed by an automatic destroyer function at the end of ui_draw(). */
#define WF_CLIPPING          (1<<10) /* Clip drawing of this and its children from going outside of the widget. */


struct widget;
struct map;

typedef void (*ui_func_on_draw_t)(struct widget * this);
typedef void (*ui_func_on_focus_t)(struct widget * this);
typedef void (*ui_func_on_activate_t)(struct widget * this, enum WIDGET_BUTTON button);
typedef void (*ui_func_on_activate_at_t)(struct widget * this, enum WIDGET_BUTTON button, int x, int y);
typedef void (*ui_func_on_mouse_move_t)(struct widget * this, int x, int y);
typedef void (*ui_func_on_unload_t)(struct widget * this);
typedef void (*ui_func_on_select_t)(struct widget * this);

struct ui_widget_select_option
{
  char   text[64];
  void * data;
};




struct widget
{
  enum WIDGET_STATE state_;

  char ** user_data_names_;
  void ** user_data_;
  int     user_data_size_;

  int x_, y_, z_;
  int width_, height_;

  uint32_t flags_;

  ui_func_on_draw_t        on_draw_;
  ui_func_on_focus_t       on_focus_;
  ui_func_on_activate_t    on_activate_;
  ui_func_on_activate_at_t on_activate_at_;
  ui_func_on_activate_t    on_release_;
  ui_func_on_mouse_move_t  on_mouse_move_;
  ui_func_on_unload_t      on_unload_;

  struct widget *  parent_;
  struct widget ** children_;
  int              children_count_;

  /* Navigation */
  struct widget * navigation_up_;
  struct widget * navigation_left_;
  struct widget * navigation_right_;
  struct widget * navigation_down_;
  struct stack *  widgets_linking_to_this;

  /* Tooltip */
  struct
  {
    char *          text;
    struct widget * widget;
  } tooltip_;

#if defined(UI_DEBUG)
  int deleted_;
#endif
};


extern struct widget * widget_new(int x, int y, int width, int height);

#ifdef NDEBUG
# define    widget_dump(x)
#else
extern void widget_dump(struct widget * widget);
#endif

/* high level widget creation */
extern struct widget * widget_new_button(struct widget * parent, int x, int y, char const * const text);
extern struct widget * widget_new_checkbox(struct widget * parent, int x, int y, int initial_state);
extern struct widget * widget_new_frame(struct widget * parent, int x, int y, int width, int height);
extern struct widget * widget_new_game_help_window(void);
extern struct widget * widget_new_game_window(char ** lines, int lines_count);
extern struct widget * widget_new_grid(struct widget * parent, int x, int y);
extern struct widget * widget_new_grid_row(struct widget * grid, int y);
extern struct widget * widget_new_highscore_einfo(struct widget * parent, struct highscore_entry * entry);
extern struct widget * widget_new_legend(struct widget * parent, int x, int y);
extern struct widget * widget_new_list(struct widget * parent, int x, int y, int width, int height, struct stack * items);
extern struct widget * widget_new_map(struct widget * parent, int x, int y, int width, int height, struct map * map, enum GAME_MODE game_mode);
extern struct widget * widget_new_message_window(const char * title, const char * message);
extern struct widget * widget_new_osd_volume(void);
extern struct widget * widget_new_playback_controls(struct widget * parent, int x, int y, char const * const title);
extern struct widget * widget_new_select(struct widget * parent, int x, int y, int width, unsigned int selected_index, struct stack * options);
extern struct widget * widget_new_slider(struct widget * parent, int x, int y, int width, int initial_value, int min_value, int max_value);
extern struct widget * widget_new_spacer(struct widget * parent, int width, int height);
extern struct widget * widget_new_text(struct widget * parent, int x, int y, char const * const text);
extern struct widget * widget_new_theme_info_button(struct widget * parent, int x, int y, struct theme * theme);
extern struct widget * widget_new_theme_info_window(struct theme * theme);
extern struct widget * widget_new_title_credits(struct widget * parent, int x, int y, int width);
extern struct widget * widget_new_title_help(struct widget * parent, int x, int y, int width, int height);
extern struct widget * widget_new_title_highscores(struct widget * parent, int x, int y, struct cave * cave);
extern struct widget * widget_new_title_partystatus(struct widget * parent, int x, int y);
extern struct widget * widget_new_tooltip(struct widget * parent);
extern struct widget * widget_new_trait_button(struct widget * parent, int x, int y, int width, int height, trait_t trait, bool always_enabled_image, bool controls);
extern struct widget * widget_new_trait_key_button(struct widget * parent, int x, int y, int width, int height, struct cave * cave, unsigned int level);
extern struct widget * widget_new_traits_available_window(trait_t new_traits);
extern struct widget * widget_new_window(struct widget * parent, int width, int height, char const * const title);
extern void            settings(void (*settings_changed_cb)(bool gfx_restart, bool sfx_restart, bool new_opengl));

/* High level widget manipulation */
extern void     widget_center(struct widget * widget); /* Center in relative to its parent. */
extern bool     widget_enabled(struct widget * widget);
extern bool     widget_set_enabled(struct widget * widget, bool enabled);
extern uint32_t widget_flags(struct widget * widget);
extern uint32_t widget_set_flags(struct widget * widget, uint32_t flags);
extern uint32_t widget_add_flags(struct widget * widget, uint32_t flags);
extern uint32_t widget_delete_flags(struct widget * widget, uint32_t flags);
extern void     widget_set_tooltip(struct widget * widget, char * tooltip);
extern void     widget_set_no_focus(struct widget * widget, bool recursively);
extern void     widget_resize_to_fit_children(struct widget * grid_row);
extern void     widget_center_on_parent(struct widget * widget);

/* Widget specific manipulation */
extern void     widget_slider_set(struct widget * widget, int value);
extern void     widget_title_credits_set(struct widget * widget, char * text);
extern struct ui_widget_select_option * widget_new_select_option(void * data, char * text);
extern void     widget_set_select_options(struct widget * widget, struct stack * options);

/* Create/delete widgets */
extern struct widget * widget_new_root(int x, int y, int width, int height);
extern struct widget * widget_new_child(struct widget * parent, int x, int y, int width, int height);
extern struct widget * widget_delete(struct widget * widget);

/* Getters */
extern struct widget *          widget_root(void); /* Return the current root widget. */
extern struct widget *          widget_focus(void);
extern struct widget *          widget_modal(void);
extern enum WIDGET_STATE        widget_state(struct widget * widget);
extern int                      widget_x(struct widget * widget);
extern int                      widget_y(struct widget * widget);
extern int                      widget_absolute_x(struct widget * widget);
extern int                      widget_absolute_y(struct widget * widget);
extern int                      widget_width(struct widget * widget);
extern int                      widget_height(struct widget * widget);

extern struct widget *          widget_parent(struct widget * widget);
extern struct widget *          widget_find(struct widget * root, const char * id); /* Search from under "root" a widget whose ulong "id" equals to id. */

/* Callbacks */
extern ui_func_on_draw_t        widget_on_draw(struct widget * widget);
extern ui_func_on_focus_t       widget_on_focus(struct widget * widget);
extern ui_func_on_activate_t    widget_on_activate(struct widget * widget);
extern ui_func_on_activate_at_t widget_on_activate_at(struct widget * widget);
extern ui_func_on_activate_t    widget_on_release(struct widget * widget);
extern ui_func_on_mouse_move_t  widget_on_mouse_move(struct widget * widget);
extern ui_func_on_unload_t      widget_on_unload(struct widget * widget);
/* Navigation */
extern struct widget *          widget_get_navigation_left(struct widget * widget);
extern struct widget *          widget_get_navigation_right(struct widget * widget);
extern struct widget *          widget_get_navigation_up(struct widget * widget);
extern struct widget *          widget_get_navigation_down(struct widget * widget);
/* User data */
extern enum GFX_IMAGE           widget_get_image(struct widget * widget, const char * name);
extern char *                   widget_get_string(struct widget * widget, const char * name);
extern long                     widget_get_long(struct widget * widget, const char * name);
extern unsigned long            widget_get_ulong(struct widget * widget, const char * name);
extern double                   widget_get_double(struct widget * widget, const char * name);
extern void *                   widget_get_pointer(struct widget * widget, const char * name);

/* Setters */
extern struct widget *          widget_set_root(struct widget * widget);
extern void                     widget_set_focus(struct widget * widget);
extern void                     widget_set_activated(struct widget * widget);
extern void                     widget_set_modal(struct widget * widget);
extern void                     widget_unset_modal(struct widget * widget);
extern int                      widget_set_x(struct widget * widget, int x);
extern int                      widget_set_y(struct widget * widget, int y);
extern int                      widget_set_width(struct widget * widget, int width);
extern int                      widget_set_height(struct widget * widget, int height);
/* Callbacks */
extern ui_func_on_draw_t        widget_set_on_draw(struct widget * widget, ui_func_on_draw_t on_draw);
extern ui_func_on_focus_t       widget_set_on_focus(struct widget * widget, ui_func_on_focus_t on_focus);
extern ui_func_on_activate_t    widget_set_on_activate(struct widget * widget, ui_func_on_activate_t on_activate);
extern ui_func_on_activate_at_t widget_set_on_activate_at(struct widget * widget, ui_func_on_activate_at_t on_activate_at);
extern ui_func_on_activate_t    widget_set_on_release(struct widget * widget, ui_func_on_activate_t on_release);
extern ui_func_on_mouse_move_t  widget_set_on_mouse_move(struct widget * widget, ui_func_on_mouse_move_t on_mouse_move);
extern ui_func_on_unload_t      widget_set_on_unload(struct widget * widget, ui_func_on_unload_t on_unload);
/* Navigation */
extern void                     widget_set_navigation_leftright(struct widget * left, struct widget * right);
extern void                     widget_set_navigation_left(struct widget * widget, struct widget * right);
extern void                     widget_set_navigation_right(struct widget * widget, struct widget * right);
extern void                     widget_set_navigation_updown(struct widget * up, struct widget * down);
extern void                     widget_set_navigation_up(struct widget * widget, struct widget * up);
extern void                     widget_set_navigation_down(struct widget * widget, struct widget * down);
/* User data */
extern enum GFX_IMAGE           widget_set_image( struct widget * widget, const char * name, enum GFX_IMAGE user_data);
extern char *                   widget_set_string(struct widget * widget, const char * name, char const * const user_data, ...)
#ifdef __GNUC__
  __attribute__((format (printf, 3, 4)))
#endif
  ;
extern long                     widget_set_long(   struct widget * widget, const char * name, long user_data);
extern unsigned long            widget_set_ulong(  struct widget * widget, const char * name, unsigned long user_data);
extern double                   widget_set_double( struct widget * widget, const char * name, double user_data);
extern void *                   widget_set_pointer(struct widget * widget, const char * name, void * user_data);


/* misc */
extern void            widget_draw(struct widget * widget); /* The default drawing function. */

#endif

