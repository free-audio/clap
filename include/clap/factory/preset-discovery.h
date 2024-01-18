/*
   Preset Discovery API.

   Preset Discovery enables a plug-in host to identify where presets are found, what
   extensions they have, which plug-ins they apply to, and other metadata associated with the
   presets so that they can be indexed and searched for quickly within the plug-in host's browser.

   This has a number of advantages for the user:
   - it allows them to browse for presets from one central location in a consistent way
   - the user can browse for presets without having to commit to a particular plug-in first

   The API works as follow to index presets and presets metadata:
   1. clap_plugin_entry.get_factory(CLAP_PRESET_DISCOVERY_FACTORY_ID)
   2. clap_preset_discovery_factory_t.create(...)
   3. clap_preset_discovery_provider.init() (only necessary the first time, declarations
   can be cached)
        `-> clap_preset_discovery_indexer.declare_filetype()
        `-> clap_preset_discovery_indexer.declare_location()
        `-> clap_preset_discovery_indexer.declare_soundpack() (optional)
        `-> clap_preset_discovery_indexer.set_invalidation_watch_file() (optional)
   4. crawl the given locations and monitor file system changes
        `-> clap_preset_discovery_indexer.get_metadata() for each presets files

   Then to load a preset, use ext/draft/preset-load.h.
   TODO: create a dedicated repo for other plugin abi preset-load extension.

   The design of this API deliberately does not define a fixed set tags or categories. It is the
   plug-in host's job to try to intelligently map the raw list of features that are found for a
   preset and to process this list to generate something that makes sense for the host's tagging and
   categorization system. The reason for this is to reduce the work for a plug-in developer to add
   Preset Discovery support for their existing preset file format and not have to be concerned with
   all the different hosts and how they want to receive the metadata.

   VERY IMPORTANT:
   - the whole indexing process has to be **fast**
      - clap_preset_provider->get_metadata() has to be fast and avoid unnecessary operations
   - the whole indexing process must not be interactive
      - don't show dialogs, windows, ...
      - don't ask for user input
*/

#pragma once

#include "../private/std.h"
#include "../private/macros.h"
#include "../timestamp.h"
#include "../version.h"
#include "../universal-plugin-id.h"

// Use it to retrieve const clap_preset_discovery_factory_t* from
// clap_plugin_entry.get_factory()
static const CLAP_CONSTEXPR char CLAP_PRESET_DISCOVERY_FACTORY_ID[] =
   "clap.preset-discovery-factory/2";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static const CLAP_CONSTEXPR char CLAP_PRESET_DISCOVERY_FACTORY_ID_COMPAT[] =
   "clap.preset-discovery-factory/draft-2";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_preset_discovery_location_kind {
   // The preset are located in a file on the OS filesystem.
   // The location is then a path which works with the OS file system functions (open, stat, ...)
   // So both '/' and '\' shall work on Windows as a separator.
   CLAP_PRESET_DISCOVERY_LOCATION_FILE = 0,

   // The preset is bundled within the plugin DSO itself.
   // The location must then be null, as the preset are within the plugin itself and then the plugin
   // will act as a preset container.
   CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN = 1,
};

enum clap_preset_discovery_flags {
   // This is for factory or sound-pack presets.
   CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT = 1 << 0,

   // This is for user presets.
   CLAP_PRESET_DISCOVERY_IS_USER_CONTENT = 1 << 1,

   // This location is meant for demo presets, those are preset which may trigger
   // some limitation in the plugin because they require additional features which the user
   // needs to purchase or the content itself needs to be bought and is only available in
   // demo mode.
   CLAP_PRESET_DISCOVERY_IS_DEMO_CONTENT = 1 << 2,

   // This preset is a user's favorite
   CLAP_PRESET_DISCOVERY_IS_FAVORITE = 1 << 3,
};

