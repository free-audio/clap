/*
 * CLAP - CLever Audio Plugin
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyright (c) 2014...2016 Alexandre BIQUE <bique.alexandre@gmail.com>
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
# define CLAP_VERSION CLAP_VERSION_MAKE(0, 2, 0)
# define CLAP_VERSION_MAJ(Version) (((Version) >> 16) & 0xff)
# define CLAP_VERSION_MIN(Version) (((Version) >> 8) & 0xff)
# define CLAP_VERSION_REV(Version) ((Version) & 0xff)

#if defined _WIN32 || defined __CYGWIN__
# ifdef __GNUC__
#  define CLAP_EXPORT __attribute__ ((dllexport))
# else
#  define CLAP_EXPORT __declspec(dllexport)
# endif
#else
# if __GNUC__ >= 4
#  define CLAP_EXPORT __attribute__ ((visibility ("default")))
# else
#  define CLAP_EXPORT
# endif
#endif

///////////////////////////
// FORWARD DELCLARATIONS //
///////////////////////////

struct clap_plugin;
struct clap_host;

enum clap_string_size
{
  CLAP_ID_SIZE         = 64,
  CLAP_NAME_SIZE       = 64,
  CLAP_DESC_SIZE       = 256,
  CLAP_DISPLAY_SIZE    = 64,
  CLAP_TAGS_SIZE       = 256,
};

enum clap_log_severity
{
  CLAP_LOG_DEBUG   = 0,
  CLAP_LOG_INFO    = 1,
  CLAP_LOG_WARNING = 2,
  CLAP_LOG_ERROR   = 3,
  CLAP_LOG_FATAL   = 4,
};

// Description of the plugin
# define CLAP_ATTR_DESCRIPTION     "clap/description"
// Product version string
# define CLAP_ATTR_VERSION         "clap/version"
// Manufacturer name
# define CLAP_ATTR_MANUFACTURER    "clap/manufacturer"
// Url to product
# define CLAP_ATTR_URL             "clap/url"
// Url to support page, or mail to support
# define CLAP_ATTR_SUPPORT         "clap/support"

// Should be "1" if the plugin supports tunning.
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
  CLAP_EVENT_NOTE_ON    = 0,    // note attribute
  CLAP_EVENT_NOTE_OFF   = 1,    // note attribute
  CLAP_EVENT_NOTE_CHOKE = 2,    // note attribute

  CLAP_EVENT_PARAM_SET  = 3,    // param attribute
  CLAP_EVENT_PARAM_RAMP = 4,    // param attribute

  CLAP_EVENT_CONTROL    = 5,    // control attribute
  CLAP_EVENT_MIDI       = 6,    // midi attribute

  CLAP_EVENT_PLAY  = 12, // no attribute
  CLAP_EVENT_PAUSE = 13, // no attribute
  CLAP_EVENT_STOP  = 14, // no attribute
  CLAP_EVENT_JUMP  = 15,  // attribute jump

  CLAP_EVENT_PROGRAM = 16, // program attribute
};

struct clap_event_param
{
  /* key/channel index */
  int8_t                  key;     // 0..127
  int8_t                  channel; // 0..16

  /* parameter */
  int32_t                 index;     // parameter index
  union clap_param_value  value;
  float                   increment; // for param ramp
};

/** Note On/Off event. */
struct clap_event_note
{
  int8_t  key;      // 0..127
  int8_t  channel;  // 0..15
  float   pitch;    // hertz
  float   velocity; // 0..1
};

struct clap_event_control
{
  int8_t key;     // 0..127
  int8_t channel; // 0..15
  int8_t control; // 0..127
  float  value;   // 0..1
};

struct clap_event_midi
{
  /* midi event */
  const uint8_t *buffer;
  int32_t        size;
};

struct clap_event_jump
{
  int32_t tempo;      // tempo in samples
  int32_t bar;        // the bar number
  int32_t bar_offset; // 0 <= bar_offset < tsig_denom * tempo
  int64_t song_time;  // song time in micro seconds
  int32_t tsig_num;   // time signature numerator
  int32_t tsig_denom; // time signature denominator
};

/**
 * Asks the plugin to load a program.
 * This is analogue to the midi program set:
 * bank msb goes into bank_msb
 * bank lsb goes into bank_lsb
 * program goes into program
 *
 * Clap is not limited to 127.
 *
 * The main advantage of setting a program instead of loading
 * a preset, is that the program should already be in the plugin's
 * memory, and can be set instantly. If the plugin has to load
 * a preset from the filesystem, then parse it, do memory allocation,
 * there is no guarentee that the preset will be loaded in time.
 */
struct clap_event_program
{
  int32_t bank_msb; // 0..0x7FFFFFFF
  int32_t bank_lsb; // 0..0x7FFFFFFF
  int32_t program;  // 0..0x7FFFFFFF
};

struct clap_event
{
  struct clap_event    *next; // linked list, NULL on end
  enum clap_event_type  type;
  int64_t               time; // offset from the first sample in the process block
  int32_t               port; // event port id

  union {
    struct clap_event_note        note;
    struct clap_event_control     control;
    struct clap_event_param       param;
    struct clap_event_midi        midi;
    struct clap_event_jump	  jump;
    struct clap_event_program     program;
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
  CLAP_PROCESS_SLEEP    = 2,
};

struct clap_process
{
  /* number of frame to process */
  int32_t frames_count;

  /* process info */
  int64_t steady_time; // the steady time in samples

