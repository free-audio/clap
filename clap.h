/**
 * CLAP - CLever Audio Plugin (<--- needs to find a marketing ok name)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyright (c) 2014 Alexandre BIQUE
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CLAP_H
# define CLAP_H

# include <stdint.h>

# define CLAP_VERSION_MAKE(Major, Minor, Revision) (((Major) << 16) | ((Minor) << 8) | (Revision))
# define CLAP_VERSION CLAP_VERSION_MAKE(1, 0, 0)

struct clap_host
{
  uint32_t clap_version; // initialized to CALP_VERSION

  const char *name;
  const char *manufacturer;
  const char *version;

  uint32_t (*gui_opened)(struct clap_host *host, struct clap_plugin *plugin);
  uint32_t (*gui_closed)(struct clap_host *host, struct clap_plugin *plugin);
};

enum clap_param_type
{
  CLAP_PARAM_BOOL,
  CLAP_PARAM_FLOAT,
  CLAP_PARAM_STRING,
  CLAP_PARAM_GROUP,
};

struct clap_param_info
{
  enum clap_param_type  type;
  const char           *id;   // a string which identify the param
  const char           *name; // the display name
  const char           *desc;
  bool                  is_per_note;

  uint32_t              group_begin; // index of the first child of the group
  uint32_t              group_end;   // index of the last child of the group
};

union clap_param_value
{
  bool                b;
  float               f;
  int32_t             i;
  const char         *s;
};

struct clap_preset_info
{
  const char *id;
  const char *name;
  const char *desc;
  const char **tags; // null terminated array of tags
};

struct clap_event_note
{
  uint16_t division; // 12 for a standard octave
  uint16_t note;     // starts from 0
  uint16_t velocity; // 0 .. UINT16_MAX

  struct clap_event *events; // events specific to this note
};

struct clap_event_param
{
  uint32_t index;
  union clap_param_value value;
};

struct clap_event_diapason
{
  float freq_hz; // usually 440Hz
};

struct clap_event_preset
{
  const char *id;
};

enum clap_event_type
{
  CLAP_EVENT_NOTE_ON,
  CLAP_EVENT_NOTE_PRESSURE,
  CLAP_EVENT_NOTE_OFF,

  CLAP_EVENT_PARAM_SET,
  CLAP_EVENT_PRESET_SET,
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  uint32_t              sample_offset; // offset from the parent event or current time in samples

  union {
    struct clap_event_note     note;
    struct clap_event_param    param;
    struct clap_event_diapason diapason;
    struct clap_event_preset   preset;
  };
};

struct clap_process
{
  /* audio buffers, they must be aligned on the machine's best vector instructions requirement */
  float    **input;
  float    **output;
  uint32_t   nb_samples;

  /* host info */
  bool     is_offline;
  uint32_t tempo_in_samples;
  uint32_t time_in_samples;

  /* events */
  struct clap_event *in_events;
  struct clap_event *out_events;

  /* output values */
  bool need_processing;
};

// bitfield
enum clap_plugin_type
{
  CLAP_PLUGIN_INSTRUMENT  = (1 << 0),
  CLAP_PLUGIN_EFFECT      = (1 << 1),
  CLAP_PLUGIN_MIDI_EFFECT = (1 << 2),
  CLAP_PLUGIN_ANALYZER    = (1 << 3),
};

struct clap_plugin
{
  uint32_t clap_version; // initialized to CALP_VERSION

  void *host_data;
  void *plugin_data;

  /* free plugin's resources */
  void (*destroy)(struct clap_plugin *plugin);

  /* plugin info */
  const char *name;
  const char *description;
  const char *manufacturer;
  const char *version;
  uint32_t plugin_type;
  uint32_t inputs_count;
  uint32_t outputs_count;

  /* parameters */
  uint32_t (*get_param_count)(struct clap_plugin *plugin);
  void (*get_param_info)(struct clap_plugin *plugin, uint32_t index, struct clap_param_info *param);

  /* presets */
  uint32_t (*get_preset_count)(struct clap_plugin *plugin);
  void (*get_preset_info)(struct clap_plugin *plugin, uint32_t index, struct clap_preset_info *preset);

  /* activation */
  void (*activate)(struct clap_plugin *plugin);
  void (*deactivate)(struct clap_plugin *plugin);

  /* work */
  void (*process)(struct clap_plugin *plugin, struct clap_process *process);

  /* gui */
  void (*open_gui)(struct clap_plugin *plugin);
  void (*close_gui)(struct clap_plugin *plugin);
};

struct clap_plugin *
clap_create(uint32_t plugin_index, struct clap_host *host, uint32_t sample_rate);

#endif /* !CLAP_H */
