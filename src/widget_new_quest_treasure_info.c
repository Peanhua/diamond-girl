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
#include "gfx.h"
#include "globals.h"
#include "image.h"
#include "quest.h"
#include "td_object.h"
#include "trader.h"
#include "treasure.h"
#include "widget.h"
#include <assert.h>
#include <libintl.h>

struct widget * widget_new_quest_treasure_info(struct widget * parent, int x, int y, struct quest * quest, bool lighter)
{
  struct widget * info;
  
  info = widget_new_frame(parent, x, y, 350, 0);
  widget_delete_flags(info, WF_CAST_SHADOW);
  if(lighter == true)
    widget_add_flags(info, WF_LIGHTER);

  struct widget * imageobj;

  imageobj = NULL;
#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      struct td_object * tdobj;

      tdobj = gfx_td_object_treasure(quest->treasure_object);
      if(tdobj != NULL)
        {
          imageobj = widget_new_3dobject(info, 5, 5, 140, 140, tdobj);
          widget_delete_flags(imageobj, WF_DRAW_BORDERS);
        }
    }
  else
#endif
    {
      struct image * image;
      double w, h;
      
      image = gfx_image_treasure(quest->treasure_object, false);
      
      w = image->width;
      h = image->height;
      max_image_size(&w, 140, &h, 100);
      imageobj = widget_new_frame(info, 5, 5, w, h);
      widget_set_image_pointer(imageobj, "raw_image", image);
      widget_delete_flags(imageobj, WF_CAST_SHADOW | WF_DRAW_BORDERS);
      widget_add_flags(imageobj, WF_SCALE_RAW_IMAGE);
    }

  assert(imageobj != NULL);
  if(imageobj != NULL)
    {
      if(lighter == true)
        widget_add_flags(imageobj, WF_LIGHTER);
        
      char buf[1024];
      struct widget * headerobj;

      x = widget_x(imageobj) + widget_width(imageobj) + 5;
      snprintf(buf, sizeof buf, gettext("'%s'"), quest->treasure_object->name);
      buf[0] = toupper(buf[0]);
      headerobj = widget_new_text_area(info, x, 5, widget_width(info) - x - 5, 10, buf, true);
      widget_set_y(headerobj, widget_y(imageobj) + (widget_height(imageobj) - widget_height(headerobj)) / 2);
      if(lighter == true)
        widget_add_flags(headerobj, WF_LIGHTER);
        
      char dbuf[128];
      char tbuf[128];
      struct tm * tm;

      snprintf(buf, sizeof buf, "%s.\n", treasure_longname(quest->treasure_object));
      buf[0] = toupper(buf[0]);

      tm = localtime(&quest->completed);
      strftime(dbuf, sizeof dbuf, gettext("%Y-%m-%d"), tm);
      strftime(tbuf, sizeof tbuf, gettext("%H:%M"), tm);
      switch(quest->completion_id)
        {
        case 0:
          snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext("Obtained on %s at %s from %s level %u."),
                   dbuf,
                   tbuf,
                   cave_displayname(quest->treasure_cave),
                   quest->treasure_level);
          break;
        default:
          assert(quest->completion_id - 1 < MAX_TRADERS);
          if(quest->completion_id - 1 < MAX_TRADERS)
            {
              struct trader * trader;
              
              trader = globals.traders[quest->completion_id - 1];
              assert(trader != NULL);
              snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext("Bought from %s on %s at %s."),
                       trader->name,
                       dbuf,
                       tbuf);
            }
          break;
        }
      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext("Price: %u"), (unsigned int) treasure_price(quest->treasure_object));
      snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
      

      if(quest->treasure_sold == true)
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "%s\n", gettext("You have sold this item."));
      
      {
        unsigned int total_count;
        unsigned int type_count;
        unsigned int material_count;
        unsigned int material_type_count;
        struct treasure * currobj;

        currobj = quest->treasure_object;
        total_count = type_count = material_count = material_type_count = 0;
        
        for(unsigned int i = 0; i < globals.questlines_size; i++)
          {
            struct questline * ql;

            ql = globals.questlines[i];
            for(unsigned int j = 0; j <= ql->current_quest; j++)
              {
                struct quest * q;
                
                q = ql->quests->data[j];
                if(q->completed > 0 && q->treasure_sold == false)
                  {
                    struct treasure * obj;
                    
                    obj = q->treasure_object;

                    total_count++;
                    
                    if(obj->type == currobj->type)
                      {
                        type_count++;
                        if(obj->material == currobj->material)
                          material_type_count++;
                      }
                    if(obj->material == currobj->material)
                      material_count++;
                  }
              }
          }

        char * p;
        
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), gettext("All items: %u"), total_count);
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
        
        p = buf + strlen(buf);
        snprintf(p, sizeof buf - strlen(buf), gettext("%s items: %u"), treasure_type_name(currobj->type), type_count);
        *p = toupper(*p);
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
        
        p = buf + strlen(buf);
        snprintf(p, sizeof buf - strlen(buf), gettext("%s items: %u"), treasure_material_name(currobj->material), material_count);
        *p = toupper(*p);
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
        
        p = buf + strlen(buf);
        snprintf(p, sizeof buf - strlen(buf), gettext("%s %s items: %u"), treasure_material_name(currobj->material), treasure_type_name(currobj->type), material_type_count);
        *p = toupper(*p);
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\n");
      }

      y = widget_y(imageobj) + widget_height(imageobj);
      if(widget_y(headerobj) + widget_height(headerobj) > y)
        y = widget_y(headerobj) + widget_height(headerobj);
      y += 5;

      struct widget * desc;
  
      desc = widget_new_text_area(info, 5, y, widget_width(info) - 5 * 2, 10, buf, false);
      if(lighter == true)
        widget_add_flags(desc, WF_LIGHTER);
      widget_resize_to_fit_children(desc);
      y += widget_height(desc) + 5;

      widget_set_height(info, y);
    }

  return info;
}
