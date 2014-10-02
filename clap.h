/**
 * CLAP - CLever Audio Plugin (<--- needs to find a marketing ok name)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyright (c) 2014 Alexandre BIQUE <bique.alexandre@gmail.com>
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

# ifdef __cplusplus
extern "C" {
# endif

# include <stdint.h>

# define CLAP_VERSION_MAKE(Major, Minor, Revision) (((Major) << 16) | ((Minor) << 8) | (Revision))
# define CLAP_VERSION CLAP_VERSION_MAKE(1, 0, 0)

//////////////
// CHANNELS //
//////////////

enum clap_audio_stream_type
{
  CLAP_AUDIO_STREAM_MONO,
  CLAP_AUDIO_STREAM_STEREO,
  CLAP_AUDIO_STREAM_SURROUND,
};

enum clap_audio_stream_role
{
  CLAP_AUDIO_STREAM_INOUT,
  CLAP_AUDIO_STREAM_SIDECHAIN,
  CLAP_AUDIO_STREAM_FEEDBACK,
};

struct clap_channel
{
  /* linked list */
  struct clap_channel *next;

  /* channel info */
  enum clap_channel_type  type;
  const char             *name;
  uint32_t                stream_id; // used to connect feedback loops
};

struct clap_channels_config
{
  /* linked list */
  const struct clap_channel_configs *next;

  /* config */
  const struct clap_channel *inputs;
  const struct clap_channel *output;
};

////////////////
// PARAMETERS //
////////////////

enum clap_param_type
{
  CLAP_PARAM_BOOL,
  CLAP_PARAM_FLOAT,
  CLAP_PARAM_INT,
};

union clap_param_value
{
  bool                b;
  float               f;
  int32_t             i;
};

struct clap_param
{
  /* tree field */
  struct clap_param *next;
  struct clap_param *childs;

  /* param info */
  enum clap_param_type    type;
  char                   *id;   // a string which identify the param
  char                   *name; // the display name
  char                   *desc;
  bool                    is_per_note;
  union clap_param_value  value;
};

/////////////
// PRESETS //
/////////////

struct clap_preset
{
  struct clap_preset *next;

  char *id;
  char *name; // display name
  char *desc; // desc and how to use it
  char *author;
  char **tags; // null terminated array of tags
};

////////////
// EVENTS //
////////////

struct clap_event_note
{
  uint16_t division; // 12 for a standard octave
  uint16_t note;     // starts from 0
  float    velocity; // 0 .. 1.0f

  struct clap_event *events; // events specific to this note
};

struct clap_event_param
{
  uint32_t               index;
  union clap_param_value value;
  float                  increment; // for param ramp
};

struct clap_event_pitch
{
  float freq_hz; // usually 440Hz
};

struct clap_event_preset
{
  const char *id;
};

enum clap_event_type
{
  CLAP_EVENT_NOTE_ON,            // note attribute
  CLAP_EVENT_NOTE_MODULATION,    // note attribute
  CLAP_EVENT_NOTE_OFF,           // note attribute

  CLAP_EVENT_PARAM_SET,          // param attribute
  CLAP_EVENT_PARAM_RAMP,         // param attribute
  CLAP_EVENT_PITCH_SET,          // diapason attribute
  CLAP_EVENT_PRESET_SET,         // preset attribute
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  uint32_t              sample_offset; // offset from the parent event or current time in samples

  union {
    struct clap_event_note     note;
    struct clap_event_param    param;
    struct clap_event_pitch    pitch;
    struct clap_event_preset   preset;
  };
};

/////////////
// PROCESS //
/////////////

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

//////////
// HOST //
//////////

struct clap_host
{
  uint32_t clap_version; // initialized to CALP_VERSION

  /* host info */
  const char *name;
  const char *manufacturer;
  const char *version;

  /* for events generated outside of process, like from the GUI. */
  void (*events)(struct clap_host   *host,
                 struct clap_plugin *plugin,
                 struct clap_event  *events);

  /* future features */
  void *(*extention)(struct clap_plugin *plugin, const char *extention_id);
};

////////////
// PLUGIN //
////////////

// bitfield
# define CLAP_PLUGIN_INSTRUMENT  (1 << 0)
# define CLAP_PLUGIN_EFFECT      (1 << 1)
# define CLAP_PLUGIN_MIDI_EFFECT (1 << 2)
# define CLAP_PLUGIN_ANALYZER    (1 << 3)

struct clap_plugin
{
  uint32_t clap_version; // initialized to CALP_VERSION

  void *host_data;   // reserved pointer for the host
  void *plugin_data; // reserved pointer for the plugin

  /* free plugin's resources */
  void (*destroy)(struct clap_plugin *plugin);

  /* plugin info */
  const char  *id;
  const char  *name;
  const char  *description;
  const char  *manufacturer;
  const char  *version;
  const char  *url;
  const char  *license;
  const char  *support;         // a link to the support
  const char **caterogries;     // fm, analogue, delay, reverb, ...
  uint32_t     plugin_type;

  /* audio channels */
  const struct clap_channels_config *channel_configs;
  void (*set_channels_config)(struct clap_plugin                *plugin,
                              const struct clap_channels_config *config);

  /* parameters */
  struct clap_param *(*get_params)(struct clap_plugin *plugin);

  /* params */
  struct clap_preset *(*get_presets)(struct clap_plugin *plugin);

  /* activation */
  bool (*activate)(struct clap_plugin *plugin);
  void (*deactivate)(struct clap_plugin *plugin);

  /* work */
  void (*process)(struct clap_plugin *plugin, struct clap_process *process);

  /* gui */
  void (*open_gui)(struct clap_plugin *plugin);
  void (*close_gui)(struct clap_plugin *plugin);

  /* state */
  void (*save)(struct clap_plugin *plugin, void **buffer, size_t *size);
  void (*restore)(struct clap_plugin *plugin, const void *buffer, size_t size);

  /* future features */
  void *(*extention)(struct clap_plugin *plugin, const char *extention_id);
};

/* typedef for dlsym() cast */
typedef struct clap_plugin *(clap_create_f)(uint32_t          plugin_index,
                                            struct clap_host *host,
                                            uint32_t          sample_rate);

/* plugin entry point */
struct clap_plugin *
clap_create(uint32_t plugin_index, struct clap_host *host, uint32_t sample_rate);

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_H */
