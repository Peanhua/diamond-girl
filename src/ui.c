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

#include "event.h"
#include "gc.h"
#include "gfx_image.h"
#include "image.h"
#include "stack.h"
#include "ui.h"
#include "widget.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

struct ui_state
{
  struct widget * root;
  struct widget * focused;
  struct widget * modal_object;
  struct stack *  previous_modal_objects;
};

static struct ui_state current;
static struct stack * ui_states;

int ui_initialize(void)
{
  current.root         = NULL;
  current.focused      = NULL;
  current.modal_object = NULL;
  current.previous_modal_objects = stack_new();

  ui_clear_handlers();

  /* Eat away stray events. */
  SDL_Event e;

  while(SDL_PeepEvents(&e, 1, SDL_GETEVENT, ~SDL_QUIT) == 1)
    ;

  return 1;
}

void ui_cleanup(void)
{
  ui_clear_handlers();

  if(current.root != NULL)
    current.root = widget_delete(current.root);
  current.focused      = NULL;
  current.modal_object = NULL;
  current.previous_modal_objects = stack_free(current.previous_modal_objects);
}


void ui_state_push(void)
{
  ui_push_handlers();

  if(ui_states == NULL)
    ui_states = stack_new();

  struct ui_state * new;

  new = malloc(sizeof(struct ui_state));
  assert(new != NULL);
  if(new != NULL)
    {
      *new = current;
      stack_push(ui_states, new);
    }

  ui_initialize();
}


void ui_state_pop()
{
  ui_cleanup();
  ui_pop_handlers();

  struct ui_state * state;

  state = stack_pop(ui_states);
  current = *state;
  free(state);
  
  if(ui_states->size == 0)
    ui_states = stack_free(ui_states);

}





struct widget * widget_root(void)
{
  return current.root;
}


struct widget * widget_set_root(struct widget * widget)
{
  current.root = widget;
  if(current.root == NULL)
    {
      current.focused      = NULL;
      current.modal_object = NULL;
      stack_free(current.previous_modal_objects);
      current.previous_modal_objects = stack_new();
    }

  return current.root;
}

struct widget * widget_new_root(int x, int y, int width, int height)
{
  struct widget * obj;

  obj = widget_new(x, y, width, height);
  if(obj != NULL)
    {
      assert(current.root == NULL);
      /*
      if(current.root != NULL)
	{
	  assert(current.root->up_ == NULL);
	  current.root->up_ = obj;
	}
      obj->down_ = current.root;
      */
      current.root = obj;
    }
  
  return obj;
}




struct widget * widget_delete(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);

  /* Remove event callbacks using this as user_data. */
  event_unregister_all(EVENT_TYPE_SIZEOF_, object);

  /* Remove object from its parent. */
  if(object->parent_ != NULL)
    {
      bool found;

      found = false;
      for(int i = 0; found == false && i < object->parent_->children_count_; i++)
        if(object->parent_->children_[i] == object)
          {
            object->parent_->children_[i] = NULL;
            found = true;
          }
      object->parent_ = NULL;
    }

  if(object == current.root)
    current.root = NULL;
	
  if(object == current.focused)
    current.focused = NULL;

  widget_unset_modal(object);

  /* Unload is called only now that the this object is taken out of the "world".
   * This makes it possible to do ui_cleanup() from within an unload callback,
   * but the ability to find this or its children by name starting from root is gone.
   */
  if(object->on_unload_ != NULL)
    object->on_unload_(object);

  /* Delete children. */
  if(object->tooltip_.widget != NULL)
    object->tooltip_.widget = widget_delete(object->tooltip_.widget);
  for(int i = 0; i < object->children_count_; i++)
    if(object->children_[i] != NULL)
      object->children_[i] = widget_delete(object->children_[i]);
  free(object->children_);

  /* Free raw_image user pointer if the flag is set. */
  if(widget_flags(object) & WF_FREE_RAW_IMAGE)
    {
      struct image * img;

      img = widget_get_image_pointer(object, "raw_image");
      if(img != NULL)
        image_free(img);
    }

  { /* Unlink from navigation. */
    struct widget * other;

    do
      {
        other = stack_pop(object->widgets_linking_to_this);
        if(other != NULL)
          {
            if(other->navigation_right_ == object)
              other->navigation_right_ = NULL;

            if(other->navigation_left_ == object)
              other->navigation_left_ = NULL;

            if(other->navigation_down_ == object)
              other->navigation_down_ = NULL;
            
            if(other->navigation_up_ == object)
              other->navigation_up_ = NULL;

            bool found;

            do
              {
                found = false;

                for(unsigned int i = 0; found == false && i < other->widgets_linking_to_this->size; i++)
                  if(other->widgets_linking_to_this->data[i] == object)
                    {
                      found = true;
                      stack_pull(other->widgets_linking_to_this, object);
                    }
              } while(found == true);
          }
      } while(other != NULL);

    object->widgets_linking_to_this = stack_free(object->widgets_linking_to_this);
  }

  object->deleted_ = true;

  return NULL;
}