  /* Linked list of events
   * The plugin must not modify those events. */
  struct clap_event *in_events;
  struct clap_event *out_events;
};

//////////
// HOST //
//////////

struct clap_host
{
  int32_t clap_version; // initialized to CLAP_VERSION

  void *host_data; // reserved pointer for the host

  char name[CLAP_NAME_SIZE]; // plugin name, eg: "BitwigStudio"
  char version[CLAP_NAME_SIZE]; // the plugin version, eg: "1.3.14"

  /* returns the size of the original string, 0 if not string
   * [thread-safe] */
  int32_t (*get_attribute)(struct clap_host *host,
                           const char       *attr,
                           char             *buffer,
                           int32_t           size);

  /* Log a message through the host.
   * [thread-safe] */
  void (*log)(struct clap_host       *host,
              struct clap_plugin     *plugin,
              enum clap_log_severity  severity,
              const char             *msg);

  /* query an extension
   * [thread-safe] */
  const void *(*extension)(struct clap_host *host, const char *extention_id);
};

////////////
// PLUGIN //
////////////

/* bitfield
 * This gives an hint to the host what the plugin might do. */
enum clap_plugin_type
{
  /* Instruments can play notes, and generate audio */
  CLAP_PLUGIN_INSTRUMENT   = (1 << 0),

  /* Audio effects, process audio input and produces audio.
   * Exemple: delay, reverb, compressor. */
  CLAP_PLUGIN_AUDIO_EFFECT = (1 << 1),

  /* Event effects, takes events as input and produces events.
   * Exemple: arpegiator */
  CLAP_PLUGIN_EVENT_EFFECT = (1 << 2), // can be seen as midi effect

  /* Analyze audio and/or events, and produces analysis results */
  CLAP_PLUGIN_ANALYZER     = (1 << 3),

  /* This plugin is a modular system, so it can load "modules",
   * have dynamic number of ports and parameters.
   * In short it could do anything. */
  CLAP_PLUGIN_PATCHER      = (1 << 4),

  /* This plugin streams the audio signal.
   * For example it can stream to a web radio.
   * This is important to not block the process loop. */
  CLAP_PLUGIN_STREAMER     = (1 << 5),

  /* This plugin act as a proxy, so it forwards the events/audio
   * to an other program on the same machine or on the network.
   *
   * For example a wine bridge which runs Windows plugins on Linux,
   * is a proxy. A plugin which sends the data to an hardware device
   * is a proxy as well. */
  CLAP_PLUGIN_PROXY        = (1 << 6),

  /* This plugin is a sampler, and can be used to load generic
   * audio files. */
  CLAP_PLUGIN_SAMPLER      = (1 << 7),
};

struct clap_plugin
{
  int32_t clap_version; // initialized to CLAP_VERSION

  void *host_data;   // reserved pointer for the host
  void *plugin_data; // reserved pointer for the plugin

  /* The 3 following strings are here because:
   * - they are mandatory
   * - it is convenient when you debug, to be able to see
   *   the plugin name, id and version by displaying
   *   the structure.
   */
  char name[CLAP_NAME_SIZE]; // plugin name, eg: "Diva"
  char id[CLAP_ID_SIZE]; // plugin id, eg: "u-he/diva"
  char version[CLAP_NAME_SIZE]; // the plugin version, eg: "1.3.2"

  enum clap_plugin_type plugin_type;

  /* Free the plugin and its resources.
   * It is not required to deactivate the plugin prior to this call. */
  void (*destroy)(struct clap_plugin *plugin);

  /* Copy at most size of the attribute's value into buffer.
   * This function must place a '\0' byte at the end of the string.
   * Returns the size of the original string or 0 if there is no
   * value for this attributes.
   * [thread-safe] */
  int32_t (*get_attribute)(struct clap_plugin *plugin,
                           const char         *attr,
                           char               *buffer,
                           int32_t             size);

  /* activation/deactivation
   * [audio-thread] */
  bool (*activate)(struct clap_plugin *plugin);
  void (*deactivate)(struct clap_plugin *plugin);

  /* process audio, events, ...
   * [audio-thread] */
  enum clap_process_status (*process)(struct clap_plugin  *plugin,
                                      struct clap_process *process);

  /* query an extension
   * [thread-safe] */
  const void *(*extension)(struct clap_plugin *plugin, const char *id);
};

/* This interface is the entry point of the dynamic library.
 *
 * Every methods must be thread-safe.
 *
 * Common sample rate values are: 44100, 48000, 88200, 96000,
 * 176400, 192000. */
struct clap_plugin_factory
{
  /* Get the number of plugins available.
   * [thread-safe] */
  int32_t (*get_plugin_count)(struct clap_plugin_factory *factory);

  /* Create a clap_plugin by its index.
   * Valid indexes are from 0 to get_plugin_count() - 1.
   * Returns null in case of error.
   * [thread-safe] */
  struct clap_plugin *(*create_plugin_by_index)(struct clap_plugin_factory *factory,
                                                struct clap_host           *host,
                                                int32_t                     sample_rate,
                                                int32_t                     index);

  /* Create a clap_plugin by its plugin_id.
   * Returns null in case of error.
   * [thread-safe] */
  struct clap_plugin *(*create_plugin_by_id)(struct clap_plugin_factory *factory,
                                            struct clap_host           *host,
                                            int32_t                     sample_rate,
                                            const char                 *plugin_id);
};

/* Entry point */
CLAP_EXPORT extern const struct clap_plugin_factory *clap_plugin_factory;

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_H */
