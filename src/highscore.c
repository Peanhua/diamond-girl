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

#include "highscore.h"
#include "playback.h"
#include "game.h"
#include "cave.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define FILE_HEADER_V2 "dgh2"
#define FILE_HEADER_V3 "dgh3"
#define FILE_HEADER_V4 "dgh4"
#define FILE_HEADER_V5 "dgh5"


static int highscore_add(struct highscorelist * list, time_t timestamp, int score, int diamond_score, uint32_t diamonds_collected, enum GAME_MODE game_mode, char const * const cave, int level, int starting_girls, trait_t traits, bool playback_dirty, int playback_id, struct playback * playback, char * notes);



struct highscorelist * highscores_new(void)
{
  struct highscorelist * list;

  list = malloc(sizeof(struct highscorelist));
  assert(list != NULL);
  if(list != NULL)
    {
      list->filename         = NULL;
      list->highscores       = NULL;
      list->highscores_size  = 0;
      list->highscores_dirty = 0;
      list->total.score              = 0;
      list->total.caves_entered      = 0;
      list->total.levels_completed   = 0;
      list->total.diamonds_collected = 0;
      list->total.girls_died         = 0;
    }

  return list;
}

struct highscorelist * highscores_free(struct highscorelist * list)
{
  free(list->filename);
  list->filename = NULL;

  for(size_t i = 0; i < list->highscores_size; i++)
    if(list->highscores[i] != NULL)
      {
        free(list->highscores[i]->cave);
        if(list->highscores[i]->playback != NULL)
          list->highscores[i]->playback = playback_free(list->highscores[i]->playback);

        free(list->highscores[i]);
      }
  free(list->highscores);
  list->highscores = NULL;
  list->highscores_size = 0;
  list->highscores_dirty = 0;

  return NULL;
}

void highscores_load(struct highscorelist * list, enum GAME_MODE game_mode, const char * cave)
{
#ifdef PROFILING

  if(list || cave || game_mode)
    {
    }
#else

  FILE * fp;
  
  { /* Free the old entries. Save other variables. */
    char * tmp;

    tmp = list->filename;
    list->filename = NULL;
    highscores_free(list);
    list->filename = tmp;
  }

  list->highscores_dirty = 0;

  if(list->filename == NULL)
    list->filename = highscore_filename(game_mode, cave);
  assert(list->filename != NULL);

  fp = fopen(get_save_filename(list->filename), "rb");
  if(fp != NULL)
    {
      int done;
      int i;
      char header[4];
      int version;
      int ok;

      version = 1; /* Version 1 did not have any header. */
      if(fread(header, 4, 1, fp) == 1) /* Header will always be 4 characters. */
        {
          if(memcmp(header, FILE_HEADER_V2, 4) == 0)
            version = 2;
          else if(memcmp(header, FILE_HEADER_V3, 4) == 0)
            version = 3;
          else if(memcmp(header, FILE_HEADER_V4, 4) == 0)
            version = 4;
          else if(memcmp(header, FILE_HEADER_V5, 4) == 0)
            version = 5;
        }

      if(version == 1)
        rewind(fp);

      ok = 1;
      if(version >= 3)
        {
          if(ok)
            ok = fread(&list->total.caves_entered,      sizeof list->total.caves_entered,      1, fp);
          if(ok)
            ok = fread(&list->total.levels_completed,   sizeof list->total.levels_completed,   1, fp);
          if(ok)
            ok = fread(&list->total.diamonds_collected, sizeof list->total.diamonds_collected, 1, fp);
          if(ok)
            ok = fread(&list->total.girls_died,         sizeof list->total.girls_died,         1, fp);
        }

      done = 0;
      i = 0;
      while(ok && !done)
	{
	  struct highscore_entry tmp;
          char tmp_cave[0xff];

          if(ok)
            ok = fread(&tmp.timestamp, sizeof tmp.timestamp, 1, fp);
	  if(ok)
	    ok = fread(&tmp.score, sizeof tmp.score, 1, fp);
          
          tmp.diamonds_collected = 0;
          if(version >= 3)
            if(ok)
              ok = fread(&tmp.diamonds_collected, sizeof tmp.diamonds_collected, 1, fp);

	  if(ok)
	    ok = fread(&tmp.level, sizeof tmp.level, 1, fp);

          tmp.playback_id = 0;
          if(version >= 2)
            if(ok)
              ok = fread(&tmp.playback_id, sizeof tmp.playback_id, 1, fp);

          if(cave != NULL)
            strcpy(tmp_cave, cave);
          else
            strcpy(tmp_cave, "");
          
          strcpy(tmp.notes, "");

          if(version >= 3)
            {
              if(ok)
                { /* Cave name. */
                  uint8_t len;
              
                  ok = fread(&len, sizeof len, 1, fp);
                  if(ok)
                    {
                      assert(len < sizeof tmp_cave);
                      assert(len > 0);
                      ok = fread(tmp_cave, len, 1, fp);
                      tmp_cave[len] = '\0';
                    }
                }
              if(ok)
                { /* Notes */
                  uint8_t len;
              
                  ok = fread(&len, sizeof len, 1, fp);
                  if(ok)
                    {
                      assert(len < sizeof tmp.notes);
                      if(len > 0)
                        ok = fread(tmp.notes, len, 1, fp);
                      tmp.notes[len] = '\0';
                    }
                }
            }

          if(version >= 4)
            {
              if(ok)
                ok = fread(&tmp.diamond_score, sizeof tmp.diamond_score, 1, fp);
              if(ok)
                ok = fread(&tmp.starting_girls, sizeof tmp.starting_girls, 1, fp);

              if(ok)
                {
                  if(version == 4)
                    {
                      uint16_t t;

                      ok = fread(&t, sizeof t, 1, fp);
                      tmp.traits = t;
                    }
                  else /* if(version >= 5) */
                    {
                      ok = fread(&tmp.traits, sizeof tmp.traits, 1, fp);
                    }
                }
            }
          else
            {
              tmp.diamond_score  = 0;
              tmp.starting_girls = 3;
              tmp.traits         = 0;
            }

	  if(ok)
	    {
	      highscore_add(list, tmp.timestamp, tmp.score, tmp.diamond_score, tmp.diamonds_collected, game_mode, tmp_cave, tmp.level, tmp.starting_girls, tmp.traits, false, tmp.playback_id, NULL, tmp.notes);
              i++;
	    }
	  else
	    {
	      if(!feof(fp))
		fprintf(stderr, "Failed to read highscores from %s: %s\n", get_save_filename(list->filename), strerror(errno));
	      done = 1;
	    }
	}
      fclose(fp);
    }

#endif
}

