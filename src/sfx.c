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

#include "diamond_girl.h"
#include "sfx.h"
#include "sfx_synth.h"
#include "globals.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <assert.h>
#include <errno.h>

static ALCdevice *  device;
static ALCcontext * context;

static ALuint   source_looping;
static ALuint * sources;

static struct sfx * sounds[SFX_SIZEOF_];
static struct sfx * musics[MUSIC_SIZEOF_];

static ALuint music_channel;
static int    next_channel;

static float volume_sfx;
static float volume_music;
static float pitch_sfx;

static bool initialized = false;


static struct sfx * sfx_new(void);
static struct sfx * sfx_free(struct sfx * sfx);


void sfx_initialize(void)
{
  device       = NULL;
  context      = NULL;
  next_channel = 0;
  volume_sfx   = 1.0f;
  volume_music = 1.0f;
  pitch_sfx    = 1.0f;

  sources = malloc(globals.max_channels * (sizeof *sources));
  assert(sources != NULL);
  if(sources != NULL)
    for(int i = 0; i < globals.max_channels; i++)
      sources[i] = AL_NONE;
  source_looping = AL_NONE;

  for(int i = 0; i < SFX_SIZEOF_; i++)
    sounds[i] = sfx_new();

  for(int i = 0; i < MUSIC_SIZEOF_; i++)
    musics[i] = sfx_new();
  music_channel = AL_NONE;

  if(globals.use_sfx || globals.use_music)
    {
      device = alcOpenDevice(NULL);
      if(device != NULL)
        {
          context = alcCreateContext(device, NULL);
          if(context != NULL)
            {
              ALenum e;

              alcMakeContextCurrent(context);

              alGetError();
              alGenSources(globals.max_channels, sources);
              e = alGetError();
              if(e == AL_NO_ERROR)
                {
                  alGenSources(1, &source_looping);
                  e = alGetError();
                  if(e == AL_NO_ERROR)
                    alSourcei(source_looping, AL_SOURCE_RELATIVE, AL_TRUE);
                }
              if(e == AL_NO_ERROR && globals.use_music)
                {
                  alGenSources(1, &music_channel);
                  e = alGetError();
                  if(e == AL_NO_ERROR)
                    alSourcei(music_channel, AL_SOURCE_RELATIVE, AL_TRUE);
                }
              if(e == AL_NO_ERROR)
                {
                  if(globals.use_sfx)
                    {
                      struct
                      {
                        enum SFX id;
                        char *   base_filename;
                      } sfx_filenames[SFX_SIZEOF_] =
                          {
                            { SFX_BOULDER_FALL,        "boulder_fall"         },
                            { SFX_BOULDER_MOVE,        "boulder_move"         },
                            { SFX_DIAMOND_FALL,        "diamond_fall"         },
                            { SFX_DIAMOND_COLLECT,     "diamond_collect"      },
                            { SFX_MOVE_EMPTY,          "move_empty"           },
                            { SFX_MOVE_SAND,           "move_sand"            },
                            { SFX_COLLECT_TREASURE,    "collect_treasure"     },
                            { SFX_AMEBA,               "ameba"                },
                            { SFX_EXPLOSION,           "explosion"            },
                            { SFX_NOISELESS_EXPLOSION, "noiseless_explosion"  },
                            { SFX_SMALL_EXPLOSION,     "small_explosion"      },
                            { SFX_TIME,                "time"                 },
                            { SFX_ENTER_EXIT,          "enter_exit"           },
                            { SFX_INTRO,               "intro"                }
                          };

                      for(unsigned int i = 0; i < SFX_SIZEOF_; i++)
                        {
                          assert(sfx_filenames[i].id == i);
                          sounds[i]->base_filename = strdup(sfx_filenames[i].base_filename);
                          if(sfx_load(sounds[i]) == true)
                            {
#ifndef NDEBUG
                              if(globals.save_sound_effects == true)
                                {
                                  char fn[128];

                                  snprintf(fn, sizeof fn, "%s.wav", sounds[i]->base_filename);
                                  printf("Saving %s...\n", fn);
                                  sfx_save(sounds[i], fn);
                                }
#endif
                            }
                          else
                            { /* Failed to load. */
                              assert(0);
                            }
                        }
                    }

                  if(globals.use_music)
                    {
                      struct
                      {
                        enum MUSIC id;
                        char *     base_filename;
                      } music_filenames[SFX_SIZEOF_] =
                          {
                            { MUSIC_TITLE, "title" },
                            { MUSIC_START, "start" }
                          };
              
                      for(unsigned int i = 0; i < MUSIC_SIZEOF_; i++)
                        {
                          assert(music_filenames[i].id == i);
                          musics[i]->base_filename = strdup(music_filenames[i].base_filename);
                          sfx_load(musics[i]);
                        }
                    }
	  
                  ALfloat listenerPos[] = { 0.0, 0.0, 0.0 };
                  ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
                  ALfloat listenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	  
                  alListenerfv(AL_POSITION,    listenerPos);
                  alListenerfv(AL_VELOCITY,    listenerVel);
                  alListenerfv(AL_ORIENTATION, listenerOri);

                  initialized = true;
                }
              else
                fprintf(stderr, "Failed to initialize audio: %d\n", (int) e);
            }
          else
            fprintf(stderr, "%s(): Failed to create context.\n", __FUNCTION__);
        }
      else
        fprintf(stderr, "%s(): Failed to connect to the default device.\n", __FUNCTION__);
    }
}

