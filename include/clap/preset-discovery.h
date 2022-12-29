/**
   Preset Discovery API.

   Preset Discovery enables a plug-in host to identify where presets are found, what
   extensions they have, which plug-ins they apply to, and other metadata associated with the
   presets so that they can be indexed and searched for quickly within the plug-in host's browser.

   This has a number of advantages for the user:
   - it allows them to browse for presets from one central location in a consistent way
   - the user can browse for presets without having to commit to a particular plug-in first

   The API works as follow to index presets and presets metadata:
   1. get the preset discovery factory
   2. create a preset provider
   3. list locations and file extensions from the preset provider
   4. monitor file system changes on the given locations
   5. read metadata for each preset files
   6. fetch information about collections from the preset provider

   Then to load a preset, use ext/draft/preset-load.h

   The design of this API deliberately does not define a fixed set tags or categories. It is the
   plug-in host's job to try to intelligently map the raw list of features that are found for a
   preset and to process this list to generate something that makes sense for the host's tagging and
   categorization system. The reason for this is to reduce the work for a plug-in developer to add
   Preset Discovery support for their existing preset file format and not have to be concerned with
   all the different hosts and how they want to receive the metadata.
*/

#pragma once

#include "private/std.h"
#include "private/macros.h"
#include "string-sizes.h"
#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Receiver that receives the metadata for a single preset file.
 * The host would define the various callbacks in this interface and the preset parser function
 * would then call them.
 *
 * This interface isn't thread-safe.
 */
typedef const struct clap_preset_metadata_receiver {
   /** If there is an error reading metadata from a file this should be called with an error
    * message. */
   void(CLAP_ABI *on_error)(const struct clap_preset_metadata_receiver *receiver,
                            const char                                 *error_message);

   /** Marks this file as a container file meaning that it can contain other presets. */
   void(CLAP_ABI *mark_as_container_file)(const struct clap_preset_metadata_receiver *receiver);

   /** If the file being parsed is a preset container file (mark_as_container_file has been called)
    * then this must be called for every preset in the file and before any preset metadata is
    * sent with the calls below. If the file is not a container file then this should not be
    * called at all.
    *
    * The path defines a human friendly path to the preset in the container file. It
    * should be unique within the container file.
    *
    * The preset_id is a machine friendly string used to load the preset inside the container via a
    * the preset-load plug-in extension. The preset_id can also just be the path if that's what the
    * extension wants but it could also be some other unique id like a database primary key or a
    * binary offset. It's use is entirely up to the plug-in.
    */
   void(CLAP_ABI *begin_contained_preset)(const struct clap_preset_metadata_receiver *receiver,
                                          const char                                 *path,
                                          const char                                 *preset_id);

   /** Marks this preset as a bank preset, meaning that it can be assigned to the plug-in as a
    * preset but will update the banks in the plug-in.
    */
   void(CLAP_ABI *mark_as_bank_preset)(const struct clap_preset_metadata_receiver *receiver);

   /** Sets plug-in id that this preset can be used with. */
   void(CLAP_ABI *set_plugin_id)(const struct clap_preset_metadata_receiver *receiver,
                                 const char                                 *plugin_id);

   /** Sets the collection to which the preset belongs to. */
   void(CLAP_ABI *set_collection_id)(const struct clap_preset_metadata_receiver,
                                     const char *collection_id);

   /** Adds a creator name for the preset. */
   void(CLAP_ABI *add_creator)(const struct clap_preset_metadata_receiver *receiver,
                               const char                                 *creator);

   /** Sets a description of the preset. */
   void(CLAP_ABI *set_description)(const struct clap_preset_metadata_receiver *receiver,
                                   const char                                 *description);

   /** Sets the creation time and last modification time of the preset.
    * If one of the time isn't known, then set it to 0. */
   void(CLAP_ABI *set_timestamps)(const struct clap_preset_metadata_receiver *receiver,
                                  uint64_t                                    creation_time,
                                  uint64_t                                    modification_time);

   /**
    * Adds a feature to the preset.
    * See plugin-features.h and preset-features.h
    *
    * The feature string is arbitrary, it is the indexer's job to remap it to its internal
    * categorization and tagging system.
    */
   void(CLAP_ABI *add_feature)(const struct clap_preset_metadata_receiver *receiver,
                               const char                                 *feature);

} *clap_preset_metadata_receiver_t;

enum clap_preset_discovery_flags {
   // This location/collection is meant for storing factory presets, most likely read-only
   CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT = 1 << 0,

   // This location/collection is meant for storing user created presets
   CLAP_PRESET_DISCOVERY_IS_USER_CONTENT = 1 << 1,

