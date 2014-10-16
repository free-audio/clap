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

# include <stdbool.h>
# include <stdint.h>

# define CLAP_VERSION_MAKE(Major, Minor, Revision) \
  ((((Major) & 0xff) << 16) | (((Minor) & 0xff) << 8) | ((Revision) & 0xff))
# define CLAP_VERSION CLAP_VERSION_MAKE(1, 0, 0)
# define CLAP_VERSION_MAJ(Version) (((Version) >> 16) & 0xff)
# define CLAP_VERSION_MIN(Version) (((Version) >> 8) & 0xff)
# define CLAP_VERSION_REV(Version) ((Version) & 0xff)

///////////////////////////
// FORWARD DELCLARATIONS //
///////////////////////////

struct clap_plugin;
struct clap_host;

enum clap_string_size
{
  CLAP_ID_SIZE         = 32,
  CLAP_NAME_SIZE       = 32,
  CLAP_DESC_SIZE       = 256,
  CLAP_DISPLAY_SIZE    = 32,
  CLAP_TAGS_SIZE       = 64,
  CLAP_URL_SIZE        = 256,
};

///////////
// PORTS //
///////////

enum clap_port_type
{
  CLAP_PORT_MONO     = 0,
  CLAP_PORT_STEREO   = 1,
  CLAP_PORT_SURROUND = 2,
};

enum clap_port_role
{
  CLAP_PORT_INOUT     = 0,
  CLAP_PORT_SIDECHAIN = 1,
  CLAP_PORT_FEEDBACK  = 2,
};

struct clap_port_info
{
  enum clap_port_type  type;
  enum clap_port_role  role;
  char                 name[CLAP_NAME_SIZE];
  uint32_t             stream_id; // used to connect feedback loops
  bool                 is_repeatable;
};

struct clap_ports_config
{
  char     name[CLAP_NAME_SIZE];
  uint32_t inputs_count;
  uint32_t outputs_count;
};

////////////////
// PARAMETERS //
////////////////

enum clap_param_type
{
  CLAP_PARAM_GROUP = 0, // no value for this one
  CLAP_PARAM_BOOL  = 1, // uses value.b
  CLAP_PARAM_FLOAT = 2, // uses value.f
  CLAP_PARAM_INT   = 3, // uses value.i
  CLAP_PARAM_ENUM  = 4, // uses value.i
};

enum clap_param_scale
{
  CLAP_PARAM_LINEAR = 0,
  CLAP_PARAM_LOG    = 1,
};

union clap_param_value
{
  bool    b;
  float   f;
  int32_t i;
};

struct clap_param
{
  /* tree fields */
  uint32_t index;  // parameter's index
  uint32_t parent; // parent's index

  /* param info */
  enum clap_param_type    type;
  char                    id[32];   // a string which identify the param
  char                    name[CLAP_NAME_SIZE]; // the display name
  char                    desc[CLAP_DESC_SIZE];
  bool                    is_per_note;
  char                    display_text[CLAP_DISPLAY_SIZE]; // the text used to display the value
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
  uint32_t id;                     // preset id
  char     name[CLAP_NAME_SIZE];   // display name
  char     desc[CLAP_DESC_SIZE];   // desc and how to use it
  char     author[CLAP_NAME_SIZE];
  char     tags[CLAP_TAGS_SIZE];   // "tag1;tag2;tag3;..."
};

////////////
// EVENTS //
////////////

enum clap_event_type
{
  CLAP_EVENT_NOTE_ON         = 0, // note attribute
  CLAP_EVENT_NOTE_MODULATION = 1, // note attribute
  CLAP_EVENT_NOTE_OFF        = 2, // note attribute

  CLAP_EVENT_PARAM_SET  = 3,    // param attribute
  CLAP_EVENT_PARAM_RAMP = 4,    // param attribute
  CLAP_EVENT_PITCH_SET  = 5,    // pitch attribute
  CLAP_EVENT_PRESET_SET = 6,    // preset attribute

  CLAP_EVENT_MIDI    = 7,       // midi attribute
  CLAP_EVENT_CONTROL = 8,       // control attribute

  CLAP_EVENT_GUI_OPENED = 9,    // plugin to host, no attribute
  CLAP_EVENT_GUI_CLOSED = 10,   // plugin to host, no attribute

  CLAP_EVENT_NEW_PRESETS       = 11, // plugin to host, no attribute
  CLAP_EVENT_NEW_PORTS_CONFIGS = 12, // plugin to host, no attribute
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
  float                   increment;        // for param ramp
  char                    display_text[CLAP_DISPLAY_SIZE]; // use this for display if not NULL.
  bool                    is_recordable;    // used to tell the host if this event
                                            // can be recorded
};

