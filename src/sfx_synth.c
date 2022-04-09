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

#include "sfx_synth.h"
#include "random.h"

#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void  restart_effect_state(struct sfx_synth * sfx_synth, bool completely);
static void  square_slide(struct sfx_synth * sfx_synth);
static void  frequency_slide(struct sfx_synth * sfx_synth);
static float amplitude_envelope(struct sfx_synth * sfx_synth, float sample);
static float base_waveform(struct sfx_synth * sfx_synth);
static float arpeggio(struct sfx_synth * sfx_synth, float sample);
static float lowpass_filter(struct sfx_synth * sfx_synth, float sample);
static void  repeater(struct sfx_synth * sfx_synth);
static bool  next_sample(struct sfx_synth * sfx_synth);

/* Generic DADSR envelope function. */
static float dadsr_envelope(struct sfx_synth * sfx_synth, struct sfx_synth_dadsr_envelope * envelope);



struct sfx_synth * sfx_synth_new(enum SFX_SYNTH_WAVEFORM waveform, uint32_t length, uint16_t frequency_hz)
{
  struct sfx_synth * rv;

  rv = malloc(sizeof(struct sfx_synth));
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->waveform.type   = waveform;

      if(frequency_hz > 0)
        rv->waveform.period = 44100.0f / (float) frequency_hz;
      else
        rv->waveform.period = 0.0f;
      if(rv->waveform.period < 4.0f)
        rv->waveform.period = 4.0f;

      rv->amplitude_envelope.delay_time    = 0;
      rv->amplitude_envelope.attack_time   = 0;
      rv->amplitude_envelope.decay_time    = 0;
      rv->amplitude_envelope.sustain_time  = length;
      rv->amplitude_envelope.sustain_level = 1.0f;
      rv->amplitude_envelope.sustain_punch = 0.0f;
      rv->amplitude_envelope.release_time  = 0;

      rv->square.duty_cycle = 0.5f;
      rv->square.slide      = 0.0f;
      
      rv->frequency_slide.active      = false;
      rv->frequency_slide.slide       = 0;
      rv->frequency_slide.slide_delta = 0;
      rv->frequency_slide.limit       = 0;
      
      rv->arpeggio.limit = 0;
      rv->arpeggio.mod   = 0;
      
      rv->repeater.limit = 0;

      rv->lowpass_filter = 1.0f;
      
      restart_effect_state(rv, true);

      rv->next           = NULL;
    }

  return rv;
}


struct sfx_synth * sfx_synth_free(struct sfx_synth * sfx_synth)
{
  while(sfx_synth != NULL)
    {
      struct sfx_synth * n;

      n = sfx_synth->next;
      free(sfx_synth);
      sfx_synth = n;
    }

  return NULL;
}


uint32_t sfx_synth_get_samples(struct sfx_synth * sfx_synth, int16_t * buffer, uint32_t max_amount)
{
  uint32_t count;
  bool running;

  count = 0;
  running = true;
  while(running == true && count < max_amount && count < 44100 * 60)
    {
      struct sfx_synth * current;
      float mixed_sample;

      mixed_sample = 0.0f;
      running = false;
      current = sfx_synth;
      while(current != NULL)
        {
          if(current->state.running == true)
            {
              float sample;

              sample = base_waveform(current);
              sample = arpeggio(current, sample);
              sample = lowpass_filter(current, sample);
              sample = amplitude_envelope(current, sample);
          
              if(next_sample(current) == true)
                {
                  repeater(current);
                  square_slide(current);
                  frequency_slide(current);

                  if(current->state.period < 4.0f || current->state.period > 5000.0f)
                    current->state.running = false;
                }
          
              if(sample < -1.0f)
                {
                  //sample = -1.0f;
                  current->state.clipped = true;
                }
              else if(sample > 1.0f)
                {
                  //sample = 1.0f;
                  current->state.clipped = true;
                }

              mixed_sample += sample;

              if(current->state.running == true)
                running = true;
            }
          current = current->next;
        }

      if(mixed_sample < -1.0f)
        mixed_sample = -1.0f;
      else if(mixed_sample > 1.0f)
        mixed_sample = 1.0f;

      buffer[count] = mixed_sample * 32767;
      if(running == true)
        count++;
    }

  return count;
}


void sfx_synth_restart(struct sfx_synth * sfx_synth)
{
  while(sfx_synth != NULL)
    {
      restart_effect_state(sfx_synth, true);
      sfx_synth = sfx_synth->next;
    }
}



void sfx_synth_set_square_slide(struct sfx_synth * sfx_synth, float slide)
{
  sfx_synth->square.slide       = slide;
  sfx_synth->state.square_slide = slide;
}


void sfx_synth_set_frequency_slide(struct sfx_synth * sfx_synth, float slide, float delta, int limit)
{
  sfx_synth->frequency_slide.active      = true;
  sfx_synth->frequency_slide.slide       = slide;
  sfx_synth->frequency_slide.slide_delta = delta;
  sfx_synth->frequency_slide.limit       = limit;
  sfx_synth->state.frequency_slide       = sfx_synth->frequency_slide.slide;
}


void sfx_synth_set_amplitude_envelope(struct sfx_synth * sfx_synth,
                                      uint32_t delay_time,
                                      uint32_t attack_time,
                                      uint32_t decay_time,
                                      uint32_t sustain_time, float sustain_level,
                                      uint32_t release_time)
{
  sfx_synth->amplitude_envelope.delay_time    = delay_time;
  sfx_synth->amplitude_envelope.attack_time   = attack_time;
  sfx_synth->amplitude_envelope.decay_time    = decay_time;
  sfx_synth->amplitude_envelope.sustain_time  = sustain_time;
  sfx_synth->amplitude_envelope.sustain_level = sustain_level;
  sfx_synth->amplitude_envelope.release_time  = release_time;
}


void sfx_synth_set_amplitude_sustain_punch(struct sfx_synth * sfx_synth, float punch)
{
  sfx_synth->amplitude_envelope.sustain_punch = punch;
}




void sfx_synth_set_arpeggio(struct sfx_synth * sfx_synth, float modifier, uint32_t limit)
{
  sfx_synth->arpeggio.limit        = limit;
  sfx_synth->arpeggio.mod          = modifier;
  sfx_synth->state.arpeggio_active = limit > 0 ? true : false;
}


void sfx_synth_set_repeater(struct sfx_synth * sfx_synth, uint32_t limit)
{
  sfx_synth->repeater.limit = limit;
}


void sfx_synth_set_lowpass_filter(struct sfx_synth * sfx_synth, float filter)
{
  sfx_synth->lowpass_filter = filter;
}




/* Local functions. These modify only the state. */
static void restart_effect_state(struct sfx_synth * sfx_synth, bool completely)
{
  if(completely == true)
    {
      sfx_synth->state.running                 = true;
      sfx_synth->state.clipped                 = false;
      sfx_synth->state.lowpass_filter_previous = 0.0f;
    }

  if(sfx_synth->state.running == true)
    {
      sfx_synth->state.period                = sfx_synth->waveform.period;
      sfx_synth->state.arpeggio_active       = sfx_synth->arpeggio.limit > 0 ? true : false;
      sfx_synth->state.square_duty_cycle     = sfx_synth->square.duty_cycle;
      sfx_synth->state.square_slide          = sfx_synth->square.slide;
      sfx_synth->state.frequency_slide       = sfx_synth->frequency_slide.slide;
      sfx_synth->state.frequency_slide_timer = sfx_synth->frequency_slide.slide;
      sfx_synth->state.sample_number         = 0;
      sfx_synth->state.phase                 = 0;
      sfx_synth->state.arpeggio_timer        = 0;
      sfx_synth->state.repeat_timer          = 0;
    }
}



static void square_slide(struct sfx_synth * sfx_synth)
{
  if(sfx_synth->state.running == true)
    if(sfx_synth->waveform.type == SFX_SYNTH_WAVEFORM_SQUARE)
      {
        sfx_synth->state.square_duty_cycle += sfx_synth->state.square_slide;
        if(sfx_synth->state.square_duty_cycle < 0.0f)
          {
            sfx_synth->state.square_duty_cycle = 0.0f;
            sfx_synth->state.square_slide = -sfx_synth->state.square_slide;
          }
        else if(sfx_synth->state.square_duty_cycle > 0.5f)
          {
            sfx_synth->state.square_duty_cycle = 0.5f;
            sfx_synth->state.square_slide = -sfx_synth->state.square_slide;
          }
      }
}

static void frequency_slide(struct sfx_synth * sfx_synth)
{
  if(sfx_synth->state.running == true)
    if(sfx_synth->frequency_slide.active == true)
      {
        sfx_synth->state.frequency_slide += sfx_synth->frequency_slide.slide_delta;
        sfx_synth->state.period          *= sfx_synth->state.frequency_slide;

        sfx_synth->state.frequency_slide_timer++;
        if(sfx_synth->frequency_slide.limit > 0)
          if(sfx_synth->state.frequency_slide_timer >= sfx_synth->frequency_slide.limit)
            sfx_synth->frequency_slide.active = false;
      }
}