// Receiver that receives the metadata for a single preset file.
// The host would define the various callbacks in this interface and the preset parser function
// would then call them.
//
// This interface isn't thread-safe.
typedef struct clap_preset_discovery_metadata_receiver {
   void *receiver_data; // reserved pointer for the metadata receiver

   // If there is an error reading metadata from a file this should be called with an error
   // message.
   // os_error: the operating system error, if applicable. If not applicable set it to a non-error
   // value, eg: 0 on unix and Windows.
   void(CLAP_ABI *on_error)(const struct clap_preset_discovery_metadata_receiver *receiver,
                            int32_t                                               os_error,
                            const char                                           *error_message);

   // This must be called for every preset in the file and before any preset metadata is
   // sent with the calls below.
   //
   // If the preset file is a preset container then name and load_key are mandatory, otherwise
   // they are optional.
   //
   // The load_key is a machine friendly string used to load the preset inside the container via a
   // the preset-load plug-in extension. The load_key can also just be the subpath if that's what
   // the plugin wants but it could also be some other unique id like a database primary key or a
   // binary offset. It's use is entirely up to the plug-in.
   //
   // If the function returns false, then the provider must stop calling back into the receiver.
   bool(CLAP_ABI *begin_preset)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                const char                                           *name,
                                const char                                           *load_key);

   // Adds a plug-in id that this preset can be used with.
   void(CLAP_ABI *add_plugin_id)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                 const clap_universal_plugin_id_t                     *plugin_id);

   // Sets the sound pack to which the preset belongs to.
   void(CLAP_ABI *set_soundpack_id)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                    const char *soundpack_id);

   // Sets the flags, see clap_preset_discovery_flags.
   // If unset, they are then inherited from the location.
   void(CLAP_ABI *set_flags)(const struct clap_preset_discovery_metadata_receiver *receiver,
                             uint32_t                                              flags);

   // Adds a creator name for the preset.
   void(CLAP_ABI *add_creator)(const struct clap_preset_discovery_metadata_receiver *receiver,
                               const char                                           *creator);

   // Sets a description of the preset.
   void(CLAP_ABI *set_description)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                   const char *description);

   // Sets the creation time and last modification time of the preset.
   // If one of the times isn't known, set it to CLAP_TIMESTAMP_UNKNOWN.
   // If this function is not called, then the indexer may look at the file's creation and
   // modification time.
   void(CLAP_ABI *set_timestamps)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  clap_timestamp creation_time,
                                  clap_timestamp modification_time);

   // Adds a feature to the preset.
   //
   // The feature string is arbitrary, it is the indexer's job to understand it and remap it to its
   // internal categorization and tagging system.
   //
   // However, the strings from plugin-features.h should be understood by the indexer and one of the
   // plugin category could be provided to determine if the preset will result into an audio-effect,
   // instrument, ...
   //
   // Examples:
   // kick, drum, tom, snare, clap, cymbal, bass, lead, metalic, hardsync, crossmod, acid,
   // distorted, drone, pad, dirty, etc...
   void(CLAP_ABI *add_feature)(const struct clap_preset_discovery_metadata_receiver *receiver,
                               const char                                           *feature);

   // Adds extra information to the metadata.
   void(CLAP_ABI *add_extra_info)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  const char                                           *key,
                                  const char                                           *value);
} clap_preset_discovery_metadata_receiver_t;

typedef struct clap_preset_discovery_filetype {
   const char *name;
   const char *description; // optional

   // `.' isn't included in the string.
   // If empty or NULL then every file should be matched.
   const char *file_extension;
} clap_preset_discovery_filetype_t;

// Defines a place in which to search for presets
typedef struct clap_preset_discovery_location {
   uint32_t    flags; // see enum clap_preset_discovery_flags
   const char *name;  // name of this location
   uint32_t    kind;  // See clap_preset_discovery_location_kind

   // Actual location in which to crawl presets.
   // For FILE kind, the location can be either a path to a directory or a file.
   // For PLUGIN kind, the location must be null.
   const char *location;
} clap_preset_discovery_location_t;

// Describes an installed sound pack.
typedef struct clap_preset_discovery_soundpack {
   uint32_t       flags;             // see enum clap_preset_discovery_flags
   const char    *id;                // sound pack identifier
   const char    *name;              // name of this sound pack
   const char    *description;       // optional, reasonably short description of the sound pack
   const char    *homepage_url;      // optional, url to the pack's homepage
   const char    *vendor;            // optional, sound pack's vendor
   const char    *image_path;        // optional, an image on disk
   clap_timestamp release_timestamp; // release date, CLAP_TIMESTAMP_UNKNOWN if unavailable
} clap_preset_discovery_soundpack_t;

// Describes a preset provider
typedef struct clap_preset_discovery_provider_descriptor {
   clap_version_t clap_version; // initialized to CLAP_VERSION
   const char    *id;           // see plugin.h for advice on how to choose a good identifier
   const char    *name;         // eg: "Diva's preset provider"
   const char    *vendor;       // optional, eg: u-he
} clap_preset_discovery_provider_descriptor_t;

