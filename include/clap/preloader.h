#pragma once

#include "private/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Preloaders are lightweight objects that allow preparing changes to plugin state
/// asynchronously in a background thread, and then atomically committing those changes later on the
/// main thread.
///
/// Preloaders can be thought of as a mirror image of plugins, except Preloaders can be manipulated
/// in other threads, and Preloaders cannot manipulate audio or events at all.
///
/// Preloaders usually "belong" to a semantic thread call the Preloader Thread, and most of their
/// methods are therefore marked as `[preloader-thread]`. See ext/thread-check.h for more details.
///
/// # Use case examples
///
/// Loading a new plugin instance (e.g. on Project load) from a background preloader thread:
///
/// * [preloader-thread] clap_preloader_factory.create
/// * [preloader-thread] clap_preloader.init
/// * [preloader-thread] clap_plugin_preloader_state.load
/// * [preloader-thread] clap_preloader_activate.prepare_activate (optional, if supported)
/// * [preloader-thread] clap_preloader.into_plugin
/// * [main-thread] clap_plugin.init
/// * [main-thread] clap_plugin.activate
///
/// Asynchronous loading:
///
/// * [preloader-thread] clap_preloader_factory.create
/// * [preloader-thread] clap_preloader.init
/// * [preloader-thread] clap_plugin_preloader_state.load
/// * [main-thread] clap_plugin_preloader.commit
/// * [preloader-thread] clap_preloader.destroy
///
/// Asynchronous saving:
///
/// * [main-thread] clap_plugin_preloader.create
/// * [preloader-thread] clap_preloader.init
/// * [preloader-thread] clap_plugin_preloader_state.save
/// * [preloader-thread] clap_preloader.destroy
///
/// More use-cases for this interface can also be added with future extensions.

/// Host-side callbacks and metadata for a preloader.
///
/// This can be thought as the Preloader equivalent to `clap_host`.
typedef struct clap_preloader_host {
   clap_version_t clap_version; // initialized to CLAP_VERSION

   void *preloader_host_data; // reserved pointer for the host

   /// The `name` and `version` fields are mandatory and should not be NULL nor point to empty
   /// strings.
   ///
   /// These fields should have the same values as the ones provided by `clap_host`.
   const char *name;    // eg: "Bitwig Studio"
   const char *vendor;  // eg: "Bitwig GmbH"
   const char *url;     // eg: "https://bitwig.com"
   const char *version; // eg: "4.3", see plugin.h for advice on how to format the version

   /// Queries an extension.
   ///
   /// The returned pointer is owned by the host.
   ///
   /// It is forbidden to call this function before `clap_preloader.init`.
   /// Hosts may call this function during the `clap_preloader.init` call, and afterward if `init`
   /// succeeds.
   // [thread-safe]
   const void *(CLAP_ABI *get_extension)(const struct clap_preloader_host *host,
                                         const char                       *extension_id);
} clap_preloader_host_t;

