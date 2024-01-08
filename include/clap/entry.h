#pragma once

#include "version.h"
#include "private/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

// This interface is the entry point of the dynamic library.
//
// CLAP plugins standard search path:
//
// Linux
//   - ~/.clap
//   - /usr/lib/clap
//
// Windows
//   - %COMMONPROGRAMFILES%\CLAP
//   - %LOCALAPPDATA%\Programs\Common\CLAP
//
// MacOS
//   - /Library/Audio/Plug-Ins/CLAP
//   - ~/Library/Audio/Plug-Ins/CLAP
//
// In addition to the OS-specific default locations above, a CLAP host must query the environment
// for a CLAP_PATH variable, which is a list of directories formatted in the same manner as the host
// OS binary search path (PATH on Unix, separated by `:` and Path on Windows, separated by ';', as
// of this writing).
//
// Each directory should be recursively searched for files and/or bundles as appropriate in your OS
// ending with the extension `.clap`.
//
// As detailed in the respective functions' documentations below, the host is forbidden from calling
// either init() or deinit() multiple times, without calling the other first. This is to enforce
// a "normal" initialization pattern, where one can't construct or destroy an object multiple times.
//
// However, there are a few cases where the host may not be able to reliably prevent this from
// happening, such as when multiple, separate hosts co-exist within the same address space (e.g.
// Meta-Plugins, plugins that can host other plugins inside them). It may also happen when a VST
// compatibility layer is used to wrap a CLAP plugin, for instance.
//
// In those cases, the init() and deinit() pair of functions can happen to be called multiple times,
// either in a row or even simultaneously, from different threads, by multiple non-synchronized
// hosts, or by a single host that is presented multiple interfaces to a same DSO. This can happen
// even though this is forbidden behavior from the host by this specification, and despite a
// compliant host's best efforts.
//
// Despite those issues, hosts *must* ensure that each successful call to init() is only followed by
// a single call to deinit() at most. As long as a single successful init() call did not have its
// matching deinit() called, the DSO *must* remain in a fully operational state, as if deinit() had
// not yet been called.
//
// Therefore, init() and deinit() functions *should* be implemented in a way that allows for init()
// to be called any number of times in a row (possibly simultaneously, from multiple threads), and
// for deinit() to be called at most that same number of times (also simultaneously, from multiple,
// possibly different threads from the init() called). As of CLAP 1.11, this *must* be the case for
// all plugin implementations.
//
// This is already trivially the case if there is no (de)initialization work in the entry at all,
// or if the initialization work is thread safe and idempotent, and there is no de-initialization
// work to be done. Those implementations already are and remain valid, and require no additional
// consideration.
//
// If that is not the case, then using a global, refcount-like locked initialization counter can be
// used to keep track of init() and deinit() calls. Then the initialization work can be done only
// when the counter goes from 0 to 1 in init(), and de-initialization can be done when the counter
// goes to 0 in deinit(). Any subsequent calls to init() and deinit() can just increase or decrease
// the counter, respectively.
typedef struct clap_plugin_entry {
   clap_version_t clap_version; // initialized to CLAP_VERSION

   // Initializes the DSO.
   //
   // This function must be called first, before any-other CLAP-related function or symbol from this
   // DSO.
   //
   // It also must only be called once, until a later call to deinit() is made, after which init()
   // can be called once more to re-initialize the DSO.
   // This enables hosts to e.g. quickly load and unload a DSO for scanning its plugins, and then
   // load it again later to actually use the plugins if needed.
   //
   // As stated above, even though hosts are forbidden to do so directly, multiple calls before any
   // deinit() call may still happen. Implementations *should* take this into account, and *must*
   // do so as of CLAP 1.11.
   //
   // It should be as fast as possible, in order to perform a very quick scan of the plugin
   // descriptors.
   //
   // It is forbidden to display graphical user interfaces in this call.
   // It is forbidden to perform any user interaction in this call.
   //
   // If the initialization depends upon expensive computation, maybe try to do them ahead of time
   // and cache the result.
   //
   // Returns true on success. If init() returns false, then the DSO must be considered
   // uninitialized, and the host must not call deinit() nor any other CLAP-related symbols from the
   // DSO.
   // This function also returns true in the case where the DSO is already initialized, and no
   // actual initialization work is done in this call, as explain above.
   //
   // plugin_path is the path to the DSO (Linux, Windows), or the bundle (macOS).
   //
   // This function may be called on any thread, including a different one from the one a later call
   // to deinit() (or a later init()) can be made.
   // However, it is forbidden to call this function simultaneously from multiple threads.
   // It is also forbidden to call it simultaneously with *any* other CLAP-related symbols from the
   // DSO, including (but not limited to) deinit().
   bool(CLAP_ABI *init)(const char *plugin_path);

   // De-initializes the DSO, freeing any resources allocated or initialized by init().
   //
   // After this function is called, no more calls into the DSO must be made, except calling init()
   // again to re-initialize the DSO.
   // This means that after deinit() is called, the DSO can be considered to be in the same state
   // as if init() was never called at all yet, enabling it to be re-initialized as needed.
   //
   // As stated above, even though hosts are forbidden to do so directly, multiple calls before any
   // new init() call may still happen. Implementations *should* take this into account, and *must*
   // do so as of CLAP 1.11.
   //
   // Just like init(), this function may be called on any thread, including a different one from
   // the one init() was called from, or from the one a later init() call can be made.
   // However, it is forbidden to call this function simultaneously from multiple threads.
   // It is also forbidden to call it simultaneously with *any* other CLAP-related symbols from the
   // DSO, including (but not limited to) deinit().
   void(CLAP_ABI *deinit)(void);

   // Get the pointer to a factory. See factory/plugin-factory.h for an example.
   //
   // Returns null if the factory is not provided.
   // The returned pointer must *not* be freed by the caller.
   //
   // Unlike init() and deinit(), this function can be called simultaneously by multiple threads.
   //
   // [thread-safe]
   const void *(CLAP_ABI *get_factory)(const char *factory_id);
} clap_plugin_entry_t;

/* Entry point */
CLAP_EXPORT extern const clap_plugin_entry_t clap_entry;

#ifdef __cplusplus
}
#endif
