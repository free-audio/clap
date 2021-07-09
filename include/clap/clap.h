/*
 * CLAP - CLever Audio Plugin
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyright (c) 2014...2021 Alexandre BIQUE <bique.alexandre@gmail.com>
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

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "events.h"
#include "macros.h"
#include "version.h"
#include "string-sizes.h"

#ifdef __cplusplus
extern "C" {
#endif

/////////////
// PROCESS //
/////////////

enum {
   // Processing failed. The output buffer must be discarded.
   CLAP_PROCESS_ERROR = 0,

   // Processing succeed, keep processing.
   CLAP_PROCESS_CONTINUE = 1,

   // Processing succeed, keep processing if the output is not quiet.
   CLAP_PROCESS_CONTINUE_IF_NOT_QUIET = 2,

   // Processing succeed, but no more processing is required,
   // until next event or variation in audio input.
   CLAP_PROCESS_SLEEP = 3,
};
typedef int32_t clap_process_status;

typedef struct clap_audio_buffer {
   // Either data32 or data64 pointer will be set.
   float ** data32;
   double **data64;
   uint32_t channel_count;
   uint32_t latency;       // latency from/to the audio interface
   uint64_t constant_mask; // mask & (1 << N) to test if channel N is constant
} clap_audio_buffer;

typedef struct clap_process {
   uint64_t steady_time;  // a steady sample time counter, requiered
   uint32_t frames_count; // number of frame to process

   // time info at sample 0
   // If null, then this is a free running host, no transport events will be provided
   const clap_event_transport *transport;

   // Audio buffers, they must have the same count as specified
   // by clap_plugin_audio_ports->get_count().
   // The index maps to clap_plugin_audio_ports->get_info().
   //
   // If a plugin does not implement clap_plugin_audio_ports,
   // then it gets a default stereo input and output.
   const clap_audio_buffer *audio_inputs;
   const clap_audio_buffer *audio_outputs;
   uint32_t                 audio_inputs_count;
   uint32_t                 audio_outputs_count;

   /* events */
   const clap_event_list *in_events;
   const clap_event_list *out_events;
} clap_process;

//////////
// HOST //
//////////

typedef struct clap_host {
   clap_version clap_version; // initialized to CLAP_VERSION

   void *host_data; // reserved pointer for the host

   // name and version are mandatory.
   const char *name;    // eg: "Bitwig Studio"
   const char *vendor;  // eg: "Bitwig GmbH"
   const char *url;     // eg: "https://bitwig.com"
   const char *version; // eg: "3.3.8"

   // Query an extension.
   // [thread-safe]
   const void *(*extension)(const struct clap_host *host, const char *extension_id);

   // Ask the host to deactivate and then reactivate the plugin.
   // The operation may be delayed by the host.
   // [thread-safe]
   void (*restart_plugin)(const struct clap_host *host);
} clap_host;

////////////
// PLUGIN //
////////////

/* bitfield
 * This gives an hint to the host what the plugin might do. */
enum {
   /* Instruments can play notes, and generate audio */
   CLAP_PLUGIN_INSTRUMENT = (1 << 0),

   /* Audio effects, process audio input and produces audio.
    * Exemple: delay, reverb, compressor. */
   CLAP_PLUGIN_AUDIO_EFFECT = (1 << 1),

   /* Event effects, takes events as input and produces events.
    * Exemple: arpegiator */
   CLAP_PLUGIN_EVENT_EFFECT = (1 << 2), // can be seen as midi effect

   // Analyze audio and/or events.
   // If this is the only type reported by the plugin, the host can assume that it wont change the
   // audio and event signal.
   CLAP_PLUGIN_ANALYZER = (1 << 3),
};
typedef int32_t clap_plugin_type;

typedef struct clap_plugin_descriptor {
   clap_version clap_version; // initialized to CLAP_VERSION

   const char *id;          // eg: "com.u-he.diva"
   const char *name;        // eg: "Diva"
   const char *vendor;      // eg: "u-he"
   const char *url;         // eg: "https://u-he.com/products/diva/"
   const char *manual_url;  // eg: "https://dl.u-he.com/manuals/plugins/diva/Diva-user-guide.pdf"
   const char *support_url; // eg: "https://u-he.com/support/"
   const char *version;     // eg: "1.4.4"
   const char *description; // eg: "The spirit of analogue"

   // Arbitrary list of keywords, separated by `;'
   // They can be matched by the host search engine and used to classify the plugin.
   // Some examples:
   // "master;eq;spectrum"
   // "compressor;analog;character"
   // "reverb;plate;cathedral"
   // "kick;analog;808;roland"
   // "analog;character;roland;moog"
   // "chip;chiptune;gameboy;nintendo;sega"
   const char *keywords;

   uint64_t plugin_type; // bitfield of clap_plugin_type
} clap_plugin_descriptor;

typedef struct clap_plugin {
   const clap_plugin_descriptor *desc;

   void *plugin_data; // reserved pointer for the plugin

   // Must be called after creating the plugin.
   // If init returns false, the host must destroy the plugin instance.
   bool (*init)(const struct clap_plugin *plugin);

   /* Free the plugin and its resources.
    * It is not required to deactivate the plugin prior to this call. */
   void (*destroy)(const struct clap_plugin *plugin);

   /* activation/deactivation
    * [main-thread] */
   bool (*activate)(const struct clap_plugin *plugin, double sample_rate);
   void (*deactivate)(const struct clap_plugin *plugin);

   // Set to true before processing, and to false before sending the plugin to sleep.
   // [audio-thread]
   bool (*start_processing)(const struct clap_plugin *plugin);
   void (*stop_processing)(const struct clap_plugin *plugin);

   /* process audio, events, ...
    * [audio-thread] */
   clap_process_status (*process)(const struct clap_plugin *plugin, const clap_process *process);

   /* query an extension
    * [thread-safe] */
   const void *(*extension)(const struct clap_plugin *plugin, const char *id);
} clap_plugin;

/////////////////
// ENTRY POINT //
/////////////////

/* This interface is the entry point of the dynamic library.
 *
 * Every methods must be thread-safe. */
struct clap_plugin_entry {
   bool (*init)(const char *plugin_path);
   void (*deinit)(void);

   /* Get the number of plugins available.
    * [thread-safe] */
   int32_t (*get_plugin_count)(void);

   /* Retrieves a plugin descriptor by its index.
    * Returns null in case of error.
    * The descriptor does not need to be freed.
    * [thread-safe] */
   const clap_plugin_descriptor *(*get_plugin_descriptor)(int32_t index);

   /* Create a clap_plugin by its plugin_id.
    * The returned pointer must be freed by calling plugin->destroy(plugin);
    * The plugin is not allowed to use the host callbacks in the create method.
    * Returns null in case of error.
    * [thread-safe] */
   const clap_plugin *(*create_plugin)(const clap_host *host, const char *plugin_id);
};

/* Entry point */
CLAP_EXPORT extern const struct clap_plugin_entry clap_plugin_entry;

#ifdef __cplusplus
}
#endif
