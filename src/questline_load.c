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

#include "globals.h"
#include "quest.h"
#include "stack.h"
#include "trader.h"
#include "treasure.h"
#include <assert.h>

static bool get(void * data, unsigned int len);
static bool getstring(char ** data);
static bool quest_load(struct questline * questline);
static bool diary_entry_load(struct questline * questline, char * savestring);

static char *       buf;
static int          bufsize;
static unsigned int bufpos;


bool questline_load(void)
{
  bool success;
  
  success = read_file(get_save_filename("questlines"), &buf, &bufsize);
  if(success == true)
    {
      assert(bufsize > 0);
      bufpos = 0;

      char * version_str;
      uint32_t tlen;
      
      success = getstring(&version_str);
      if(success == true)
        {
          if(strcmp(version_str, QUESTLINE_SAVEFILE_V1))
            success = false;
        }
      free(version_str);

      if(success == true)
        success = get(&globals.active_questline, sizeof globals.active_questline);

      unsigned int qlcount;

      qlcount = 0;
      if(success == true)
        {
          success = get(&tlen, sizeof tlen);
          qlcount = tlen;
        }

      for(unsigned int i = 0; success == true && i < qlcount && globals.questlines_size < MAX_QUESTLINES; i++)
        {
          struct questline * ql;

          ql = questline_new(QUEST_TYPE_SIZEOF_, RELATION_TYPE_SIZEOF_, RELATION_TYPE_SIZEOF_);
          if(ql != NULL)
            {
              globals.questlines[globals.questlines_size] = ql;
              globals.questlines_size++;
              
              if(success == true) success = get(&ql->type,                                sizeof ql->type);
              if(success == true) success = get(&ql->opening_weekday,                     sizeof ql->opening_weekday);
              if(success == true) success = get(&ql->opening_monthday_max,                sizeof ql->opening_monthday_max);
              if(success == true) success = get(&ql->opening_time_hour,                   sizeof ql->opening_time_hour);
              if(success == true) success = get(&ql->opening_time_length,                 sizeof ql->opening_time_length);
              if(success == true) success = get(&ql->first_questgiver.relation_to_player, sizeof ql->first_questgiver.relation_to_player);
              if(success == true) success = getstring(&ql->first_questgiver.name);
              if(success == true) success = get(&ql->first_questgiver.gender,             sizeof ql->first_questgiver.gender);
              if(success == true) success = get(&ql->ancient_person.relation_to_player,   sizeof ql->ancient_person.relation_to_player);
              if(success == true) success = getstring(&ql->ancient_person.name);
              if(success == true) success = get(&ql->ancient_person.gender,               sizeof ql->ancient_person.gender);
              if(success == true) success = get(&ql->reward,                              sizeof ql->reward);
              if(success == true) success = get(&ql->current_quest,                       sizeof ql->current_quest);
              if(success == true) success = get(&ql->current_phase,                       sizeof ql->current_phase);

              if(success == true) success = get(&tlen, sizeof tlen);
              for(unsigned int j = 0; success == true && j < tlen; j++)
                success = quest_load(ql);

              if(success == true) success = get(&tlen, sizeof tlen);
              for(unsigned int j = 0; success == true && j < tlen; j++)
                {
                  char * str;
          
                  success = getstring(&str);
                  if(success == true)
                    success = diary_entry_load(ql, str);
                  free(str);
                }
            }
          else
            success = false;
        }

      if(success == true) success = get(&globals.active_trader,       sizeof globals.active_trader);
      if(success == true) success = get(&globals.active_trader_start, sizeof globals.active_trader_start);
              
      for(int i = 0; success == true && i < MAX_TRADERS; i++)
        {
          char * str;
          
          success = getstring(&str);
          if(success == true)
            {
              globals.traders[i] = trader_load(str);
              if(globals.traders[i] == NULL)
                success = false;
              free(str);
            }
        }
      
      free(buf);

      assert(success == true);
      assert((int) bufpos == bufsize);
    }
  
  return success;
}


static bool get(void * data, unsigned int len)
{
  bool rv;
  
  assert((int) (bufpos + len) <= bufsize);
  if((int) (bufpos + len) <= bufsize)
    {
      memcpy(data, buf + bufpos, len);
      bufpos += len;
      rv = true;
    }
  else
    {
      memset(data, 0, len);
      rv = false;
    }

  return rv;
}