void sfx_cleanup(void)
{
  if(globals.use_sfx || globals.use_music)
    {
      if(globals.use_sfx)
        {
          for(int i = 0; i < SFX_SIZEOF_; i++)
            if(sounds[i] != NULL)
              sounds[i] = sfx_free(sounds[i]);

          alDeleteSources(globals.max_channels, sources);
          alDeleteSources(1, &source_looping);
        }
      
      if(globals.use_music)
        {
          for(int i = 0; i < MUSIC_SIZEOF_; i++)
            if(musics[i] != NULL)
              musics[i] = sfx_free(musics[i]);
      
          alDeleteSources(1, &music_channel);
        }

      if(device != NULL)
        {
          if(context != NULL)
            {
              alcMakeContextCurrent(NULL);
              alcDestroyContext(context);
            }
          alcCloseDevice(device);
        }
      context = NULL;
      device  = NULL;
    }

  if(sources != NULL)
    {
      free(sources);
      sources = NULL;
    }
}



void sfx_emit(enum SFX sfx_id, int x, int y)
{
  if(globals.use_sfx)
    {
      assert(sounds[sfx_id] != NULL);
      assert(sounds[sfx_id]->openal_buffer != AL_NONE);
      if(sounds[sfx_id] != NULL && sounds[sfx_id]->openal_buffer != AL_NONE)
	{
	  ALuint source;
	  int loops;

	  loops = 0;
	  if(sfx_id == SFX_AMEBA)
	    loops = 1;

	  if(loops)
	    {
	      source = source_looping;
	    }
	  else
	    {
	      source = sources[next_channel];
	      next_channel++;
	      if(next_channel >= globals.max_channels)
		next_channel = 0;
	    }
	  
	  ALenum e;

	  alSourceStop(source);

	  alSourcei(source, AL_BUFFER, sounds[sfx_id]->openal_buffer);
	  e = alGetError();
	  if(e == AL_NO_ERROR)
	    {
	      ALfloat pos[3];

              pos[0] = x;
              pos[1] = y;
              pos[2] = 0;
	      
	      alSourcefv(source, AL_POSITION, pos);
	      if(loops)
		alSourcei(source, AL_LOOPING, AL_TRUE);
	      else
		alSourcei(source, AL_LOOPING, AL_FALSE);
              alSourcef(source, AL_MAX_GAIN, volume_sfx * (float) globals.volume / 100.0f);
              alSourcef(source, AL_PITCH, pitch_sfx);
	      alSourcePlay(source);
	    }
	}
    }
}

void sfx_stop(void)
{
  if(globals.use_sfx)
    alSourceStop(source_looping);
}

