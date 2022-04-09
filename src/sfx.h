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

#ifndef SFX_H_
#define SFX_H_

#include <AL/al.h>
#include <stdint.h>
#include <stdbool.h>

enum SFX
  {
    SFX_BOULDER_FALL,
    SFX_BOULDER_MOVE,
    SFX_DIAMOND_FALL,
    SFX_DIAMOND_COLLECT,
    SFX_MOVE_EMPTY,
    SFX_MOVE_SAND,
    SFX_COLLECT_TREASURE,
    SFX_AMEBA,
    SFX_EXPLOSION,
    SFX_NOISELESS_EXPLOSION,
    SFX_SMALL_EXPLOSION,
    SFX_TIME,
    SFX_ENTER_EXIT,
    SFX_INTRO,
    SFX_SIZEOF_
  };

enum MUSIC
  {
    MUSIC_TITLE,
    MUSIC_START,
    MUSIC_SIZEOF_
  };


struct sfx
{
  ALuint             openal_buffer;
  char *             base_filename;
  int16_t *          waveform;
  uint32_t           waveform_length;
  struct sfx_synth * synth;
};




/* sfx functions */
extern void     sfx_initialize(void);
extern void     sfx_cleanup(void);
extern void     sfx_set_listener_position(int x, int y);
extern void     sfx_emit(enum SFX sfx_id, int x, int y);
extern void     sfx_stop(void);
extern float    sfx_length(enum SFX sfx_id);
extern void     sfx_music(enum MUSIC music_id, int loop_forever);
extern void     sfx_music_stop(void);
extern uint32_t sfx_music_position(void);
extern void     sfx_volume(float volume);
extern void     sfx_music_volume(float volume);
extern void     sfx_pitch(float pitch);

extern float    sfx_get_volume(void);
extern float    sfx_get_music_volume(void);

extern bool     sfx_load(struct sfx * sfx);
extern bool     sfx_load_xm(struct sfx * sfx);
extern bool     sfx_load_file(struct sfx * sfx, const char * filename);
extern bool     sfx_load_ogg(struct sfx * sfx);
extern bool     sfx_load_synth(struct sfx * sfx);

extern bool     sfx_save(struct sfx * sfx, char const * filename);

#endif
