#pragma once

#include "private/macros.h"
#include "host.h"
#include "process.h"

#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_plugin_descriptor {
   clap_version_t clap_version; // initialized to CLAP_VERSION

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
   //
   // Some pre-defined keywords:
   // - "instrument", "audio_effect", "note_effect", "analyzer"
   // - "mono", "stereo", "surround", "ambisonic"
   // - "distortion", "compressor", "limiter", "transient"
   // - "equalizer", "filter", "de-esser"
   // - "delay", "reverb", "chorus", "flanger"
   // - "tool", "utility", "glitch"
   //
   // - "win32-dpi-aware" informs the host that this plugin is dpi-aware on Windows
   //
   // Some examples:
   // "equalizer;analyzer;stereo;mono"
   // "compressor;analog;character;mono"
   // "reverb;plate;stereo"
   // "reverb;spring;surround"
   // "kick;analog;808;roland;drum;mono;instrument"
   // "instrument;chiptune;gameboy;nintendo;sega;mono"
   const char *features;
} clap_plugin_descriptor_t;

typedef struct clap_plugin {
   const clap_plugin_descriptor_t *desc;

   void *plugin_data; // reserved pointer for the plugin

   // Must be called after creating the plugin.
   // If init returns false, the host must destroy the plugin instance.
   // [main-thread]
   bool (*init)(const struct clap_plugin *plugin);

   // Free the plugin and its resources.
   // It is not required to deactivate the plugin prior to this call.
   // [main-thread & !active]
   void (*destroy)(const struct clap_plugin *plugin);

   // Activate and deactivate the plugin.
   // In this call the plugin may allocate memory and prepare everything needed for the process
   // call. The process's sample rate will be constant and process's frame count will included in
   // the [min, max] range, which is bounded by [1, INT32_MAX].
   // Once activated the latency and port configuration must remain constant, until deactivation.
   //
   // [main-thread & !active_state]
   bool (*activate)(const struct clap_plugin *plugin,
                    double                    sample_rate,
                    uint32_t                  min_frames_count,
                    uint32_t                  max_frames_count);
   // [main-thread & active_state]
   void (*deactivate)(const struct clap_plugin *plugin);

   // Call start processing before processing.
   // [audio-thread & active_state & !processing_state]
   bool (*start_processing)(const struct clap_plugin *plugin);

   // Call stop processing before sending the plugin to sleep.
   // [audio-thread & active_state & processing_state]
   void (*stop_processing)(const struct clap_plugin *plugin);

   // process audio, events, ...
   // [audio-thread & active_state & processing_state]
   clap_process_status (*process)(const struct clap_plugin *plugin, const clap_process_t *process);

   // Query an extension.
   // The returned pointer is owned by the plugin.
   // [thread-safe]
   const void *(*get_extension)(const struct clap_plugin *plugin, const char *id);

   // Called by the host on the main thread in response to a previous call to:
   //   host->request_callback(host);
   // [main-thread]
   void (*on_main_thread)(const struct clap_plugin *plugin);
} clap_plugin_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif