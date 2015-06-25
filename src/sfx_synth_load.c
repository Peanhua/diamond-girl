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
#include "sfx_synth.h"
#include <assert.h>
#include <json.h>

/* Parameters structure for process_* functions */
struct params
{
  const char *   name;
  enum json_type type;
  void *         value_ptr;
  bool           handled;
};

static bool get_params(const char * func, struct json_object * obj, struct params params[]);
static bool process_new(struct json_object * obj);
static bool process_set_amplitude_envelope(struct json_object * obj);
static bool process_set_amplitude_sustain_punch(struct json_object * obj);
static bool process_set_arpeggio(struct json_object * obj);
static bool process_set_frequency_slide(struct json_object * obj);
static bool process_set_low_pass_filter(struct json_object * obj);


static struct sfx_synth * synth;
static char current_filename[1024];


struct sfx_synth * sfx_synth_load(const char * filename)
{
  char * json;
  int    json_size;

  synth = NULL;
  snprintf(current_filename, sizeof current_filename, "%s", get_data_filename(filename));
  if(read_file(current_filename, &json, &json_size) == true)
    {
      enum json_tokener_error e;
      struct json_object * obj;

      obj = json_tokener_parse_verbose(json, &e);
      if(obj != NULL)
        {
          struct array_list * array;

          array = json_object_get_array(obj);
          assert(array != NULL);
          if(array != NULL)
            {
              int array_len;
              bool ok;
              
              ok = true;
              array_len = json_object_array_length(obj);
              for(int i = 0; ok == true && i < array_len; i++)
                {
                  struct json_object * item;

                  item = json_object_array_get_idx(obj, i);
                  assert(item != NULL);
                  if(item != NULL)
                    {
                      struct json_object_iterator it;
                      struct json_object_iterator itEnd;

                      ok    = true;
                      it    = json_object_iter_begin(item);
                      itEnd = json_object_iter_end(item);

                      while(ok == true && !json_object_iter_equal(&it, &itEnd))
                        {
                          const char *         name;
                          struct json_object * value;
              
                          name  = json_object_iter_peek_name(&it);
                          value = json_object_iter_peek_value(&it);
              
                          struct
                          {
                            const char * name;
                            bool (*handler)(struct json_object *);
                          } handlers[] =
                              {
                                { "New",                      process_new                         },
                                { "SetAmplitudeEnvelope",     process_set_amplitude_envelope      },
                                { "SetAmplitudeSustainPunch", process_set_amplitude_sustain_punch },
                                { "SetArpeggio",              process_set_arpeggio                },
                                { "SetFrequencySlide",        process_set_frequency_slide         },
                                { "SetLowPassFilter",         process_set_low_pass_filter         },
                                { NULL, NULL }
                              };
                          
                          int ind;
                          
                          ind = -1;
                          for(int j = 0; ind == -1 && handlers[j].name != NULL; j++)
                            if(!strcmp(handlers[j].name, name))
                              ind = j;
                          
                          if(ind >= 0)
                            {
                              ok = handlers[ind].handler(value);
                              assert(ok == true);
                            }
                          else
                            {
                              fprintf(stderr, "%s: Failed to parse: unknown handler '%s'.\n", current_filename, name);
                              ok = false;
                            }
                          
                          json_object_iter_next(&it);
                        }
                    }
                }
            
              if(ok == false)
                if(synth != NULL)
                  synth = sfx_synth_free(synth);
            }
          else
            {
              fprintf(stderr, "%s: Failed to obtain the main array.\n", current_filename);
            }
        }
      else
        {
          fprintf(stderr, "%s: Failed to parse: %d: %s\n", current_filename, (int) e, json_tokener_error_desc(e));
        }
      free(json);
    }
  else
    {
      fprintf(stderr, "%s: Failed to read '%s'.\n", __FUNCTION__, current_filename);
    }

  return synth;
}



static bool get_params(const char * func, struct json_object * obj, struct params params[])
{
  bool rv;

  rv = true;
  assert(obj != NULL);
  if(json_object_get_type(obj) == json_type_object)
    {
      struct json_object_iterator it;
      struct json_object_iterator itEnd;

      for(int i = 0; params[i].name != NULL; i++)
        params[i].handled = false;

      it    = json_object_iter_begin(obj);
      itEnd = json_object_iter_end(obj);
      while(rv == true && !json_object_iter_equal(&it, &itEnd))
        {
          const char *         v_name;
          struct json_object * v_value;
          enum json_type       v_type;
                  
          v_name  = json_object_iter_peek_name(&it);
          v_value = json_object_iter_peek_value(&it);
          v_type  = json_object_get_type(v_value);

          int ind;

          ind = -1;
          for(int i = 0; ind == -1 && params[i].name != NULL; i++)
            if(!strcmp(v_name, params[i].name))
              ind = i;

          if(ind >= 0)
            {
              assert(params[ind].value_ptr != NULL);
              if(v_type == params[ind].type)
                {
                  switch(v_type)
                    {
                    case json_type_string:
                      {
                        char ** ptr;

                        ptr = params[ind].value_ptr;
                        *ptr = strdup(json_object_get_string(v_value));
                      }
                      break;
                    case json_type_int:
                      {
                        int * ptr;

                        ptr = params[ind].value_ptr;
                        *ptr = json_object_get_int(v_value);
                      }
                      break;
                    case json_type_double:
                      {
                        double * ptr;

                        ptr = params[ind].value_ptr;
                        *ptr = json_object_get_double(v_value);
                      }
                      break;
                    default:
                      assert(false);
                      break;
                    }
                  params[ind].handled = true;
                }
              else
                {
                  fprintf(stderr, "%s: In %s: Wrong type '%s' for parameter '%s', expected '%s'.\n", func, current_filename, json_type_to_name(v_type), v_name, json_type_to_name(params[ind].type));
                  rv = false;
                }
            }

          json_object_iter_next(&it);
        }

      for(int i = 0; rv == true && params[i].name != NULL; i++)
        if(params[i].handled == false)
          rv = false;
    }
  else
    {
      fprintf(stderr, "%s: In %s: Wrong object type '%s' for parameters, expected '%s'.\n", func, current_filename, json_type_to_name(json_object_get_type(obj)), json_type_to_name(json_type_object));
      rv = false;
    }

  return rv;
}


