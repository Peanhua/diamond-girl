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
#include "map.h"
#include "quest.h"
#include "random.h"
#include "stack.h"
#include "treasure.h"
#include <assert.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

static struct quest * quest_new(enum QUEST_TYPE type, struct questline * questline, unsigned int level);
static struct quest * quest_free(struct quest * quest);

struct questline * questline_new(enum QUEST_TYPE type, enum RELATION_TYPE first_questgiver_relation, enum RELATION_TYPE ancient_person_relation)
{
  struct persondata
  {
    enum RELATION_TYPE relation_type;
    enum GENDER        gender;
    double             name_chance_players;
    double             name_chance_existing;
    //double             name_chance_new;
  };

  struct persondata questgivers[] =
    {
      { RELATION_TYPE_GREAT_GRANDMOTHER, GENDER_FEMALE, 0.6, 0.3 },
      { RELATION_TYPE_GREAT_GRANDFATHER, GENDER_MALE,   0.6, 0.3 },
      { RELATION_TYPE_GRANDMOTHER,       GENDER_FEMALE, 0.6, 0.3 },
      { RELATION_TYPE_GRANDFATHER,       GENDER_MALE,   0.6, 0.3 },
      { RELATION_TYPE_MOTHER,            GENDER_FEMALE, 1.0, 0.0 },
      { RELATION_TYPE_FATHER,            GENDER_MALE,   1.0, 0.0 },
      { RELATION_TYPE_SISTER,            GENDER_FEMALE, 0.8, 0.0 },
      { RELATION_TYPE_BROTHER,           GENDER_MALE,   1.0, 0.0 },
      { RELATION_TYPE_AUNT,              GENDER_FEMALE, 0.6, 0.3 },
      { RELATION_TYPE_UNCLE,             GENDER_MALE,   0.6, 0.3 },
      { RELATION_TYPE_COUSIN,            GENDER_FEMALE, 0.4, 0.3 },
      { RELATION_TYPE_RELATIVE,          GENDER_MALE,   0.4, 0.3 },
      { RELATION_TYPE_FEMALE_FRIEND,     GENDER_FEMALE, 0.0, 0.1 },
      { RELATION_TYPE_MALE_FRIEND,       GENDER_MALE,   0.0, 0.1 },
      { RELATION_TYPE_SIZEOF_,           GENDER_NEUTER, 0.0, 0.0 }
    };

  struct persondata ancientpersons[] =
    {
      { RELATION_TYPE_ANCIENT_QUEEN,    GENDER_FEMALE, 0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_KING,     GENDER_MALE,   0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_PRINCESS, GENDER_FEMALE, 0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_PRINCE,   GENDER_MALE,   0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_KNIGHT,   GENDER_MALE,   0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_WAR_HERO, GENDER_NEUTER, 0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_PRIEST,   GENDER_NEUTER, 0.0, 0.5 },
      { RELATION_TYPE_ANCIENT_SHAMAN,   GENDER_NEUTER, 0.0, 0.5 },
      { RELATION_TYPE_SIZEOF_,          GENDER_NEUTER, 0.0, 0.0 }
    };        
  
  struct persondata * qgdata;
  struct persondata * apdata;
  struct questline * questline;

  qgdata = NULL;
  for(int i = 0; qgdata == NULL && questgivers[i].relation_type != RELATION_TYPE_SIZEOF_; i++)
    if(questgivers[i].relation_type == first_questgiver_relation)
      qgdata = &questgivers[i];
  apdata = NULL;
  for(int i = 0; apdata == NULL && ancientpersons[i].relation_type != RELATION_TYPE_SIZEOF_; i++)
    if(ancientpersons[i].relation_type == ancient_person_relation)
      apdata = &ancientpersons[i];

