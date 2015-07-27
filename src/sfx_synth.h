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

#ifndef SFX_SYNTH_H_
#define SFX_SYNTH_H_

#include "sfx.h"
#include <stdbool.h>


enum SFX_SYNTH_WAVEFORM
  {
    SFX_SYNTH_WAVEFORM_SQUARE,
    SFX_SYNTH_WAVEFORM_SAWTOOTH,
    SFX_SYNTH_WAVEFORM_INVERSE_SAWTOOTH,
    SFX_SYNTH_WAVEFORM_SINE,
    SFX_SYNTH_WAVEFORM_NOISE,
    SFX_SYNTH_WAVEFORM_TRIANGLE,
    SFX_SYNTH_WAVEFORM_SIZEOF_
  };

struct sfx_synth_dadsr_envelope
{ /* DADSR envelope. Times in samples, sustain_level in 0.0 .. 1.0, sustain_punch can be "anything". */
  uint32_t delay_time;
  uint32_t attack_time;
  uint32_t decay_time;
  uint32_t sustain_time;
  float    sustain_level;
  float    sustain_punch;
  uint32_t release_time;
};

struct sfx_synth
{
  struct
  { /* Generic stuff for the base waveform. */
    enum SFX_SYNTH_WAVEFORM type;
    float                   period; /* How many samples per cycle. */
  } waveform;

  struct
  { /* Square waveform stuff. */
    float duty_cycle; /* 0.0 .. 1.0 */
    float slide;      /* 0.0 .. 1.0 */
  } square;

  /* Effects. */
  struct sfx_synth_dadsr_envelope amplitude_envelope;

  struct
  { /* Arpeggio. */
    uint32_t limit;
    float    mod;
  } arpeggio;

  struct
  { /* Frequency slide. */
    bool  active;
    float slide;
    float slide_delta;
    int   limit;
  } frequency_slide;

  struct
  { /* Repeater. */
    int limit;
  } repeater;

  float lowpass_filter; /* >= 1.0f to disable */


  /* Current state. */
  struct
  {
    bool     running;
    bool     clipped;       /* True if the samples were clipped during sfx_synth_get_samples(). */
    uint32_t sample_number; /* Current is the nth sample, from 0 to "infinity". */
    uint32_t phase;         /* Current phase, from 0 to period. */
    float    period;        /* Current period, starting value from wavefrom.period. */

    bool     arpeggio_active;
    uint32_t arpeggio_timer;
    float    square_duty_cycle;
    float    square_slide;
    float    frequency_slide;
    int      frequency_slide_timer;
    int      repeat_timer;
    float    lowpass_filter_previous;
  } state;

  /* If next is non-NULL, this and next will be mixed together, there can be unlimited number of chained sfx_synth's mixed. */
  struct sfx_synth * next;
};

extern struct sfx_synth * sfx_synth_load(const char * filename);
extern struct sfx_synth * sfx_synth_new(enum SFX_SYNTH_WAVEFORM waveform, uint32_t length, uint16_t frequency_hz);
extern struct sfx_synth * sfx_synth_free(struct sfx_synth * sfx_synth);
extern uint32_t           sfx_synth_get_samples(struct sfx_synth * sfx_synth, int16_t * buffer, uint32_t max_amount);
extern void               sfx_synth_restart(struct sfx_synth * sfx_synth);

extern void sfx_synth_set_square_slide(struct sfx_synth * sfx_synth, float slide);
extern void sfx_synth_set_frequency_slide(struct sfx_synth * sfx_synth, float slide, float delta, int limit);
extern void sfx_synth_set_amplitude_envelope(struct sfx_synth * sfx_synth,
                                             uint32_t delay_time,
                                             uint32_t attack_time,
                                             uint32_t decay_time,
                                             uint32_t sustain_time, float sustain_level,
                                             uint32_t release_time);
extern void sfx_synth_set_amplitude_sustain_punch(struct sfx_synth * sfx_synth, float punch);
extern void sfx_synth_set_arpeggio(struct sfx_synth * sfx_synth, float modifier, uint32_t limit);
extern void sfx_synth_set_repeater(struct sfx_synth * sfx_synth, uint32_t limit);
extern void sfx_synth_set_lowpass_filter(struct sfx_synth * sfx_synth, float filter);


#endif