static float base_waveform(struct sfx_synth * sfx_synth)
{
  float sample;

  sample = 0.0f;

  if(sfx_synth->state.running == true)
    {
      float current;

      current = (float) sfx_synth->state.phase / sfx_synth->state.period;

      switch(sfx_synth->waveform.type)
        {
        case SFX_SYNTH_WAVEFORM_SQUARE:
          if(sfx_synth->state.square_duty_cycle > 0.0f)
            {
              if(current < sfx_synth->state.square_duty_cycle)
                sample =  1.0f;
              else
                sample = -1.0f;
            }
          else
            {
              sample = 0.0f;
              sfx_synth->state.running = false;
            }
          break;

        case SFX_SYNTH_WAVEFORM_SAWTOOTH:
          if(current < 0.5f)
            sample = current * 2.0f;
          else
            sample = -2.0f + current * 2.0f;
          break;

        case SFX_SYNTH_WAVEFORM_INVERSE_SAWTOOTH:
          if(current < 0.5f)
            sample = -current * 2.0f;
          else
            sample = 2.0f - current * 2.0f;
          break;

        case SFX_SYNTH_WAVEFORM_TRIANGLE:
          if(current < 0.25f)
            sample = current * 4.0f;
          else if(current < 0.5f)
            sample = 1.0f - (current - 0.25f) * 4.0f;
          else if(current < 0.75f)
            sample = 0.0f - (current - 0.5f) * 4.0f;
          else /* if(current <= 1.0f) */
            sample = (current - 0.75f) * 4.0f - 1.0f;
          break;

        case SFX_SYNTH_WAVEFORM_SINE:
          sample = sinf(current * 2.0f * M_PI);
          break;

        case SFX_SYNTH_WAVEFORM_NOISE:
          sample = ((float) get_rand(100000) - 50000.0f) / 50000.0f;
          assert(sample >= -1.0f);
          assert(sample <= 1.0f);
          break;

        case SFX_SYNTH_WAVEFORM_SIZEOF_:
          assert(0);
          break;
        }
    }

  return sample;
}

static float amplitude_envelope(struct sfx_synth * sfx_synth, float sample)
{
  if(sfx_synth->state.running == true)
    {
      float amplitude;

      amplitude = dadsr_envelope(sfx_synth, &sfx_synth->amplitude_envelope);
      sample *= amplitude;
    }

  return sample;
}




static float dadsr_envelope(struct sfx_synth * sfx_synth, struct sfx_synth_dadsr_envelope * envelope)
{
  float rv;

  rv = 0.0f;
  if(sfx_synth->state.running == true)
    {
      uint32_t n;

      n = sfx_synth->state.sample_number;
      if(n < envelope->delay_time)
        { /* Delay */
          rv = 0.0f;
        }
      else
        {
          n -= envelope->delay_time;
          if(n < envelope->attack_time)
            { /* Attack */
              rv = (float) n / (float) envelope->attack_time;
            }
          else
            {
              n -= envelope->attack_time;
              if(n < envelope->decay_time)
                { /* Decay */
                  rv = 1.0f - (1.0f - envelope->sustain_level) * (float) n / (float) envelope->decay_time;
                }
              else
                {
                  n -= envelope->decay_time;
                  if(n < envelope->sustain_time)
                    { /* Sustain */
                      rv = envelope->sustain_level * (1.0f + envelope->sustain_punch);
                    }
                  else
                    { /* Release */
                      n -= envelope->sustain_time;
                      if(n < envelope->release_time)
                        {
                          rv = (1.0f - (float) n / (float) envelope->release_time) * envelope->sustain_level;
                        }
                      else
                        {
                          rv = 0.0f;
                          sfx_synth->state.running = false;
                        }
                    }
                }
            }
        }
    }

  return rv;
}

static float arpeggio(struct sfx_synth * sfx_synth, float sample)
{
  if(sfx_synth->state.running == true)
    if(sfx_synth->state.arpeggio_active == true)
      if(sfx_synth->state.arpeggio_timer == sfx_synth->arpeggio.limit - 1)
        sfx_synth->state.period *= sfx_synth->arpeggio.mod;
  
  return sample;
}

static void repeater(struct sfx_synth * sfx_synth)
{
  if(sfx_synth->state.running == true)
    if(sfx_synth->repeater.limit > 0)
      {
        sfx_synth->state.repeat_timer++;

        if(sfx_synth->state.repeat_timer >= sfx_synth->repeater.limit)
          restart_effect_state(sfx_synth, false);
      }
}


static float lowpass_filter(struct sfx_synth * sfx_synth, float sample)
{
  if(sfx_synth->state.running == true)
    if(sfx_synth->lowpass_filter < 1.0f)
      {
        sample = sfx_synth->state.lowpass_filter_previous + sfx_synth->lowpass_filter * (sample - sfx_synth->state.lowpass_filter_previous);
        sfx_synth->state.lowpass_filter_previous = sample;
      }

  return sample;
}


static bool next_sample(struct sfx_synth * sfx_synth)
{
  bool next_period;

  next_period = false;
  if(sfx_synth->state.running == true)
    {
      sfx_synth->state.sample_number++;

      sfx_synth->state.phase++;
      if(sfx_synth->state.phase >= sfx_synth->state.period)
        {
          sfx_synth->state.phase = 0;
          next_period = true;
        }
      
      if(sfx_synth->state.arpeggio_active == true)
        {
          sfx_synth->state.arpeggio_timer++;
          if(sfx_synth->state.arpeggio_timer >= sfx_synth->arpeggio.limit)
            sfx_synth->state.arpeggio_active = false;
        }
    }

  return next_period;
}
