#pragma once

#include "private/macros.h"
#include "host.h"
#include "process.h"

#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

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
   // Some examples:
   // "master;eq;spectrum"
   // "compressor;analog;character"
   // "reverb;plate;cathedral"
   // "kick;analog;808;roland"
   // "analog;character;roland;moog"
   // "chip;chiptune;gameboy;nintendo;sega"
   const char *keywords;

   alignas(8) uint64_t plugin_type; // bitfield of clap_plugin_type
} clap_plugin_descriptor_t;

typedef struct clap_plugin {
   const clap_plugin_descriptor_t *desc;

   void *plugin_data; // reserved pointer for the plugin

   // Must be called after creating the plugin.
   // If init returns false, the host must destroy the plugin instance.
   bool (*init)(const struct clap_plugin *plugin);

   /* Free the plugin and its resources.
    * It is not required to deactivate the plugin prior to this call. */
   void (*destroy)(const struct clap_plugin *plugin);

   // Activate and deactivate the plugin.
   // In this call the plugin may allocate memory and prepare everything needed for the process
   // call. The process's sample rate will be constant and process's frame count will included in
   // the [min, max] range, which is bounded by [1, INT32_MAX].
   // Once activated the latency and port configuration must remain constant, until deactivation.
   //
   // [main-thread]
   bool (*activate)(const struct clap_plugin *plugin,
                    double                    sample_rate,
                    uint32_t                  min_frames_count,
                    uint32_t                  max_frames_count);
   void (*deactivate)(const struct clap_plugin *plugin);

   // Set to true before processing, and to false before sending the plugin to sleep.
   // [audio-thread]
   bool (*start_processing)(const struct clap_plugin *plugin);
   void (*stop_processing)(const struct clap_plugin *plugin);

   /* process audio, events, ...
    * [audio-thread] */
   clap_process_status (*process)(const struct clap_plugin *plugin, const clap_process_t *process);

   /* Query an extension.
    * The returned pointer is owned by the plugin.
    * [thread-safe] */
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