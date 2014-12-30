/*
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
  CLAP_ID_SIZE         = 48,
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
// Should be "1" if the plugin supports tunning.
# define CLAP_ATTR_SUPPORTS_TUNING "clap/supports_tuning"
// Shoudl be "1" if the plugin is doing remote processing.
// This is a hint for the host to optimize task scheduling.
# define CLAP_ATTR_IS_REMOTE_PROCESSING "clap/is_remote_processing"
// Should be "1" if the plugin supports in place processing.
# define CLAP_ATTR_SUPPORTS_IN_PLACE_PROCESSING "clap/supports_in_place_processing"

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
  CLAP_EVENT_NOTE_ON  = 0, // note attribute
  CLAP_EVENT_NOTE_OFF = 1, // note attribute

  CLAP_EVENT_PARAM_SET  = 2,    // param attribute
  CLAP_EVENT_PARAM_RAMP = 3,    // param attribute
  CLAP_EVENT_PRESET_SET = 4,    // preset attribute

  CLAP_EVENT_MIDI    = 5,       // midi attribute
  CLAP_EVENT_CONTROL = 6,       // control attribute

  CLAP_EVENT_GUI_OPENED = 7,   // plugin to host, no attribute
  CLAP_EVENT_GUI_CLOSED = 8,   // plugin to host, no attribute

  CLAP_EVENT_NEW_PRESETS       = 9, // plugin to host, no attribute
  CLAP_EVENT_NEW_PORTS_CONFIGS = 10, // plugin to host, no attribute

  CLAP_EVENT_LATENCY_CHANGED = 11, // plugin to host, latency attribute

  CLAP_EVENT_PLAY  = 12, // no attribute
  CLAP_EVENT_PAUSE = 13, // no attribute
  CLAP_EVENT_STOP  = 14, // no attribute

  CLAP_EVENT_JUMP = 15, // attribute jump
};

struct clap_event_note
{
  uint8_t key;
  float   pitch;
  float   velocity; // 0..1
};

struct clap_event_param
{
  /* key/voice index */
  bool                    is_global; // is this event global?
  uint8_t                 key;       // if !is_global, target key

  /* parameter */
  uint32_t                index; // parameter index
  union clap_param_value  value;
  float                   increment;        // for param ramp
  char                    display_text[CLAP_DISPLAY_SIZE]; // use this for display.
  bool                    is_recordable;    // used to tell the host if this event
                                            // can be recorded
  bool                    is_used; // is the parameter used in the patch?
};

struct clap_event_control
{
  /* voice/key index */
  bool     is_global; // is this event global?
  uint8_t  key;       // if !is_global, target key

  /* control */
  uint32_t index;
  float    value; // 0 .. 1.0f
};

struct clap_event_preset
{
  char url[CLAP_URL_SIZE]; // the url to the preset
};

struct clap_event_midi
{
  /* voice/key index */
  bool           is_global; // is this event global?
  uint8_t        key;       // if !is_global, target key

  /* midi event */
  const uint8_t *buffer;
  uint32_t       size;
};

struct clap_event_latency
{
  uint32_t latency;
};

struct clap_event_jump
{
  uint32_t tempo;      // tempo in samples
  uint32_t bar;        // the bar number
  uint32_t bar_offset; // 0 <= cycle_offset < tsig_denom * tempo
  uint32_t tsig_num;   // time signature numerator
  uint32_t tsig_denom; // time signature denominator
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  uint64_t              steady_time; // steady_time of the event, see host->steady_time(host)

  union {
    struct clap_event_note        note;
    struct clap_event_param       param;
    struct clap_event_preset      preset;
    struct clap_event_midi        midi;
    struct clap_event_control     control;
    struct clap_event_latency     latency;
    struct clap_event_jump	  jump;
  };
};

/////////////
// PROCESS //
/////////////

enum clap_process_status
{
  /* Processing failed. The output buffer must be discarded. */
  CLAP_PROCESS_ERROR    = 0,

  /* Processing succeed. */
  CLAP_PROCESS_CONTINUE = 1,

  /* Processing succeed, but no more processing is required, until next event. */
  CLAP_PROCESS_STOP     = 2,
};

struct clap_process
{
  /* audio buffers */
  float    **input;
  float    **output;
  uint32_t   samples_count;

  /* process info */
  uint64_t steady_time; // the steady time in samples

  /* events */
  struct clap_event *events;
};

//////////
// HOST //
//////////

struct clap_host
{
  uint32_t clap_version; // initialized to CLAP_VERSION

  /* returns the size of the original string, 0 if not string */
  uint32_t (*get_attribute)(struct clap_host *host,
                            const char       *attr,
                            char             *buffer,
                            uint32_t          size);

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
              struct clap_plugin     *plugin,
              enum clap_log_severity  severity,
              const char             *msg);

  /* feature extensions */
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

  /* Free the plugin and its resources.
   * It is not required to deactivate the plugin prior to this call. */
  void (*destroy)(struct clap_plugin *plugin);

  /* Copy at most size of the attribute's value into buffer.
   * This function must place a '\0' byte at the end of the string.
   * Returns the size of the original string or 0 if there is no
   * value for this attributes. */
  uint32_t (*get_attribute)(struct clap_plugin *plugin,
                            const char         *attr,
                            char               *buffer,
                            uint32_t            size);

  /* activation */
  bool (*activate)(struct clap_plugin *plugin);
  void (*deactivate)(struct clap_plugin *plugin);

  /* process */
  enum clap_process_status (*process)(struct clap_plugin  *plugin,
                                      struct clap_process *process);

  /* features extensions */
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
 * The return value has to be freed by calling plugin->destroy(plugin).
 *
 * This function must be thread-safe. */
struct clap_plugin *
clap_create(uint32_t          plugin_index,
            struct clap_host *host,
            uint32_t          sample_rate,
            uint32_t         *plugins_count);

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_H */