  questline = malloc(sizeof *questline);
  assert(questline != NULL);
  if(questline != NULL)
    {
      questline->type                                = type;
      questline->opening_weekday                     = -1;
      questline->first_questgiver.relation_to_player = first_questgiver_relation;
      questline->first_questgiver.gender             = qgdata != NULL ? qgdata->gender : GENDER_NEUTER;
      questline->first_questgiver.name               = NULL;
      questline->ancient_person.relation_to_player   = ancient_person_relation;
      questline->ancient_person.gender               = apdata != NULL ? apdata->gender : GENDER_NEUTER;
      questline->ancient_person.name                 = NULL;
      questline->reward                              = 0;

      questline->quests = stack_new();
      questline->diary_entries = stack_new();

      questline->current_quest = 0;
      questline->current_phase = QUESTLINE_PHASE_NONE;


      if(questline->type != QUEST_TYPE_SIZEOF_)
        {
          assert(qgdata != NULL);
          assert(apdata != NULL);
          questline->first_questgiver.name = quest_new_human_name(questline->first_questgiver.relation_to_player,
                                                                  questline->first_questgiver.gender,
                                                                  qgdata->name_chance_players, qgdata->name_chance_existing);
              
          questline->ancient_person.name = quest_new_human_name(questline->ancient_person.relation_to_player,
                                                                questline->ancient_person.gender,
                                                                apdata->name_chance_players, apdata->name_chance_existing);

          int qcount;
          
          switch(type)
            {
            case QUEST_TYPE_RELATIVE:        qcount = 10; break;
            case QUEST_TYPE_CHILDHOOD_DREAM: qcount = 20; break;
            case QUEST_TYPE_SIZEOF_:         qcount =  0; break;
            default:              assert(0); qcount =  0; break;
            }

          for(int i = 0; i < qcount; i++)
            {
              struct quest * q;

              q = quest_new(type, questline, i);
              stack_push(questline->quests, q);
            }
#if 0
          if(type == QUEST_TYPE_RELATIVE)
            {
              questline->quests = stack_new();
              for(int i = 0; i < TREASURE_TYPE_SIZEOF_; i++)
                for(int j = 0; j < 2; j++)
                  {
                    struct quest * q;
                    
                    q = quest_new(type, questline, i);
                    stack_push(questline->quests, q);
                    
                    treasure_free(q->treasure_object);
                    if(j == 0)
                      q->treasure_object = treasure_new(i, TREASURE_MATERIAL_GOLD, TREASURE_GEMSTONE_RUBY);
                    else
                      q->treasure_object = treasure_new(i, TREASURE_MATERIAL_GOLD, TREASURE_GEMSTONE_NONE);
                    q->opened = time(NULL);
                    q->completed = q->opened;
                  }
              questline->current_quest = questline->quests->size - 1;
              questline->current_phase = QUESTLINE_PHASE_TREASURE_COLLECTED;
            }
#endif
        }
    }

  assert(questline != NULL);
  
  return questline;
}



struct questline * questline_free(struct questline * questline)
{
  assert(questline != NULL);

  free(questline->first_questgiver.name);
  free(questline->ancient_person.name);
  
  for(unsigned int i = 0; i < questline->quests->size; i++)
    questline->quests->data[i] = quest_free(questline->quests->data[i]);
  questline->quests = stack_free(questline->quests);
  questline->diary_entries = stack_free(questline->diary_entries);
  
  free(questline);
  return NULL;
}






static struct quest * quest_new(enum QUEST_TYPE type, struct questline * questline, unsigned int level)
{
  struct quest * quest;