static bool process_new(struct json_object * obj)
{
  bool rv;
  char * p_type;
  int    p_length;
  int    p_frequency;
  struct params pars[] =
    {
      { "Type",      json_type_string, &p_type,      false },
      { "Length",    json_type_int,    &p_length,    false },
      { "Frequency", json_type_int,    &p_frequency, false },
      { NULL,        json_type_int,    NULL,         false }
    };

  rv = get_params("New", obj, pars);
  if(rv == true)
    {
      struct
      {
        char *                  name;
        enum SFX_SYNTH_WAVEFORM type;
      } typenames[] =
          {
            { "square",           SFX_SYNTH_WAVEFORM_SQUARE           },
            { "sawtooth",         SFX_SYNTH_WAVEFORM_SAWTOOTH         },
            { "inverse_sawtooth", SFX_SYNTH_WAVEFORM_INVERSE_SAWTOOTH },
            { "sine",             SFX_SYNTH_WAVEFORM_SINE             },
            { "noise",            SFX_SYNTH_WAVEFORM_NOISE            },
            { "triangle",         SFX_SYNTH_WAVEFORM_TRIANGLE         },
            { NULL,               SFX_SYNTH_WAVEFORM_SIZEOF_          }
          };
      enum SFX_SYNTH_WAVEFORM w;

      w = SFX_SYNTH_WAVEFORM_SIZEOF_;
      for(int i = 0; w == SFX_SYNTH_WAVEFORM_SIZEOF_ && typenames[i].name != NULL; i++)
        if(!strcmp(typenames[i].name, p_type))
          w = typenames[i].type;

      assert(w != SFX_SYNTH_WAVEFORM_SIZEOF_);

      struct sfx_synth * new;
  
      new = sfx_synth_new(w, p_length, p_frequency);
      new->next = synth; /* Actually it's "prev", but it does not matter when mixing all together. */
      synth = new;

      free(p_type);
    }

  return rv;
}



static bool process_set_amplitude_envelope(struct json_object * obj)
{
  int    p_delay;
  int    p_attack;
  int    p_decay;
  int    p_sustain;
  double p_sustain_level;
  int    p_release;
  bool rv;
  struct params pars[] =
    {
      { "Delay",        json_type_int,    &p_delay,         false },
      { "Attack",       json_type_int,    &p_attack,        false },
      { "Decay",        json_type_int,    &p_decay,         false },
      { "Sustain",      json_type_int,    &p_sustain,       false },
      { "SustainLevel", json_type_double, &p_sustain_level, false },
      { "Release",      json_type_int,    &p_release,       false },
      { NULL,           json_type_int,    NULL,          false }
    };

  rv = get_params("SetAmplitudeEnvelope", obj, pars);
  if(rv == true)
    sfx_synth_set_amplitude_envelope(synth, p_delay, p_attack, p_decay, p_sustain, p_sustain_level, p_release);

  return rv;
}


static bool process_set_amplitude_sustain_punch(struct json_object * obj)
{
  double p_punch;
  bool rv;
  struct params pars[] =
    {
      { "Punch", json_type_double, &p_punch, false },
      { NULL,    json_type_int,    NULL,     false }
    };

  rv = get_params("SetAmplitudeSustainPunch", obj, pars);
  if(rv == true)
    sfx_synth_set_amplitude_sustain_punch(synth, p_punch);

  return rv;
}


static bool process_set_arpeggio(struct json_object * obj)
{
  double p_modifier;
  int    p_limit;
  bool rv;
  struct params pars[] =
    {
      { "Modifier", json_type_double, &p_modifier, false },
      { "Limit",    json_type_int,    &p_limit,    false },
      { NULL,       json_type_int,    NULL,        false }
    };

  rv = get_params("SetArpeggio", obj, pars);
  if(rv == true)
    sfx_synth_set_arpeggio(synth, p_modifier, p_limit);

  return rv;
}


static bool process_set_frequency_slide(struct json_object * obj)
{
  double p_slide;
  double p_delta;
  int    p_limit;
  bool rv;
  struct params pars[] =
    {
      { "Slide", json_type_double, &p_slide, false },
      { "Delta", json_type_double, &p_delta, false },
      { "Limit", json_type_int,    &p_limit, false },
      { NULL,    json_type_int,    NULL,     false }
    };

  rv = get_params("SetFrequencySlide", obj, pars);
  if(rv == true)
    sfx_synth_set_frequency_slide(synth, p_slide, p_delta, p_limit);
  
  return rv;
}


static bool process_set_low_pass_filter(struct json_object * obj)
{
  double p_filter;
  bool rv;
  struct params pars[] =
    {
      { "Filter", json_type_double, &p_filter, false },
      { NULL,     json_type_int,    NULL,      false }
    };

  rv = get_params("SetLowPassFilter", obj, pars);
  if(rv == true)
    sfx_synth_set_lowpass_filter(synth, p_filter);

  return rv;
}