/* getters */
struct widget * widget_focus(void)
{
  struct widget * rv;

  rv = NULL;
  if(current.focused != NULL)
    if(current.focused->state_ == WIDGET_STATE_FOCUSED || current.focused->state_ == WIDGET_STATE_ACTIVATED)
      if((widget_flags(current.focused) & WF_HIDDEN) == 0)
        rv = current.focused;

  return rv;
}

enum WIDGET_STATE widget_state(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);

  return object->state_;
}

int widget_x(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);

  return object->x_;
}

int widget_y(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);

  return object->y_;
}

int widget_absolute_x(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  int x;
  
  x = object->x_;
  if(object->parent_ != NULL)
    x += widget_absolute_x(object->parent_);

  return x;
}

int widget_absolute_y(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  int y;
  
  y = object->y_;
  if(object->parent_ != NULL)
    y += widget_absolute_y(object->parent_);

  return y;
}

int widget_width(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->width_;
}

int widget_height(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->height_;
}

enum GFX_IMAGE widget_get_image(struct widget * object, const char * name)
{
  assert(object != NULL && object->deleted_ == false);
  enum GFX_IMAGE rv;

  rv = GFX_IMAGE_SIZEOF_;
  for(int i = 0; i < object->user_data_size_ && rv == GFX_IMAGE_SIZEOF_; i++)
    if(!strcmp(object->user_data_names_[i], name))
      rv = *( (enum GFX_IMAGE *)object->user_data_[i] );

  return rv;
}


ui_func_on_draw_t widget_on_draw(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_draw_;
}

ui_func_on_focus_t widget_on_focus(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_focus_;
}

ui_func_on_activate_t widget_on_activate(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_activate_;
}

ui_func_on_activate_at_t widget_on_activate_at(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_activate_at_;
}

ui_func_on_activate_t widget_on_release(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_release_;
}

ui_func_on_mouse_move_t widget_on_mouse_move(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_mouse_move_;
}

ui_func_on_unload_t widget_on_unload(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_unload_;
}

ui_func_on_timeout_t widget_on_timeout(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->on_timeout_;
}

struct widget * widget_parent(struct widget * object)
{
  assert(object != NULL && object->deleted_ == false);
  return object->parent_;
}

/* setters */
void widget_set_focus(struct widget * object)
{
  assert(object != NULL);

  if(object->deleted_ == false)
    {
      if(current.focused != NULL && current.focused != object)
        {
          if(current.focused->tooltip_.widget != NULL && (widget_flags(current.focused->tooltip_.widget) & WF_HIDDEN) == 0)
            widget_add_flags(current.focused->tooltip_.widget, WF_HIDDEN);

          if(current.focused->state_ == WIDGET_STATE_FOCUSED || current.focused->state_ == WIDGET_STATE_ACTIVATED)
            current.focused->state_ = WIDGET_STATE_NORMAL;
        }

      current.focused = object;
  
      if(current.focused != NULL)
        {
          current.focused->state_ = WIDGET_STATE_FOCUSED;
          if(current.focused->on_focus_ != NULL)
            current.focused->on_focus_(current.focused);
        }
    }
}

void widget_set_activated(struct widget * widget)
{
  assert(widget != NULL && widget->deleted_ == false);
  if(current.focused == widget && widget->state_ == WIDGET_STATE_FOCUSED)
    widget->state_ = WIDGET_STATE_ACTIVATED;
}

int widget_set_x(struct widget * object, int x)
{
  assert(object != NULL);
  assert(object->deleted_ == false);

  object->x_ = x;

  return widget_x(object);
}