void sfx_music(enum MUSIC music_id, int loop_forever)
{
  if(globals.use_music)
    if(musics[music_id] != NULL && musics[music_id]->openal_buffer != AL_NONE)
      {
	ALenum e;

	alSourceStop(music_channel);
	alSourcei(music_channel, AL_BUFFER, musics[music_id]->openal_buffer);
	e = alGetError();
	if(e == AL_NO_ERROR)
	  {
	    ALfloat sourcePos[] = {0.0f, 0.0f, 0.0f};
	      
	    alSourcefv(music_channel, AL_POSITION, sourcePos);
	    if(loop_forever)
	      alSourcei(music_channel, AL_LOOPING, AL_TRUE);
	    else
	      alSourcei(music_channel, AL_LOOPING, AL_FALSE);
            alSourcef(music_channel, AL_MAX_GAIN, volume_music * (float) globals.volume / 100.0f);
	    alSourcePlay(music_channel);
	  }
      }
}

void sfx_music_stop(void)
{
  if(globals.use_music)
    if(music_channel != AL_NONE)
      alSourceStop(music_channel);
}



void sfx_volume(float volume)
{
  if(globals.use_sfx)
    {
      volume_sfx = volume;
      alSourcef(source_looping, AL_MAX_GAIN, volume_sfx * (float) globals.volume / 100.0f);
    }
}

float sfx_get_volume(void)
{
  float rv;

  if(globals.use_sfx)
    rv = volume_sfx;
  else
    rv = 0.0f;

  return rv;
}


void sfx_music_volume(float volume)
{
  if(globals.use_music)
    {
      volume_music = volume;
      alSourcef(music_channel, AL_MAX_GAIN, volume_music * (float) globals.volume / 100.0f);
    }
}

float sfx_get_music_volume(void)
{
  float rv;

  if(globals.use_music)
    rv = volume_music;
  else
    rv = 0.0f;

  return rv;
}


void sfx_pitch(float pitch)
{
  if(globals.use_sfx)
    pitch_sfx = pitch;
}


uint32_t sfx_music_position(void)
{
  uint32_t pos;
  
  pos = 0;
  if(globals.use_music)
    {
      int offset;

      alGetSourcei(music_channel, AL_SAMPLE_OFFSET, &offset);
      pos = offset;
    }

  return pos;
}



static struct sfx * sfx_new(void)
{
  struct sfx * sfx;

  sfx = malloc(sizeof(struct sfx));
  assert(sfx != NULL);
  if(sfx != NULL)
    {
      sfx->openal_buffer   = AL_NONE;
      sfx->base_filename   = NULL;
      sfx->waveform        = NULL;
      sfx->waveform_length = 0;
      sfx->synth           = NULL;
    }
  else
    fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));

  return sfx;
}


static struct sfx * sfx_free(struct sfx * sfx)
{
  assert(sfx != NULL);

  if(sfx->openal_buffer != AL_NONE)
    {
      alDeleteBuffers(1, &sfx->openal_buffer);
      sfx->openal_buffer = AL_NONE;
    }

  sfx->synth = sfx_synth_free(sfx->synth);

  free(sfx->base_filename);
  free(sfx->waveform);

  free(sfx);

  return NULL;
}


void sfx_set_listener_position(int x, int y)
{
  ALfloat pos[3];

  pos[0] = x;
  pos[1] = y;
  pos[2] = 0;
  alListenerfv(AL_POSITION, pos);
}




float sfx_length(enum SFX sfx_id)
{
  float length;

  length = 0.0f;
  if(globals.use_sfx)
    {
      ALint freq, bits, channels, size;

      assert(sounds[sfx_id] != NULL);
      assert(sounds[sfx_id]->openal_buffer != AL_NONE);

      alGetBufferi(sounds[sfx_id]->openal_buffer, AL_FREQUENCY, &freq);
      alGetBufferi(sounds[sfx_id]->openal_buffer, AL_BITS,      &bits);
      alGetBufferi(sounds[sfx_id]->openal_buffer, AL_CHANNELS,  &channels);
      alGetBufferi(sounds[sfx_id]->openal_buffer, AL_SIZE,      &size);

      length = (float) size / (float) channels / (float) (bits / 8) / (float) freq;
    }

  return length;
}
