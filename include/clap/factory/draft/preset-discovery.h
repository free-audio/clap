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
   3. clap_preset_discovery_provider.register_filetypes()
        `-> clap_preset_discovery_indexer.register_filetype()
      clap_preset_discovery_provider.register_locations()
        `-> clap_preset_discovery_indexer.register_location()
      clap_preset_discovery_provider.register_collections()
        `-> clap_preset_discovery_indexer.register_collection()
   4. crawl the given locations and monitor file system changes
   5. read metadata for each preset files

   Then to load a preset, use ext/draft/preset-load.h

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

#include "../../private/std.h"
#include "../../private/macros.h"
#include "../../string-sizes.h"
#include "../../version.h"
#include "../../color.h"

// Use it to retrieve const clap_preset_discovery_factory_t* from
// clap_plugin_entry.get_factory()
static const CLAP_CONSTEXPR char CLAP_PRESET_DISCOVERY_FACTORY_ID[] =
   "clap.preset-discovery-factory/draft-0";

#ifdef __cplusplus
extern "C" {
#endif

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

   // This preset is a user's favorite
   CLAP_PRESET_DISCOVERY_IS_FAVORITE = 1 << 3,
};

// Receiver that receives the metadata for a single preset file.
// The host would define the various callbacks in this interface and the preset parser function
// would then call them.
//
// This interface isn't thread-safe.
typedef const struct clap_preset_discovery_metadata_receiver {
   // If there is an error reading metadata from a file this should be called with an error
   // message.
   // os_error: the operating system error, if applicable. If not applicable set it to a non-error
   // value: 0 on unix and Windows.
   void(CLAP_ABI *on_error)(const struct clap_preset_discovery_metadata_receiver *receiver,
                            int32_t                                               os_error,
                            const char                                           *error_message);

   // Marks this file as a container file meaning that it can contain other presets.
   void(CLAP_ABI *mark_as_container_file)(
      const struct clap_preset_discovery_metadata_receiver *receiver);

   // If the file being parsed is a preset container file (mark_as_container_file has been called)
   // then this must be called for every preset in the file and before any preset metadata is
   // sent with the calls below. If the file is not a container file then this should not be
   // called at all.
   //
   // The path defines a human friendly path to the preset in the container file. It
   // should be unique within the container file.
   //
   // The preset_id is a machine friendly string used to load the preset inside the container via a
   // the preset-load plug-in extension. The preset_id can also just be the path if that's what the
   // extension wants but it could also be some other unique id like a database primary key or a
   // binary offset. It's use is entirely up to the plug-in.
   void(CLAP_ABI *begin_contained_preset)(
      const struct clap_preset_discovery_metadata_receiver *receiver,
      const char                                           *path,
      const char                                           *preset_id);

   // Marks this preset as a bank preset, meaning that it can be assigned to the plug-in as a
   // preset but will update the banks in the plug-in.
   void(CLAP_ABI *mark_as_bank_preset)(
      const struct clap_preset_discovery_metadata_receiver *receiver);

   // Sets plug-in id that this preset can be used with.
   void(CLAP_ABI *set_plugin_id)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                 const char                                           *plugin_id);

   // Sets the collection to which the preset belongs to.
   void(CLAP_ABI *set_collection_id)(const struct clap_preset_discovery_metadata_receiver,
                                     const char *collection_id);

   // Sets the flags, see clap_preset_discovery_flags.
   // If unset, they are then inherited from the location.
   void(CLAP_ABI *set_flags)(const struct clap_preset_discovery_metadata_receiver, uint32_t flags);

   // Sets the preset name.
   // The preset name can be infered from the file name, but it is mandatory to set it for
   // container's preset.
   void(CLAP_ABI *set_name)(const struct clap_preset_discovery_metadata_receiver *receiver,
                            const char                                           *name);

   // Adds a creator name for the preset.
   void(CLAP_ABI *add_creator)(const struct clap_preset_discovery_metadata_receiver *receiver,
                               const char                                           *creator);

   // Sets a description of the preset.
   void(CLAP_ABI *set_description)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                   const char *description);

   // Sets a color tag associated to the preset.
   void(CLAP_ABI *set_color_tag)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                 const clap_color_t                                   *color);

   // Sets the creation time and last modification time of the preset.
   // If one of the time isn't known, then set it to 0.
   void(CLAP_ABI *set_timestamps)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  uint64_t creation_time,
                                  uint64_t modification_time);

   // Adds a feature to the preset.
   // See plugin-features.h and preset-features.h
   //
   // The feature string is arbitrary, it is the indexer's job to understand it and remap it to its
   // internal categorization and tagging system.
   //
   // Examples:
   // kick, drum, tom, snare, clap, cymbal, bass, lead, metalic, hardsync, crossmod, acid,
   // distorted, drone, pad, dirty, etc...
   void(CLAP_ABI *add_feature)(const struct clap_preset_discovery_metadata_receiver *receiver,
                               const char                                           *feature);

} clap_preset_discovery_metadata_receiver_t;

typedef struct clap_preset_discovery_filetype {
   const char *name;
   const char *description;

   // `.' isn't included in the string.
   // If empty or NULL then every file should be matched.
   const char *file_extension;

   // This icon is optional and will be associated with the filetype.
   // It comes in multiple variations but the plugin can use 4 times the same image.
   // small shall have less details than the large one.
   // light and dark variation, so the host can pick the one which works best for its background.
   const char *icon_uri_small_light;
   const char *icon_uri_small_dark;
   const char *icon_uri_large_light;
   const char *icon_uri_large_dark;
} clap_preset_discovery_filetype_t;