int widget_set_y(struct widget * object, int y)
{
  assert(object != NULL);
  assert(object->deleted_ == false);

  object->y_ = y;

  return widget_y(object);
}

int widget_set_z(struct widget * object, int z)
{
  assert(object != NULL);
  assert(object->deleted_ == false);

  int change;

  change = z - object->z_;
  object->z_ = z;

  for(int i = 0; i < object->children_count_; i++)
    if(object->children_[i] != NULL)
      widget_set_z(object->children_[i], object->children_[i]->z_ + change);
  
  return object->z_;
}


int widget_set_width(struct widget * object, int width)
{
  assert(object != NULL && object->deleted_ == false);

  enum WIDGET_TYPE type;

  type = widget_get_ulong(object, "type");
  if(type == WT_WINDOW)
    {
      struct widget * closebutton;

      closebutton = widget_get_widget_pointer(object, "closebutton");
      if(closebutton != NULL)
        {
          int wchange;

          wchange = width - widget_width(object);
          widget_set_x(closebutton, widget_x(closebutton) + wchange);
        }
    }
  
  object->width_ = width;

  return widget_width(object);
}

int widget_set_height(struct widget * object, int height)
{
  assert(object != NULL && object->deleted_ == false);
  object->height_ = height;

  return widget_height(object);
}

void widget_to_image_size(struct widget * widget, struct image * image)
{
  assert(widget != NULL && widget->deleted_ == false);
  if(image != NULL)
    {
      int w, h;

      w = image->content_width;
      h = image->content_height;
      if(widget->flags_ & WF_DRAW_BORDERS)
        {
          w += 2;
          h += 2;
        }
      widget_set_width(widget, w);
      widget_set_height(widget, h);
    }
}




ui_func_on_draw_t widget_set_on_draw(struct widget * object, ui_func_on_draw_t on_draw)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_draw_ = on_draw;

  return widget_on_draw(object);
}

ui_func_on_focus_t widget_set_on_focus(struct widget * object, ui_func_on_focus_t on_focus)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_focus_ = on_focus;

  return widget_on_focus(object);
}

ui_func_on_activate_t widget_set_on_activate(struct widget * object, ui_func_on_activate_t activate_function)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_activate_ = activate_function;

  widget_add_flags(object, WF_FOCUSABLE);

  return widget_on_activate(object);
}

ui_func_on_activate_at_t widget_set_on_activate_at(struct widget * object, ui_func_on_activate_at_t on_activate_at)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_activate_at_ = on_activate_at;
  return widget_on_activate_at(object);
}

ui_func_on_activate_t widget_set_on_release(struct widget * object, ui_func_on_activate_t on_release)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_release_ = on_release;

  widget_add_flags(object, WF_FOCUSABLE);

  return widget_on_release(object);
}

ui_func_on_mouse_move_t widget_set_on_mouse_move(struct widget * object, ui_func_on_mouse_move_t on_mouse_move)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_mouse_move_ = on_mouse_move;
  return widget_on_mouse_move(object);
}

ui_func_on_unload_t widget_set_on_unload(struct widget * object, ui_func_on_unload_t on_unload)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_unload_ = on_unload;
  return widget_on_unload(object);
}

ui_func_on_timeout_t widget_set_on_timeout(struct widget * object, ui_func_on_timeout_t on_timeout, unsigned int delay)
{
  assert(object != NULL && object->deleted_ == false);
  object->on_timeout_ = on_timeout;
  object->timeout_timer_ = delay;
  return widget_on_timeout(object);
}




void widget_set_modal(struct widget * widget)
{
  assert(widget == NULL || widget->deleted_ == false);
  
  if(widget == NULL)
    {
      current.modal_object = stack_pop(current.previous_modal_objects);
    }
  else
    {
      assert(current.modal_object != widget); // todo: Should also check through current.previous_modal_objects
      
      if(current.modal_object != NULL)
        stack_push(current.previous_modal_objects, current.modal_object);
      current.modal_object = widget;
    }
}

void widget_unset_modal(struct widget * widget)
{
  if(current.modal_object == widget)
    widget_set_modal(NULL);
  else
    stack_pull(current.previous_modal_objects, widget);
}


struct widget * widget_modal(void)
{
  return current.modal_object;
}