struct clap_event_control
{
  uint32_t index;
  float    value; // 0 .. 1.0f
};

struct clap_event_pitch
{
  float freq_hz; // usually 440Hz
};

struct clap_event_preset
{
  uint32_t id;
};

struct clap_event_midi
{
  uint32_t  size;
  uint8_t  *buffer;
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  uint64_t              steady_time; // steady_time of the event, see host->steady_time(host)

  union {
    struct clap_event_note    note;
    struct clap_event_param   param;
    struct clap_event_pitch   pitch;
    struct clap_event_preset  preset;
    struct clap_event_midi    midi;
    struct clap_event_control control;
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
  uint32_t feedback_chunk_size;

  /* process info */
  bool     is_offline;
  uint32_t tempo;       // the tempo in samples
  uint64_t song_time;   // the song time in samples
  uint64_t steady_time; // the steady time in samples

  /* events */
  struct clap_event *events;

  /* output values */
  bool need_processing;
};

//////////
// HOST //
//////////

struct clap_host
{
  uint32_t clap_version; // initialized to CLAP_VERSION

  /* host info */
  char name[CLAP_NAME_SIZE];
  char manufacturer[CLAP_NAME_SIZE];
  char version[CLAP_NAME_SIZE];

  /* for events generated by the plugin. */
  void (*events)(struct clap_host   *host,
                 struct clap_plugin *plugin,
                 struct clap_event  *events);

  /* The time in samples, this clock is monotonicaly increasing,
   * it means that each time you call this function, the return
   * value must be greater or equal to the previous one. */
  uint64_t (*steady_time)(struct clap_host *host);

  /* future features */
  void *(*extension)(struct clap_host *host, const char *extention_id);
};

////////////
// PLUGIN //
////////////

// bitfield
enum clap_plugin_type
{
  CLAP_PLUGIN_INSTRUMENT  = (1 << 0),
  CLAP_PLUGIN_EFFECT      = (1 << 1),
  CLAP_PLUGIN_MIDI_EFFECT = (1 << 2),
  CLAP_PLUGIN_ANALYZER    = (1 << 3)
};

struct clap_plugin
{
  uint32_t clap_version; // initialized to CALP_VERSION

  void *host_data;   // reserved pointer for the host
  void *plugin_data; // reserved pointer for the plugin

  /* free plugin's resources */
  void (*destroy)(struct clap_plugin *plugin);

  /* plugin info */
  char id[CLAP_ID_SIZE];
  char name[CLAP_NAME_SIZE];
  char description[CLAP_DESC_SIZE];
  char manufacturer[CLAP_NAME_SIZE];
  char version[CLAP_NAME_SIZE];
  char url[CLAP_URL_SIZE];
  char license[CLAP_NAME_SIZE];
  char support[CLAP_URL_SIZE];  // a link to the support
  char caterogries[CLAP_TAGS_SIZE]; // fm;analogue;delay;...

  uint32_t plugin_type;
  uint32_t chunk_size;

  bool has_gui;
  bool supports_tunning;
  bool supports_microtones;

  /* audio ports */
  uint32_t (*get_ports_configs_count)(struct clap_plugin *plugin);
  bool (*get_ports_config)(struct clap_plugin       *plugin,
                           uint32_t                  config_index,
                           struct clap_ports_config *config);
  bool (*get_port_info)(struct clap_plugin    *plugin,
                        uint32_t               config_index,
                        uint32_t               port_index,
                        struct clap_port_info *port);
  bool (*set_ports_config)(struct clap_plugin *plugin,
                           uint32_t            config_index);
  bool (*set_port_repeat)(struct clap_plugin  *plugin,
                          uint32_t             port_index,
                          uint32_t             count);

  /* Returns a newly allocated parameters tree. The caller has to free it. */
  uint32_t (*get_params_count)(struct clap_plugin *plugin);
  bool (*get_param)(struct clap_plugin *plugin, uint32_t index, struct clap_param *param);

  /* Returns a newly allocated preset list. The caller has to free it. */
  uint32_t (*get_presets_count)(struct clap_plugin *plugin);
  bool (*get_presets)(struct clap_plugin *plugin, uint32_t index, struct clap_preset *preset);

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
typedef struct clap_plugin *(*clap_create_f)(uint32_t          plugin_index,
			                     struct clap_host *host,
                                             uint32_t          sample_rate,
                                             uint32_t         *plugins_count);

/* plugin entry point */
struct clap_plugin *
clap_create(uint32_t          plugin_index,
            struct clap_host *host,
            uint32_t          sample_rate,
            uint32_t         *plugins_count);

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_H */