   // This location/collection is meant for demo presets, those are preset which may trigger
   // some limitation in the plugin because they require additionnal features which the user
   // needs to purchase or the content itself needs to be bought and is only available in
   // demo mode.
   CLAP_PRESET_DISCOVERY_IS_DEMO_CONTENT = 1 << 2,
};

// Defines a place in which to search for presets
typedef struct clap_preset_location {
   uint64_t flags; // see enum clap_preset_discovery_flags

   // name of this location
   char name[CLAP_NAME_SIZE];

   // path to a directory on the file system in which preset can be found
   char path[CLAP_URI_SIZE];
} clap_preset_location_t;

// A collection, represent a collection of presets; it is will most often used to identify presets
// which belongs to the same "sound pack".
typedef struct clap_preset_collection_info {
   uint64_t flags; // see enum clap_preset_discovery_flags

   char name[CLAP_NAME_SIZE];               // name of this collection
   char description[CLAP_DESCRIPTION_SIZE]; // reasonably short description of the collection
   char homepage_url[CLAP_URI_SIZE];        // url to the pack's homepage
   char image_uri[CLAP_URI_SIZE];           // may be an image on disk or from an http server
} clap_preset_collection_info_t;

// Describes a preset provider
typedef struct clap_preset_provider_descriptor {
   clap_version_t     clap_version; // initialized to CLAP_VERSION
   const char        *id;
   const char        *name;
   const char        *vendor;
   const char *const *plugin_ids; // null terminated array of plugin ids
} clap_preset_provider_descriptor_t;

// This interface isn't thread-safe.
typedef struct clap_preset_provider {
   const clap_preset_provider_descriptor_t *desc;

   // Destroys the preset provider
   void (CLAP_ABI *destroy)(const struct clap_preset_provider *provider);

   // returns the number of locations
   uint32_t(CLAP_ABI *locations_count)(const struct clap_preset_provider *provider);

   // gets the location info at index
   bool(CLAP_ABI *get_location)(const struct clap_preset_provider *provider,
                                uint32_t                           index,
                                clap_preset_location_t            *out_info);

   // returns the number of different supported file extensions
   uint32_t(CLAP_ABI *file_extensions_count)(const struct clap_preset_provider *provider);

   // stores the file extensions at index into out_extension.
   // the `.' isn't included in the string.
   // extension_capacity indicates the number of bytes available to store the extension
   bool(CLAP_ABI *get_file_extension)(const struct clap_preset_provider *provider,
                                      uint32_t                           index,
                                      char                              *out_extension,
                                      uint32_t                           extension_capacity);

   // gets information about a given collection.
   bool(CLAP_ABI *get_collection_info)(const struct clap_preset_provider *provider,
                                       const char                        *collection_id,
                                       clap_preset_collection_info_t     *out_info);

   // reads metadata from the given file and passes them to the metadata receiver
   bool(CLAP_ABI *read_file_metadata)(const struct clap_preset_provider   *provider,
                                      const char                          *file_path,
                                      const clap_preset_metadata_receiver *metadata_receiver);
} clap_preset_provider_t;

typedef struct clap_preset_indexer {
   clap_version_t clap_version; // initialized to CLAP_VERSION
   const char    *name;
   const char    *vendor;
   const char    *version;
} clap_preset_indexer_t;

static const CLAP_CONSTEXPR char CLAP_PRESET_DISCOVERY_FACTORY_ID[] =
   "clap.preset-discovery-factory";

// Every methods in this factory must be thread-safe.
// It is encourraged to perform preset indexing in background threads, maybe even in background
// process.
typedef struct clap_preset_discovery_factory {
   // Get the number of preset providers available.
   // [thread-safe]
   uint32_t(CLAP_ABI *count)(const struct clap_preset_discovery_factory *factory);

   // Retrieves a preset provider descriptor by its index.
   // Returns null in case of error.
   // The descriptor must not be freed.
   // [thread-safe]
   const clap_preset_provider_descriptor_t *(CLAP_ABI *get_descriptor)(
      const struct clap_preset_discovery_factory *factory, uint32_t index);

   // Create a preset provider by its id.
   // The returned pointer must be freed by calling preset_provider->destroy(preset_provider);
   // The plugin is not allowed to use the indexer callbacks in the create method.
   // Returns null in case of error.
   // [thread-safe]
   const clap_preset_provider_t *(CLAP_ABI *create)(
      const struct clap_preset_discovery_factory *factory,
      const clap_preset_indexer_t                *indexer,
      const char                                 *provider_id);
} clap_plugin_factory_t;

#ifdef __cplusplus
}
#endif