void highscores_save(struct highscorelist * list, enum GAME_MODE game_mode, const char * cave)
{
#ifdef PROFILING

  if(list || cave || game_mode)
    {
    }

#else

  if(list->highscores_dirty)
    {
      FILE * fp;

      if(list->filename == NULL)
        list->filename = highscore_filename(game_mode, cave);
      assert(list->filename != NULL);

      fp = fopen(get_save_filename(list->filename), "wb");
      if(fp != NULL)
        {
          int ok;
          size_t i;

          ok = fwrite(FILE_HEADER_V5, 4, 1, fp);

          if(ok)
            ok = fwrite(&list->total.caves_entered,      sizeof list->total.caves_entered,      1, fp);
          if(ok)
            ok = fwrite(&list->total.levels_completed,   sizeof list->total.levels_completed,   1, fp);
          if(ok)
            ok = fwrite(&list->total.diamonds_collected, sizeof list->total.diamonds_collected, 1, fp);
          if(ok)
            ok = fwrite(&list->total.girls_died,         sizeof list->total.girls_died,         1, fp);
      
          for(i = 0; i < list->highscores_size && ok; i++)
            if(list->highscores[i] != NULL)
              {
                uint8_t len;

                if(ok)
                  ok = fwrite(&list->highscores[i]->timestamp,          sizeof list->highscores[i]->timestamp,          1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->score,              sizeof list->highscores[i]->score,              1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->diamonds_collected, sizeof list->highscores[i]->diamonds_collected, 1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->level,              sizeof list->highscores[i]->level,              1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->playback_id,        sizeof list->highscores[i]->playback_id,        1, fp);

                assert(strlen(list->highscores[i]->cave) > 0);
                assert(strlen(list->highscores[i]->cave) <= 0xff);
                len = strlen(list->highscores[i]->cave);
                if(ok)
                  ok = fwrite(&len,                                     sizeof len,                                     1, fp);
                if(ok && len > 0)
                  ok = fwrite(list->highscores[i]->cave,                len,                                            1, fp);

                assert(strlen(list->highscores[i]->notes) <= 0xff);
                len = strlen(list->highscores[i]->notes);
                if(ok)
                  ok = fwrite(&len,                                     sizeof len,                                     1, fp);
                if(ok && len > 0)
                  ok = fwrite(list->highscores[i]->notes,               len,                                            1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->diamond_score,      sizeof list->highscores[i]->diamond_score,      1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->starting_girls,     sizeof list->highscores[i]->starting_girls,     1, fp);

                if(ok)
                  ok = fwrite(&list->highscores[i]->traits,             sizeof list->highscores[i]->traits,             1, fp);

                if(!ok)
                  fprintf(stderr, "Failed to write highscores: %s\n", strerror(errno));

                if(ok && list->highscores[i]->playback_dirty == true && list->highscores[i]->playback != NULL)
                  {
                    char fnbuf[128];
                    
                    snprintf(fnbuf, sizeof fnbuf, "%s-%d.dgp", list->filename, list->highscores[i]->playback_id);
                    playback_save(get_save_filename(fnbuf), list->highscores[i]->playback);
                  }
              }

          if(fclose(fp) != 0)
            {
              fprintf(stderr, "Failed to write highscores: %s\n", strerror(errno));
              ok = 0;
            }

          if(ok)
            list->highscores_dirty = 0;
        }
      else
        fprintf(stderr, "Failed to open highscores file '%s': %s\n", get_save_filename(list->filename), strerror(errno));
    }
#endif
}