  quest = malloc(sizeof *quest);
  assert(quest != NULL);
  if(quest != NULL)
    {
      char * caves[3] =
        {
          "a",
          "/random",
          "/well"
        };

      quest->questline = questline;
      quest->opened    = 0;
      quest->completed = 0;
      quest->description_id = 0;
      quest->completion_id  = 0;
      quest->hours_to_salesman = 2 + get_rand(22);
      
      /* Treasure object */
      quest->treasure_object = treasure_new(get_rand(TREASURE_TYPE_SIZEOF_), get_rand(TREASURE_MATERIAL_SIZEOF_), get_rand(TREASURE_GEMSTONE_SIZEOF_));
      quest->treasure_sold   = false;
      /* Treasure location */
      quest->treasure_cave = strdup(caves[get_rand(3)]);
      quest->treasure_level = 1 + level;

      /* Find the previous quest */
      struct quest * prevquest;

      if(questline->quests->size == 0)
        prevquest = NULL;
      else
        prevquest = questline->quests->data[questline->quests->size - 1];
      

      if(type == QUEST_TYPE_RELATIVE)
        {
          if(level == 0)
            {
              quest->action_to_open = QUEST_ACTION_EXAMINE_NOTES;
              quest->description_id = 0;
            }
          else if(get_rand(100) < 30)
            {
              quest->action_to_open = QUEST_ACTION_EXAMINE_OBJECT;
              quest->description_id = get_rand(2);
            }
          else if(get_rand(100) < 50)
            {
              quest->action_to_open = QUEST_ACTION_EXAMINE_OBJECT;
              quest->description_id = 2;

              /* The treasure object should be similar to the previous one, so copy the data, except for the jewels which we modify manually. */
              enum TREASURE_GEMSTONE g;

              g = prevquest->treasure_object->gemstones + 1;
              if(g >= TREASURE_GEMSTONE_SIZEOF_)
                g = 0;

              treasure_free(quest->treasure_object);
              quest->treasure_object = treasure_new(prevquest->treasure_object->type, prevquest->treasure_object->material, g);

              /* Also the location is close to the previous one: */
              free(quest->treasure_cave);
              quest->treasure_cave = strdup(prevquest->treasure_cave);
              quest->treasure_level = prevquest->treasure_level + 1;
            }
          else
            {
              quest->action_to_open = QUEST_ACTION_EXAMINE_NOTES;
              quest->description_id = get_rand(2);
            }
        }
      else if(type == QUEST_TYPE_CHILDHOOD_DREAM)
        {
          quest->action_to_open = QUEST_ACTION_VISIT_LIBRARY;
          quest->description_id = 0;
        }
      else
        assert(0);

      
      /* Make sure the treasure_level is in range for the treasure_cave: */
      struct map * map;

      map = NULL;
      while(map == NULL)
        {
          map = map_get(quest->treasure_cave, quest->treasure_level);
          if(map == NULL || map->level != (int) quest->treasure_level)
            { /* Past the maximum number of levels in the given cave, try one level less. */
              assert(quest->treasure_level > 0);
              quest->treasure_level--;
              map = map_free(map);
            }
          else if(map->is_intermission == true)
            { /* Skip intermission levels. Assume that there are no two intermission levels as the last two levels of the cave. */
              map = map_free(map);
              map = map_get(quest->treasure_cave, quest->treasure_level + 1);
              if(map != NULL && map->level == (int) quest->treasure_level + 1)
                quest->treasure_level++;
              else /* End of levels in this cave, we need to go back. */
                quest->treasure_level--;
            }
        }
      assert(map != NULL);

      
      /* Skip intermission levels: */
      while(map != NULL && map->is_intermission == true)
        {
          map = map_get(quest->treasure_cave, map->level + 1);
          assert(map != NULL);
        }
      
      if(map->level < (int) quest->treasure_level)
        quest->treasure_level = map->level;
      
      
      /* Generate the treasure coordinates now that we know for sure the cave and level: */
      /* Find suitable location to place our treasure chest: */
      unsigned int * locations;
      unsigned int   locations_ind;

      locations = malloc(sizeof(unsigned int) * 2 * map->width * map->height);
      assert(locations != NULL);
      locations_ind = 0;

      enum MAP_GLYPH suitable[] =
        {
          MAP_SAND,
          MAP_EMPTY,
          MAP_BOULDER,
          MAP_ENEMY2,
          MAP_SIZEOF_
        };

      
      for(int i = 0; locations_ind == 0 && suitable[i] != MAP_SIZEOF_; i++)
        for(unsigned int y = 0; (int) y < map->height; y++)
          for(unsigned int x = 0; (int) x < map->width; x++)
            if(map->data[x + y * map->width] == suitable[i])
              {
                locations[locations_ind * 2 + 0] = x;
                locations[locations_ind * 2 + 1] = y;
                locations_ind++;
              }
      assert(locations_ind > 0);

      /* Pick a random one from the suitable locations: */
      unsigned int n;

      n = get_rand(locations_ind);
      quest->treasure_x = locations[n * 2 + 0];
      quest->treasure_y = locations[n * 2 + 1];

      free(locations);
      map = map_free(map);
    }

  return quest;
}


static struct quest * quest_free(struct quest * quest)
{
  assert(quest != NULL);

  quest->treasure_object = treasure_free(quest->treasure_object);
  free(quest->treasure_cave);
  
  free(quest);

  return NULL;
}


unsigned int questline_treasure_get_collected_count(struct questline const * questline)
{
  unsigned int count;

  count = 0;
  for(unsigned int i = 0; i <= questline->current_quest; i++)
    {
      struct quest * q;

      q = questline->quests->data[i];
      if(q->completed > 0 && q->treasure_sold == false)
        count++;
    }

  return count;
}