// Defines a place in which to search for presets
typedef struct clap_preset_discovery_location {
   uint32_t flags; // see enum clap_preset_discovery_flags

   // name of this location
   const char *name;

   // URI
   // - file:/// for pointing to a file or directory; directories are scanned recursively
   //   eg: file:///home/abique/.u-he/Diva/presets/
   // - plugin://<clap-plugin-id> for presets which are bundled inside the plugin itself
   const char *uri;
} clap_preset_discovery_location_t;

// A collection, represent a collection of presets; it is will most often used to identify presets
// which belongs to the same "sound pack".
typedef struct clap_preset_discovery_collection {
   uint64_t flags; // see enum clap_preset_discovery_flags

   const char *id;           // collection identifier
   const char *name;         // name of this collection
   const char *description;  // reasonably short description of the collection
   const char *homepage_url; // url to the pack's homepage

   const char *image_uri;    // may be an image on disk or from an http server
} clap_preset_discovery_collection_t;

// Describes a preset provider
typedef struct clap_preset_discovery_provider_descriptor {
   clap_version_t     clap_version; // initialized to CLAP_VERSION
   const char        *id;
   const char        *name;
   const char        *vendor;
   const char *const *plugin_ids; // null terminated array of plugin ids
} clap_preset_discovery_provider_descriptor_t;

// This interface isn't thread-safe.
typedef struct clap_preset_discovery_provider {
   const clap_preset_discovery_provider_descriptor_t *desc;

   // Destroys the preset provider
   void(CLAP_ABI *destroy)(const struct clap_preset_discovery_provider *provider);

   // Asks the preset provider to register its locations
   void(CLAP_ABI *register_locations)(const struct clap_preset_discovery_provider *provider);

   // Asks the preset provider to register its filetypes
   void(CLAP_ABI *register_filetypes)(const struct clap_preset_discovery_provider *provider);

   // Asks the preset provider to register its collections
   void(CLAP_ABI *register_collections)(const struct clap_preset_discovery_provider *provider);

   // Retrives the path to a watch file.
   // Whenever the given file is "touched", then the indexer shall invalidate all the data.
   bool(CLAP_ABI *invalidation_watch_file)(const struct clap_preset_discovery_provider *provider,
                                           char    *watch_file_path,
                                           uint32_t watch_file_path_capacity);

   // reads metadata from the given file and passes them to the metadata receiver
   bool(CLAP_ABI *get_metadata)(const struct clap_preset_discovery_provider     *provider,
                                const char                                      *uri,
                                const clap_preset_discovery_metadata_receiver_t *metadata_receiver);
} clap_preset_discovery_provider_t;

typedef struct clap_preset_discovery_indexer {
   clap_version_t clap_version; // initialized to CLAP_VERSION
   const char    *name;
   const char    *vendor;
   const char    *version;

   // Registers a preset filetype
   void(CLAP_ABI *register_filetype)(const struct clap_preset_discovery_indexer *indexer,
                                     const clap_preset_discovery_filetype_t     *filetype);

   // Registers a preset location
   void(CLAP_ABI *register_location)(const struct clap_preset_discovery_indexer *indexer,
                                     const clap_preset_discovery_location_t     *location);

   // Registers a preset collection
   void(CLAP_ABI *register_collection)(const struct clap_preset_discovery_indexer *indexer,
                                       const clap_preset_discovery_collection_t   *collection);
} clap_preset_indexer_t;

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
   const clap_preset_discovery_provider_descriptor_t *(CLAP_ABI *get_descriptor)(
      const struct clap_preset_discovery_factory *factory, uint32_t index);

   // Create a preset provider by its id.
   // The returned pointer must be freed by calling preset_provider->destroy(preset_provider);
   // The preset provider is not allowed to use the indexer callbacks in the create method.
   // Returns null in case of error.
   // [thread-safe]
   const clap_preset_discovery_provider_t *(CLAP_ABI *create)(
      const struct clap_preset_discovery_factory *factory,
      const clap_preset_indexer_t                *indexer,
      const char                                 *provider_id);
} clap_preset_discovery_factory_t;

#ifdef __cplusplus
}
#endif