int highscore_new(struct highscorelist * list, struct gamedata * gamedata, char * notes)
{
  int level;
  struct playback * playback;
  int position;

  position = -1;
  playback = gamedata->playback;
  
  assert(playback != NULL);
  if(playback != NULL)
    {
      while(playback->next != NULL)
        playback = playback->next;
      level = playback->level;
      
      int playback_id;

      playback_id = 0;
      for(size_t i = 0; i < list->highscores_size; i++)
        if(list->highscores[i] != NULL)
          if(list->highscores[i]->playback_id >= playback_id)
            playback_id = list->highscores[i]->playback_id;
      playback_id += 1;

      struct playback * save_playback;

      save_playback = NULL;
      switch(playback->game_mode)
        {
        case GAME_MODE_CLASSIC:
        case GAME_MODE_ADVENTURE:
          save_playback = gamedata->playback;
          break;
        case GAME_MODE_PYJAMA_PARTY:
          break;
        }

      position = highscore_add(list, time(NULL), gamedata->score, gamedata->diamond_score, gamedata->diamonds, playback->game_mode, playback->cave, level, gamedata->starting_girls, gamedata->traits, true, playback_id, save_playback, notes);
      if(position >= 0)
        {
          list->highscores_dirty = 1;
          if(save_playback == gamedata->playback)
            gamedata->playback = NULL; /* Because the pointer was simply copied in highscore_add() */
        }
    }

  return position;
}