static bool getstring(char ** data)
{
  uint32_t len;
  bool success;

  *data = NULL;
  success = get(&len, sizeof len);

  if(success == true)
    {
      *data = malloc(len + 1);
      if(*data != NULL)
        {
          success = get(*data, len);
          if(success == true)
            (*data)[len] = '\0';
        }
      else
        success = false;
    }
  
  return success;
}


static bool quest_load(struct questline * questline)
{
  bool success;
  struct quest * quest;

  quest = malloc(sizeof *quest);
  assert(quest != NULL);
  if(quest != NULL)
    {
      quest->questline = questline;
      success = get(&quest->action_to_open, sizeof quest->action_to_open);
      if(success == true) success = get(&quest->opened,            sizeof quest->opened);
      if(success == true) success = get(&quest->completed,         sizeof quest->completed);
      if(success == true) success = get(&quest->description_id,    sizeof quest->description_id);
      if(success == true) success = get(&quest->completion_id,     sizeof quest->completion_id);
      if(success == true) success = get(&quest->hours_to_salesman, sizeof quest->hours_to_salesman);
      if(success == true) success = getstring(&quest->treasure_cave);
      if(success == true) success = get(&quest->treasure_level,    sizeof quest->treasure_level);
      if(success == true) success = get(&quest->treasure_x,        sizeof quest->treasure_x);
      if(success == true) success = get(&quest->treasure_y,        sizeof quest->treasure_y);

      quest->treasure_object = malloc(sizeof *quest->treasure_object);
      assert(quest->treasure_object != NULL);
      if(quest->treasure_object != NULL)
        {
          if(success == true) success = get(&quest->treasure_object->type,      sizeof quest->treasure_object->type);
          if(success == true) success = get(&quest->treasure_object->material,  sizeof quest->treasure_object->material);
          if(success == true) success = get(&quest->treasure_object->gemstones, sizeof quest->treasure_object->gemstones);
          if(success == true) success = getstring(&quest->treasure_object->name);
        }
      else
        success = false;
      if(success == true) success = get(&quest->treasure_sold, sizeof quest->treasure_sold);

      if(success == true)
        stack_push(questline->quests, quest);
    }
  else
    success = false;

  assert(success == true);
  
  return success;
}



bool diary_entry_load(struct questline * questline, char * savestring)
{
  bool success;

  success = false;
  assert(savestring != NULL);
  if(savestring != NULL)
    {
      struct diary_entry entry;
      char * p;

      entry.timestamp = strtoul(savestring, &p, 0);
      assert(p != NULL);

      if(p != NULL)
        entry.image_source.type = strtoul(p, &p, 0);
      assert(p != NULL);

      if(p != NULL)
        entry.image_source.material = strtoul(p, &p, 0);
      assert(p != NULL);

      if(p != NULL)
        entry.image_source.gemstones = strtoul(p, &p, 0);
      assert(p != NULL);

      if(p != NULL)
        {
          struct treasure * treasure;

          while(*p == ' ')
            p++;
          entry.entry = p;
          entry.image_source.name = NULL;

          if(entry.image_source.type == TREASURE_TYPE_SIZEOF_)
            treasure = NULL;
          else
            treasure = &entry.image_source;
          
          struct diary_entry * e;

          e = questline_diary_add(questline, entry.timestamp, treasure, entry.entry);
          if(e != NULL)
            success = true;
        }
    }

  assert(success == true);

  return success;
}

struct diary_entry * questline_diary_add(struct questline * questline, time_t timestamp, struct treasure * image_source, char * entry)
{
  struct diary_entry * e;

  assert(questline != NULL);
  e = malloc(sizeof *e);
  if(e != NULL)
    {
      e->timestamp = timestamp;
      e->image = NULL;
      if(image_source == NULL)
        {
          e->image_source.type      = TREASURE_TYPE_SIZEOF_;
          e->image_source.material  = TREASURE_MATERIAL_SIZEOF_;
          e->image_source.gemstones = TREASURE_GEMSTONE_SIZEOF_;
          e->image_source.name = NULL;
          e->image = NULL;
        }
      else
        {
          e->image_source = *image_source;
          e->image_source.name = NULL;
          e->image = gfx_image_treasure(&e->image_source, true);
        }
      
      e->entry = strdup(entry);
      stack_push(questline->diary_entries, e);
    }

  return e;
}