// This interface isn't thread-safe.
typedef struct clap_preset_discovery_provider {
   const clap_preset_discovery_provider_descriptor_t *desc;

   void *provider_data; // reserved pointer for the provider

   // Initialize the preset provider.
   // It should declare all its locations, filetypes and sound packs.
   // Returns false if initialization failed.
   bool(CLAP_ABI *init)(const struct clap_preset_discovery_provider *provider);

   // Destroys the preset provider
   void(CLAP_ABI *destroy)(const struct clap_preset_discovery_provider *provider);

   // reads metadata from the given file and passes them to the metadata receiver
   // Returns true on success.
   bool(CLAP_ABI *get_metadata)(const struct clap_preset_discovery_provider     *provider,
                                uint32_t                                         location_kind,
                                const char                                      *location,
                                const clap_preset_discovery_metadata_receiver_t *metadata_receiver);

   // Query an extension.
   // The returned pointer is owned by the provider.
   // It is forbidden to call it before provider->init().
   // You can call it within provider->init() call, and after.
   const void *(CLAP_ABI *get_extension)(const struct clap_preset_discovery_provider *provider,
                                         const char                                  *extension_id);
} clap_preset_discovery_provider_t;

// This interface isn't thread-safe
typedef struct clap_preset_discovery_indexer {
   clap_version_t clap_version; // initialized to CLAP_VERSION
   const char    *name;         // eg: "Bitwig Studio"
   const char    *vendor;       // optional, eg: "Bitwig GmbH"
   const char    *url;          // optional, eg: "https://bitwig.com"
   const char *version; // optional, eg: "4.3", see plugin.h for advice on how to format the version

   void *indexer_data; // reserved pointer for the indexer

   // Declares a preset filetype.
   // Don't callback into the provider during this call.
   // Returns false if the filetype is invalid.
   bool(CLAP_ABI *declare_filetype)(const struct clap_preset_discovery_indexer *indexer,
                                    const clap_preset_discovery_filetype_t     *filetype);

   // Declares a preset location.
   // Don't callback into the provider during this call.
   // Returns false if the location is invalid.
   bool(CLAP_ABI *declare_location)(const struct clap_preset_discovery_indexer *indexer,
                                    const clap_preset_discovery_location_t     *location);

   // Declares a sound pack.
   // Don't callback into the provider during this call.
   // Returns false if the sound pack is invalid.
   bool(CLAP_ABI *declare_soundpack)(const struct clap_preset_discovery_indexer *indexer,
                                     const clap_preset_discovery_soundpack_t    *soundpack);

   // Query an extension.
   // The returned pointer is owned by the indexer.
   // It is forbidden to call it before provider->init().
   // You can call it within provider->init() call, and after.
   const void *(CLAP_ABI *get_extension)(const struct clap_preset_discovery_indexer *indexer,
                                         const char                                 *extension_id);
} clap_preset_discovery_indexer_t;

// Every methods in this factory must be thread-safe.
// It is encouraged to perform preset indexing in background threads, maybe even in background
// process.
//
// The host may use clap_plugin_invalidation_factory to detect filesystem changes
// which may change the factory's content.
typedef struct clap_preset_discovery_factory {
   // Get the number of preset providers available.
   // [thread-safe]
   uint32_t(CLAP_ABI *count)(const struct clap_preset_discovery_factory *factory);

   // Retrieves a preset provider descriptor by its index.
   // Returns null in case of error.
   // The descriptor must not be freed.
   // [thread-safe]
   const clap_preset_discovery_provider_descriptor_t *(CLAP_ABI *get_descriptor)(
      const struct clap_preset_discovery_factory *factory, uint32_t index);

   // Create a preset provider by its id.
   // The returned pointer must be freed by calling preset_provider->destroy(preset_provider);
   // The preset provider is not allowed to use the indexer callbacks in the create method.
   // It is forbidden to call back into the indexer before the indexer calls provider->init().
   // Returns null in case of error.
   // [thread-safe]
   const clap_preset_discovery_provider_t *(CLAP_ABI *create)(
      const struct clap_preset_discovery_factory *factory,
      const clap_preset_discovery_indexer_t      *indexer,
      const char                                 *provider_id);
} clap_preset_discovery_factory_t;

#ifdef __cplusplus
}
#endif
