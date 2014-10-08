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

# define CLAP_VERSION_MAKE(Major, Minor, Revision) \
  ((((Major) & 0xff) << 16) | (((Minor) & 0xff) << 8) | ((Revision) & 0xff))
# define CLAP_VERSION CLAP_VERSION_MAKE(1, 0, 0)
# define CLAP_VERSION_MAJ(Version) (((Version) >> 16) & 0xff)
# define CLAP_VERSION_MIN(Version) (((Version) >> 8) & 0xff)
# define CLAP_VERSION_REV(Version) ((Version) & 0xff)

//////////////
// CHANNELS //
//////////////

enum clap_channel_type
{
  CLAP_CHANNEL_MONO,
  CLAP_CHANNEL_STEREO,
  CLAP_CHANNEL_SURROUND,
};

enum clap_channel_role
{
  CLAP_CHANNEL_INOUT,
  CLAP_CHANNEL_SIDECHAIN,
  CLAP_CHANNEL_FEEDBACK,
};

struct clap_channel
{
  enum clap_channel_type  type;
  enum clap_channel_role  role;
  char                   *name;
  uint32_t                stream_id; // used to connect feedback loops
  bool                    repeatable;
};

struct clap_channels_config
{
  uint32_t input_count;
  uint32_t output_count;

  struct clap_channel *inputs;
  struct clap_channel *output;
};

////////////////
// PARAMETERS //
////////////////

enum clap_param_type
{
  CLAP_PARAM_GROUP,
  CLAP_PARAM_BOOL,
  CLAP_PARAM_FLOAT,
  CLAP_PARAM_INT,
  CLAP_PARAM_ENUM, // uses int
};

enum clap_param_scale
{
  CLAP_PARAM_LINEAR,
  CLAP_PARAM_LOG,
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
  char                   *display_text; // use this for display if not NULL.
  union clap_param_value  value;
  union clap_param_value  min;
  union clap_param_value  max;
  enum clap_param_scale   scale;
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

enum clap_event_type
{
  CLAP_EVENT_NOTE_ON,            // note attribute
  CLAP_EVENT_NOTE_MODULATION,    // note attribute
  CLAP_EVENT_NOTE_OFF,           // note attribute

  CLAP_EVENT_PARAM_SET,          // param attribute
  CLAP_EVENT_PARAM_RAMP,         // param attribute
  CLAP_EVENT_PITCH_SET,          // diapason attribute
  CLAP_EVENT_PRESET_SET,         // preset attribute

  CLAP_EVENT_GUI_OPENED,         // plugin to host
  CLAP_EVENT_GUI_CLOSED,         // plugin to host
};

struct clap_event_note
{
  uint32_t division; // 12 for a standard octave
  uint32_t note;     // starts from 0
  float    velocity; // 0 .. 1.0f

  struct clap_event *events; // events specific to this note
};

struct clap_event_param
{
  uint32_t                index;
  union clap_param_value  value;
  float                   increment;     // for param ramp
  char                   *display_text;  // use this for display if not NULL.
  bool                    is_recordable; // used to tell the host if this event
                                         // can be recorded
};

struct clap_event_pitch
{
  float freq_hz; // usually 440Hz
};

struct clap_event_preset
{
  const char *id;
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  uint64_t              sample_offset; // offset from the parent event or current time in samples

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
  /* host custom ptr */
  void *host_data;

  /* audio buffers */
  float    **input;
  float    **output;
  uint32_t   nb_samples;

  /* feedback loops */
  void (*feedback)(struct clap_process *process, uint32_t stream_id, uint32_t nb_samples);

  /* process info */
  bool     is_offline;
  uint32_t tempo_in_samples;
  uint64_t time_in_samples;

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

  /* used for timing events */
  uint64_t (*steady_time)(struct clap_host *host);

  /* future features */
  void *(*extension)(struct clap_host *host, const char *extention_id);
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

  bool has_gui;
  bool supports_tunning;
  bool supports_microtones;

  /* audio channels */
  uint32_t                     channels_configs_count;
  struct clap_channels_config *channels_configs;
  bool (*set_channels_config)(struct clap_plugin          *plugin,
                              struct clap_channels_config *config);

  /* Returns a newly allocated parameters tree. The caller has to free it. */
  struct clap_param *(*get_params)(struct clap_plugin *plugin);

  /* Returns a newly allocated preset list. The caller has to free it. */
  struct clap_preset *(*get_presets)(struct clap_plugin *plugin);

  /* activation */
  bool (*activate)(struct clap_plugin *plugin);
  void (*deactivate)(struct clap_plugin *plugin);

  /* work */
  void (*process)(struct clap_plugin *plugin, struct clap_process *process);

  /* gui */
  bool (*open_gui)(struct clap_plugin *plugin);
  void (*close_gui)(struct clap_plugin *plugin);

  /* The plugin has to allocate and save its state into *buffer.
   * The host has to free *buffer after that. */
  void (*save)(struct clap_plugin *plugin, void **buffer, size_t *size);
  void (*restore)(struct clap_plugin *plugin, const void *buffer, size_t size);

  /* future features */
  void *(*extension)(struct clap_plugin *plugin, const char *extention_id);
};

/* typedef for dlsym() cast */
typedef struct clap_plugin *(clap_create_f)(uint32_t          plugin_index,
                                            struct clap_host *host,
                                            uint32_t          sample_rate,
                                            uint32_t         *plugin_count);

/* plugin entry point */
struct clap_plugin *
clap_create(uint32_t          plugin_index,
            struct clap_host *host,
            uint32_t          sample_rate,
            uint32_t         *plugin_count);

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_H */
