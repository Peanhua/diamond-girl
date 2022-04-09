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

#ifndef QUEST_H_
#define QUEST_H_

#include <stdbool.h>
#include <time.h>

#include "diamond_girl.h"
#include "treasure.h"

struct questline;
struct stack;
struct trader;
struct widget;

#define QUESTLINE_SAVEFILE_V1 "dgq1"

enum QUEST_TYPE
  {
    QUEST_TYPE_RELATIVE,
    QUEST_TYPE_LIBRARY,
    QUEST_TYPE_ZOMBIES,
    QUEST_TYPE_SIZEOF_
  };

enum QUEST_ACTION
  {
    QUEST_ACTION_EXAMINE_NOTES,
    QUEST_ACTION_EXAMINE_OBJECT,
    QUEST_ACTION_VISIT_CAFE,
    QUEST_ACTION_VISIT_LIBRARY
  };

struct quest
{
  struct questline * questline;
  
  enum QUEST_ACTION  action_to_open;
  time_t             opened;
  time_t             completed;
  uint8_t            description_id;
  uint8_t            completion_id;
  uint8_t            hours_to_salesman; // How many hours from opened timestamp until the salesman in the cafe starts to sell the treasure item.
  
  struct treasure *  treasure_object;
  bool               treasure_sold;
  
  char *             treasure_cave;
  unsigned int       treasure_level;
  unsigned int       treasure_x;
  unsigned int       treasure_y;
};

extern char const * quest_description(struct quest * quest);


enum RELATION_TYPE
  {
    RELATION_TYPE_GREAT_GRANDMOTHER,
    RELATION_TYPE_GREAT_GRANDFATHER,
    RELATION_TYPE_GRANDMOTHER,
    RELATION_TYPE_GRANDFATHER,
    RELATION_TYPE_MOTHER,
    RELATION_TYPE_FATHER,
    RELATION_TYPE_SISTER,
    RELATION_TYPE_BROTHER,
    RELATION_TYPE_AUNT,
    RELATION_TYPE_UNCLE,
    RELATION_TYPE_COUSIN,
    RELATION_TYPE_RELATIVE,
    RELATION_TYPE_FEMALE_FRIEND,
    RELATION_TYPE_MALE_FRIEND,

    RELATION_TYPE_ANCIENT_QUEEN,
    RELATION_TYPE_ANCIENT_KING,

    RELATION_TYPE_ANCIENT_PRINCESS,
    RELATION_TYPE_ANCIENT_PRINCE,

    RELATION_TYPE_ANCIENT_KNIGHT,
    RELATION_TYPE_ANCIENT_WAR_HERO,
    RELATION_TYPE_ANCIENT_PRIEST,
    RELATION_TYPE_ANCIENT_SHAMAN,
    
    RELATION_TYPE_SIZEOF_
  };

extern char const * relation_type_name(enum RELATION_TYPE type);


struct diary_entry
{
  time_t          timestamp;
  struct image *  image;
  struct treasure image_source; /* Information how to generate diary_entry->image */
  char *          entry;
};


struct person
{
  char *             name;
  enum GENDER        gender;
  enum RELATION_TYPE relation_to_player;
};


enum QUESTLINE_PHASE
  {
    QUESTLINE_PHASE_NONE,
    QUESTLINE_PHASE_OPENED,
    QUESTLINE_PHASE_TREASURE_COLLECTED
  };

struct questline
{
  enum QUEST_TYPE    type;
  int                opening_weekday;  /* The months first weekday when this can be opened, -1 to ignore, 0 = Sunday, 1 = Monday, ... 6 = Saturday. */
  int                opening_monthday_max;
  int                opening_time_hour;   /* The first hour this questline, and the quests in it, can be opened, -1 to disable. */
  int                opening_time_length; /* The last hour this questline, and the quests in it, can be opened, is hour+length */
  struct person      first_questgiver; /* The person who initiates the quest, part of the story. */
  struct person      ancient_person;   /* Some ancient person, probably dead by now, part of the story. */
  unsigned int       reward;           /* Diamond score from obtained item = 100 + 'quest#' + 'reward', and from the last item = 2000 + 250 * 'reward' */
  struct stack *     quests;
  struct stack *     diary_entries;

  unsigned int         current_quest;
  enum QUESTLINE_PHASE current_phase;
};




extern struct questline * questline_new(enum QUEST_TYPE type, enum RELATION_TYPE first_questgiver_relation, enum RELATION_TYPE ancient_person_relation);
extern struct questline * questline_free(struct questline * questline);

extern struct questline * questline_generate(enum QUEST_TYPE type);

extern bool questline_save(void); /* Save all questlines from globals.questlines */
extern bool questline_load(void); /* Load questlines to globals.questlines */

extern char const * questline_name(struct questline * questline);
extern unsigned int questline_treasure_get_collected_count(struct questline const * questline);

extern void quest_diary(struct questline * questline);
extern void quest_treasures(struct questline * questline, struct trader * trader);

extern void quest_action(struct questline * questline, enum QUEST_ACTION action);
extern void quest_open(struct quest * quest);
extern void quest_collect_treasure(struct questline * questline);

extern char * quest_new_human_name(enum RELATION_TYPE relation_type, enum GENDER gender, double name_chance_players, double name_chance_existing);

extern struct diary_entry * questline_diary_add(struct questline * questline, time_t timestamp, struct treasure * image_source, char * entry);

#endif