static int highscore_add(struct highscorelist * list, time_t timestamp, int score, int diamond_score, uint32_t diamonds_collected, enum GAME_MODE game_mode, char const * const cave, int level, int starting_girls, trait_t traits, bool playback_dirty, int playback_id, struct playback * playback, char * notes)
{
  int position;
  struct highscore_entry * entry;

  assert(cave != NULL);

  position = -1;
  entry = malloc(sizeof(struct highscore_entry));
  assert(entry != NULL);
  if(entry != NULL)
    {
      entry->timestamp          = timestamp;
      entry->score              = score;
      entry->diamond_score      = diamond_score;
      entry->diamonds_collected = diamonds_collected;
      entry->cave               = strdup(cave);
      entry->starting_girls     = starting_girls;
      entry->traits             = traits;
      entry->level              = level;
      entry->playback_dirty     = playback_dirty;
      entry->playback_id        = playback_id;
      entry->playback           = playback;
      memset(entry->notes, '\0', sizeof entry->notes);
      if(notes != NULL)
        snprintf(entry->notes, sizeof entry->notes, "%s", notes);


      if(list->highscores_size < 999)
        { /* Upper limit of the list has not yet been reached. */
          struct highscore_entry ** tmplist;

          tmplist = realloc(list->highscores, sizeof(struct highscore_entry *) * (list->highscores_size + 1));
          if(tmplist != NULL)
            {
              size_t i;
              size_t j, pos;
      
              i = list->highscores_size;
              list->highscores = tmplist;
              list->highscores_size++;
              list->highscores[i] = entry;

	      /* put it in right position */
              pos = i;
              switch(game_mode)
                {
                case GAME_MODE_CLASSIC:
                case GAME_MODE_ADVENTURE:
                  for(j = 0; j < list->highscores_size; j++)
                    if(list->highscores[j]->score < list->highscores[i]->score)
                      {
                        pos = j;
                        break;
                      }
                  break;
                case GAME_MODE_PYJAMA_PARTY:
                  for(j = 0; j < list->highscores_size; j++)
                    if(list->highscores[j]->timestamp < list->highscores[i]->timestamp)
                      {
                        pos = j;
                        break;
                      }
                  break;
                }

	      if(pos != i)
		{
		  struct highscore_entry * tmp;
		  
		  tmp = list->highscores[i];
		  for(j = list->highscores_size - 1; j > pos; j--)
		    list->highscores[j] = list->highscores[j - 1];
		  list->highscores[pos] = tmp;
		}

              position = pos;
            }
          else
            fprintf(stderr, "Failed to allocate memory for a new highscore entry: %s\n", strerror(errno));
        }
      else
        { /* Upper limit of the list has been reached. */
          size_t j, pos;

          pos = list->highscores_size;
          switch(game_mode)
            {
            case GAME_MODE_CLASSIC:
            case GAME_MODE_ADVENTURE:
              for(j = 0; j < list->highscores_size; j++)
                if(list->highscores[j]->score < score)
                  {
                    pos = j;
                    break;
                  }
              break;
            case GAME_MODE_PYJAMA_PARTY:
              for(j = 0; j < list->highscores_size; j++)
                if(list->highscores[j]->timestamp < timestamp)
                  {
                    pos = j;
                    break;
                  }
              break;
            }

          if(pos < list->highscores_size)
            {
              if(list->filename != NULL)
                {
                  assert(list->highscores[list->highscores_size - 1] != NULL);
                  if(list->highscores[list->highscores_size - 1] != NULL)
                    { /* Remove the 999th highscore entry. */
                      assert(list->total.score >= (uint64_t) list->highscores[list->highscores_size - 1]->score);
                      list->total.score -= list->highscores[list->highscores_size - 1]->score;
                  
                      /* Delete the 999th playback file because it'll be purged soon enough. */
                      char fn[128];
                      
                      snprintf(fn, sizeof fn, "%s-%d.dgp", list->filename, list->highscores[list->highscores_size - 1]->playback_id);
                      unlink(get_save_filename(fn));

                      /* Free the 999th highscore entry. */
                      if(list->highscores[list->highscores_size - 1]->playback != NULL)
                        list->highscores[list->highscores_size - 1]->playback = playback_free(list->highscores[list->highscores_size - 1]->playback);

                      free(list->highscores[list->highscores_size - 1]);
                      list->highscores[list->highscores_size - 1] = NULL;
                    }
                }

              /* Move the highscores to make room for this new. */
              for(j = list->highscores_size - 1; j > pos; j--)
                list->highscores[j] = list->highscores[j - 1];

              list->highscores[pos] = entry;
              position = pos;
            }
        }

      if(position < 0)
        { /* Cleanup upon error. */
          free(entry->cave);
          free(entry);
        }
    }
  else
    fprintf(stderr, "Failed to allocate memory for a new highscore entry: %s\n", strerror(errno));

  if(position >= 0)
    list->total.score += score;

  return position;
}


struct highscore_entry ** highscores_get(struct highscorelist * list, size_t * size_ptr)
{
  if(size_ptr != NULL)
    *size_ptr = list->highscores_size;
  return list->highscores;
}



struct playback * highscore_playback(struct highscore_entry * entry, enum GAME_MODE game_mode)
{
  if(entry->playback == NULL)
    {
      char * fn;

      fn = highscore_filename(game_mode, entry->cave);
      if(fn != NULL)
        {
          char fnbuf[256];

          snprintf(fnbuf, sizeof fnbuf, "%s-%d.dgp", fn, entry->playback_id);
          free(fn);

          entry->playback = playback_load(get_save_filename(fnbuf));
        }
      else
        fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
    }

  return entry->playback;
}


void highscores_delete(struct highscorelist * list)
{
  assert(list->filename != NULL);
  unlink(get_save_filename(list->filename));
  
  for(unsigned int i = 0; i < list->highscores_size; i++)
    if(list->highscores[i] != NULL)
      {
        char fn[256];

        snprintf(fn, sizeof fn, "%s-%d.dgp.bz2", list->filename, list->highscores[i]->playback_id);
        unlink(get_save_filename(fn));
      }
}


char * highscore_filename(enum GAME_MODE gamemode, const char * cave)
{
  char buf[256];
  char * fn;

  assert(cave != NULL);
  if(gamemode == GAME_MODE_PYJAMA_PARTY)
    snprintf(buf, sizeof buf, "party/highscores%s", cave_filename(cave));
  else
    snprintf(buf, sizeof buf, "highscores%s", cave_filename(cave));
  fn = strdup(buf);

  return fn;
}