/// A Preloader instance.
///
/// Preloader types always match Plugin types one-to-one, and as such they share the same ID.
/// It is always an error to use a Preloader with another Plugin type than the one it is
/// representing.
///
/// This is initialized and destroyed in the same manner as a `clap_plugin`, using the `init` and
/// `destroy` functions.
///
/// Preloaders can be created one of two ways:
/// * From a plugin ID, using `clap_preloader_factory.create`;
/// * Directly from a plugin instance, using `clap_plugin_preloader.create`.
///
/// Once created, a Preloader's state can be modified in the background `preloader-thread`, and then
/// applied to a plugin instance using `clap_plugin_preloader.commit`, or used to create a brand-new
/// plugin instance with `clap_preloader.into_plugin`.
///
/// Note that by default, this interface provides no way to change or use a Preloader's inner state,
/// nor does it define what that state actually is.
///
/// All changes and uses for a Preloader are done via Preloader Extensions, such as
/// `clap_preloader_activate` or `clap_plugin_preloader_state`.
/// There is no required set of extensions either hosts or plugins have to implement in order to use
/// Preloaders.
/// This allows hosts and plugins to pick, choose and then negotiate which operations can be
/// offloaded to background threads.
///
/// All supported operations must also be supported via the "regular" [main-thread] interfaces and
/// extensions as a fallback. E.g. if a plugin supports `clap_plugin_preloader_state`, it must also
/// support `clap_plugin_state` as a fallback in case the host doesn't support it, with the same
/// resulting behavior.
///
/// # Committing and Snapshotting
///
/// This specification uses the terms "Committing" and "Snapshotting" to describe the following
/// operations:
///
/// * A "Commit" operation applies a Preloader instance's state to a plugin instance;
/// * A "Snapshot" operation copies a plugin instance's state into a Provider instance.
///
/// Committing takes the entire state of the Preloader instance (which may have been created by the
/// accumulation of several different operations), and transfers it all to a given Plugin instance
/// in a single operation. When successful, a Commit operation leaves the Preloader in an "empty"
/// state, i.e. the same state a Preloader is in when freshly created from
/// `clap_preloader_factory.create`.
///
/// Committing an "empty" Preloader to any Plugin instance is always equivalent to a no-op.
///
/// To make the Commit operation faster, plugin implementations may transfer old, unused resources
/// back into the Preloader for it to deallocate in another thread (when `clap_preloader.destroy`
/// is called), rather than deallocating them in the main thread.
///
/// Snapshotting is the opposite operation to Committing, and takes the entire state of a Plugin
/// instance and copies it to a Preloader instance.
/// Snapshotting overwrites any state that the Preloader may still contain. Therefore, Snapshotting
/// to a Preloader instance that already has e.g. state loaded, is equivalent to Snapshotting to a
/// new, "empty" Preloader instance.
///
/// Snapshotting is a read-only operation for the Plugin instance, after which it may continue to
/// process audio, receive events, etc., as usual.
///
/// Unlike with Committing, Snapshotting a brand-new (i.e. default) Plugin instance is not a no-op,
/// it only results in a Preloader instance containing the "Default" preset.
/// Note that this does *not* result in an "empty" Preloader
///
/// The goal of Preloaders is to move as much work as possible away from the main thread.
/// Therefore, plugin implementations should strive to make Commit and Snapshot operations as quick
/// as possible, and should avoid to perform any long-running operations (such as I/O).
///
/// Despite this however, Commit and Snapshot operations may still perform non-realtime-safe
/// operations, such as memory allocation.
///
/// Committing and Snapshotting are always atomic operations. They either completely succeed, or
/// completely fail.
/// As such, Committing and Snapshotting cannot result in a "partial" or invalid state. A failed
/// Commit or Snapshot is equivalent to a no-op for both the Plugin and the Preloader.
///
typedef struct clap_preloader {
   /// The Plugin Descriptor of the matching Plugin type.
   ///
   /// This pointer may point to the same `clap_plugin_descriptor` instance as the pointer in the
   /// matching `clap_plugin` type.
   /// However, hosts should not rely on this to try and figure if a given preloader matches a given
   /// plugin. They should compare the values of the descriptor's `id` fields for that.
   const clap_plugin_descriptor_t *desc;

   void *preloader_data; // reserved pointer for the preloader

   /// Must be called after creating the preloader.
   ///
   /// Returns `true` on success, and `false` on failure.
   /// If this succeeds, then this preloader instance is initialized and ready to be used.
   /// If this fails, then the caller must not use any other functions on this instance, and should
   /// `destroy` it right away.
   ///
   /// Only starting from within this function call (and afterward if `init` succeeds), is the
   /// Preloader allowed to use the callbacks provided by the previously given `clap_preloader_host`
   /// instance.
   ///
   /// This works in the same manner as `clap_plugin.init`.
   // [preloader-thread]
   bool(CLAP_ABI *init)(const struct clap_preloader *preloader);

   /// Frees the Preloader and its resources.
   // [preloader-thread]
   void(CLAP_ABI *destroy)(const struct clap_preloader *preloader);

   /// Commits into a new plugin instance by consuming the preloader.
   ///
   /// If this succeeds, a new clap_plugin instance is created and returned, and the preloader is
   /// consumed.
   ///
   /// "Consumed" in this context means the Preloader transfers all of its allocated resources into
   /// the plugin instance, including itself.
   ///
   /// If this succeeds, that means the caller must then discard the `preloader` pointer, and must
   /// not call `clap_preloader.destroy` on it, or any other Preloader function.
   ///
   /// Just like with `clap_plugin_factory.create`, the plugin instance is not allowed to use the
   /// host callbacks in this method. It must wait until clap_plugin.init is called.
   /// Also like with `clap_plugin_factory.create`, the caller owns the newly created plugin
   /// instance, and must free it with `clap_plugin.destroy` when it is done with it.
   ///
   /// This returns NULL if the Commit operation failed.
   // [preloader-thread]
   const clap_plugin_t *(CLAP_ABI *into_plugin)(const struct clap_preloader *preloader,
                                                const clap_host_t           *host);

   /// Queries an extension.
   ///
   /// The returned pointer is owned by the plugin.
   ///
   /// It is forbidden to call this function before `clap_preloader.init`.
   /// Hosts may call this function during the `clap_preloader.init` call, and afterward if `init`
   /// succeeds.
   // [thread-safe]
   const void *(CLAP_ABI *get_extension)(const struct clap_preloader *preloader, const char *id);
} clap_preloader_t;

#ifdef __cplusplus
}
#endif
