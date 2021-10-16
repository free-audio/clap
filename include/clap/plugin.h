#pragma once

#include "macros.h"
#include "host.h"
#include "process.h"

#ifdef __cplusplus
extern "C" {
#endif

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

   /* Query an extension.
    * The returned pointer is owned by the plugin.
    * [thread-safe] */
   const void *(*get_extension)(const struct clap_plugin *plugin, const char *id);

   // Called by the host on the main thread in response to a previous call to:
   //   host->request_callback(host);
   // [main-thread]
   void (*on_main_thread)(const struct clap_plugin *plugin);
} clap_plugin;

/////////////////
// ENTRY POINT //
/////////////////

typedef struct clap_plugin_invalidation_source {
   // Directory containing the file(s) to scan
   const char *directory;

   // globing pattern, in the form *.dll
   const char *filename_glob;

   // should the directory be scanned recursively?
   bool recursive_scan;
} clap_plugin_invalidation_source;

// This interface is the entry point of the dynamic library.
//
// There is an invalidation mechanism for the set of plugins which is based on files.
// The host can watch the plugin DSO's mtime and a set of files's mtime provided by
// get_clap_invalidation_source().
//
// The set of plugins must not change, except during a call to refresh() by the host.
//
// Every methods must be thread-safe.
struct clap_plugin_entry {
   clap_version clap_version; // initialized to CLAP_VERSION

   bool (*init)(const char *plugin_path);
   void (*deinit)(void);

   /* Get the number of plugins available.
    * [thread-safe] */
   uint32_t (*get_plugin_count)(void);

   /* Retrieves a plugin descriptor by its index.
    * Returns null in case of error.
    * The descriptor does not need to be freed.
    * [thread-safe] */
   const clap_plugin_descriptor *(*get_plugin_descriptor)(uint32_t index);

   /* Create a clap_plugin by its plugin_id.
    * The returned pointer must be freed by calling plugin->destroy(plugin);
    * The plugin is not allowed to use the host callbacks in the create method.
    * Returns null in case of error.
    * [thread-safe] */
   const clap_plugin *(*create_plugin)(const clap_host *host, const char *plugin_id);

   // Get the number of invalidation source.
   uint32_t (*get_invalidation_source_count)(void);

   // Get the invalidation source by its index.
   // [thread-safe]
   const clap_plugin_invalidation_source *(*get_invalidation_source)(uint32_t index);

   // In case the host detected a invalidation event, it can call refresh() to let the
   // plugin_entry scan the set of plugins available.
   void (*refresh)(void);
};

/* Entry point */
CLAP_EXPORT extern const struct clap_plugin_entry clap_plugin_entry;

#ifdef __cplusplus
}
#endif
