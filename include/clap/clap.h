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

# include <stddef.h>
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
  CLAP_TAGS_SIZE       = 128,
  CLAP_URL_SIZE        = 256,
};

enum clap_log_severity
{
  CLAP_LOG_DEBUG   = 0,
  CLAP_LOG_INFO    = 1,
  CLAP_LOG_WARNING = 2,
  CLAP_LOG_ERROR   = 3,
  CLAP_LOG_FATAL   = 4,
};

# define CLAP_ATTR_ID              "clap/id"
# define CLAP_ATTR_NAME            "clap/name"
# define CLAP_ATTR_DESCRIPTION     "clap/description"
# define CLAP_ATTR_VERSION         "clap/version"
# define CLAP_ATTR_MANUFACTURER    "clap/manufacturer"
# define CLAP_ATTR_URL             "clap/url"
# define CLAP_ATTR_SUPPORT         "clap/support"
# define CLAP_ATTR_LICENSE         "clap/license"
# define CLAP_ATTR_CATEGORIES      "clap/categories"
# define CLAP_ATTR_TYPE            "clap/type"
# define CLAP_ATTR_CHUNK_SIZE      "clap/chunk_size"
# define CLAP_ATTR_LATENCY         "clap/latency"
# define CLAP_ATTR_HAS_GUI         "clap/has_gui"
# define CLAP_ATTR_SUPPORTS_TUNING "clap/supports_tuning"

////////////////
// PARAMETERS //
////////////////

union clap_param_value
{
  bool    b;
  float   f;
  int32_t i;
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
  CLAP_EVENT_PRESET_SET = 6,    // preset attribute

  CLAP_EVENT_MIDI    = 7,       // midi attribute
  CLAP_EVENT_CONTROL = 8,       // control attribute

  CLAP_EVENT_GUI_OPENED = 9,    // plugin to host, no attribute
  CLAP_EVENT_GUI_CLOSED = 10,   // plugin to host, no attribute

  CLAP_EVENT_NEW_PRESETS       = 11, // plugin to host, no attribute
  CLAP_EVENT_NEW_PORTS_CONFIGS = 12, // plugin to host, no attribute

  CLAP_EVENT_LATENCY_CHANGED = 13, // plugin to host, latency attribute

  CLAP_EVENT_PLAY  = 14, // no attribute
  CLAP_EVENT_PAUSE = 15, // no attribute
  CLAP_EVENT_STOP  = 16, // no attribute
};

struct clap_event_note
{
  uint8_t key;
  float   pitch;

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

struct clap_event_preset
{
  uint32_t id; // the preset id
};

struct clap_event_midi
{
  uint32_t  size;
  uint8_t  *buffer;
};

struct clap_event_latency
{
  uint32_t latency;
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  uint64_t              steady_time; // steady_time of the event, see host->steady_time(host)

  union {
    struct clap_event_note    note;
    struct clap_event_param   param;
    struct clap_event_preset  preset;
    struct clap_event_midi    midi;
    struct clap_event_control control;
    struct clap_event_latency latency;
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
  uint32_t   samples_count;

  /* feedback loops */
  void (*feedback)(struct clap_process *process,
                   uint32_t             stream_id,
                   uint32_t             nb_samples);
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

  /* returns the size of the original string, 0 if not string */
  uint32_t (*get_attribute)(struct clap_plugin *plugin,
                            const char         *attr,
                            char               *buffer,
                            uint32_t            size);

  /* for events generated by the plugin. */
  void (*events)(struct clap_host   *host,
                 struct clap_plugin *plugin,
                 struct clap_event  *events);

  /* The time in samples, this clock is monotonicaly increasing,
   * it means that each time you call this function, the return
   * value must be greater or equal to the previous one. */
  uint64_t (*steady_time)(struct clap_host *host);

  /* Log a message through the host. */
  void (*log)(struct clap_host       *host,
              enum clap_log_severity  severity,
              const char             *msg);

  /* future features */
  void *(*extension)(struct clap_host *host, const char *extention_id);
};

////////////
// PLUGIN //
////////////

// bitfield
enum clap_plugin_type
{
  CLAP_PLUGIN_INSTRUMENT   = (1 << 0),
  CLAP_PLUGIN_EFFECT       = (1 << 1),
  CLAP_PLUGIN_EVENT_EFFECT = (1 << 2), // can be seen as midi effect
  CLAP_PLUGIN_ANALYZER     = (1 << 3),
};

struct clap_plugin
{
  uint32_t clap_version; // initialized to CALP_VERSION

  void *host_data;   // reserved pointer for the host
  void *plugin_data; // reserved pointer for the plugin

  /* free plugin's resources */
  void (*destroy)(struct clap_plugin *plugin);

  /* returns the size of the original string, 0 if not string */
  uint32_t (*get_attribute)(struct clap_plugin *plugin,
                            const char         *attr,
                            char               *buffer,
                            uint32_t            size);

  /* activation */
  bool (*activate)(struct clap_plugin *plugin);
  void (*deactivate)(struct clap_plugin *plugin);

  /* work */
  void (*process)(struct clap_plugin *plugin, struct clap_process *process);

  /* gui */
  bool (*open_gui)(struct clap_plugin *plugin);
  void (*close_gui)(struct clap_plugin *plugin);

  /* The plugin has to allocate and save its state into *buffer.
   * The plugin is also responsible to free the buffer on the
   * next call to save() or when the plugin is destroyed. */
  bool (*save)(struct clap_plugin *plugin, void **buffer, uint32_t *size);
  bool (*restore)(struct clap_plugin *plugin, const void *buffer, uint32_t size);

  /* future features */
  void *(*extension)(struct clap_plugin *plugin, const char *id);
};

/* typedef for dlsym() cast */
typedef struct clap_plugin *(*clap_create_f)(uint32_t          plugin_index,
			                     struct clap_host *host,
                                             uint32_t          sample_rate,
                                             uint32_t         *plugins_count);

/* Plugin entry point. If plugins_count is not null, then clap_create has
 * to store the number of plugins available in *plugins_count.
 * If clap_create failed to create a plugin, it returns NULL.
 * The return value has to be freed by calling plugin->destroy(plugin). */
struct clap_plugin *
clap_create(uint32_t          plugin_index,
            struct clap_host *host,
            uint32_t          sample_rate,
            uint32_t         *plugins_count);

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_H */
