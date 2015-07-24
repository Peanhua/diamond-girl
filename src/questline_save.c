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
#include <errno.h>

static unsigned char * buf;
static unsigned int    bufpos;
static unsigned int    bufsize;

static bool add(void * data, unsigned int len);
static bool addstring(char * data);
static bool quest_save(struct quest * quest);
static char * diary_entry_save(struct diary_entry * entry);


bool questline_save(void)
{
  bool success;

  bufpos = 0;
  bufsize = 1024 * 16;
  buf = malloc(bufsize);
  assert(buf != NULL);
  if(buf != NULL)
    {
      if(globals.questlines_size > 0)
        {
          uint32_t tlen;

          success = addstring(QUESTLINE_SAVEFILE_V1);

          if(success == true)
            success = add(&globals.active_questline, sizeof globals.active_questline);

          if(success == true)
            {
              tlen = globals.questlines_size;
              success = add(&tlen, sizeof tlen);
            }
          
          for(unsigned int i = 0; success == true && i < globals.questlines_size; i++)
            {
              struct questline * ql;
          
              ql = globals.questlines[i];
              if(ql != NULL)
                {
                  if(success == true) success = add(&ql->type,                                sizeof ql->type);
                  if(success == true) success = add(&ql->opening_weekday,                     sizeof ql->opening_weekday);
                  if(success == true) success = add(&ql->first_questgiver.relation_to_player, sizeof ql->first_questgiver.relation_to_player);
                  if(success == true) success = addstring(ql->first_questgiver.name);
                  if(success == true) success = add(&ql->first_questgiver.gender,             sizeof ql->first_questgiver.gender);
                  if(success == true) success = add(&ql->ancient_person.relation_to_player,   sizeof ql->ancient_person.relation_to_player);
                  if(success == true) success = addstring(ql->ancient_person.name);
                  if(success == true) success = add(&ql->ancient_person.gender,               sizeof ql->ancient_person.gender);
                  if(success == true) success = add(&ql->reward,                              sizeof ql->reward);
                  if(success == true) success = add(&ql->current_quest,                       sizeof ql->current_quest);
                  if(success == true) success = add(&ql->current_phase,                       sizeof ql->current_phase);

                  tlen = ql->quests->size;
                  if(success == true) success = add(&tlen, sizeof tlen);
                  for(unsigned int j = 0; success == true && j < ql->quests->size; j++)
                    success = quest_save(ql->quests->data[j]);

                  tlen = ql->diary_entries->size;
                  if(success == true) success = add(&tlen, sizeof tlen);
                  for(unsigned int j = 0; success == true && j < ql->diary_entries->size; j++)
                    {
                      struct diary_entry * e;
                      char * str;
                      
                      e = ql->diary_entries->data[j];
                      str = diary_entry_save(e);
                      if(str != NULL)
                        {
                          success = addstring(str);
                          free(str);
                        }
                      else
                        success = false;
                    }
                }
            }

          if(success == true) success = add(&globals.active_trader,       sizeof globals.active_trader);
          if(success == true) success = add(&globals.active_trader_start, sizeof globals.active_trader_start);
          
          for(int i = 0; success == true && i < MAX_TRADERS; i++)
            {
              char * str;
              
              str = trader_save(globals.traders[i]);
              if(str != NULL)
                {
                  success = addstring(str);
                  free(str);
                }
              else
                success = false;
            }
          
          if(success == true)
            {
              success = write_file(get_save_filename("questlines"), (char *) buf, bufpos);
            }
          assert(success == true);
  
        }
      else
        success = false;
    }
  else
    {
      fprintf(stderr, "%s: Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
      success = false;
    }

  return success;
}


static bool add(void * data, unsigned int len)
{
  if(bufpos + len >= bufsize)
    {
      unsigned char * tmp;

      tmp = realloc(buf, bufsize * 2);
      if(tmp != NULL)
        {
          buf = tmp;
          bufsize *= 2;
        }
    }

  bool rv;
  
  if(bufpos + len < bufsize)
    {
      memcpy(buf + bufpos, data, len);
      bufpos += len;
      rv = true;
    }
  else
    rv = false;

  return rv;
}


static bool addstring(char * data)
{
  uint32_t len;
  bool success;

  assert(data != NULL);
  len = strlen(data);
  success = add(&len, sizeof len);
  
  if(success == true && len > 0)
    success = add(data, len);

  return success;
}




static bool quest_save(struct quest * quest)
{
  bool success;

  success = add(&quest->action_to_open, sizeof quest->action_to_open);
  if(success == true) success = add(&quest->opened,            sizeof quest->opened);
  if(success == true) success = add(&quest->completed,         sizeof quest->completed);
  if(success == true) success = add(&quest->description_id,    sizeof quest->description_id);
  if(success == true) success = add(&quest->completion_id,     sizeof quest->completion_id);
  if(success == true) success = add(&quest->hours_to_salesman, sizeof quest->hours_to_salesman);
  if(success == true) success = addstring(quest->treasure_cave);
  if(success == true) success = add(&quest->treasure_level,    sizeof quest->treasure_level);
  if(success == true) success = add(&quest->treasure_x,        sizeof quest->treasure_x);
  if(success == true) success = add(&quest->treasure_y,        sizeof quest->treasure_y);
  
  if(success == true) success = add(&quest->treasure_object->type,      sizeof quest->treasure_object->type);
  if(success == true) success = add(&quest->treasure_object->material,  sizeof quest->treasure_object->material);
  if(success == true) success = add(&quest->treasure_object->gemstones, sizeof quest->treasure_object->gemstones);
  if(success == true) success = addstring(quest->treasure_object->name);
  if(success == true) success = add(&quest->treasure_sold,              sizeof quest->treasure_sold);

  return success;
}

static char * diary_entry_save(struct diary_entry * entry)
{
  char str[1024];

  snprintf(str, sizeof str, "%u %u %u %u %s",
           (unsigned) entry->timestamp,
           (unsigned) entry->image_source.type,
           (unsigned) entry->image_source.material,
           (unsigned) entry->image_source.gemstones,
           entry->entry);

  return strdup(str);
}
