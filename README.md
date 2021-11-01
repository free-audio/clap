```
    _______      .---.         ____     .-------.
   /   __  \     | ,_|       .'  __ `.  \  _(`)_ \
  | ,_/  \__)  ,-./  )      /   '  \  \ | (_ o._)|
,-./  )        \  '_ '`)    |___|  /  | |  (_,_) /
\  '_ '`)       > (_)  )       _.-`   | |   '-.-'
 > (_)  )  __  (  .  .-'    .'   _    | |   |
(  .  .-'_/  )  `-'`-'|___  |  _( )_  | |   |
 `-'`-'     /    |        \ \ (_ o _) / /   )
   `._____.'     `--------`  '.(_,_).'  `---'

```

**CL**ever **A**udio **P**lugin.

# Learn about CLAP

The starting point is [clap.h](include/clap/clap.h).

The two most important objects are `clap_host` and `clap_plugin`.

Most features comes from extensions, which are in fact C interfaces.
```C
// host extension
const clap_host_log *log = host->extension(host, CLAP_EXT_LOG);
if (log)
   log->log(host, CLAP_LOG_INFO, "Hello World! ;^)");

// plugin extension
const clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
if (params)
{
   uint32_t paramsCount = params->count(plugin);
   // ...
}
```

The extensions are defined in [ext](include/clap/ext) folder.

Some extensions are still in the progress of being designed and they are in
the [draft](include/clap/ext/draft) folder.

An extension comes with:
- an header `#include <clap/ext/xxx.h>`
- an extension identifier: `#define CLAP_EXT_XXX "clap/XXX"`
- host interfaces are named like: `struct clap_host_xxx`
- plugin interfaces are named like: `struct clap_plugin_xxx`
- each methods must have a clear thread specification

You can create your own extensions and share them, make sure that the extension identifier
- includes versioning in case the ABI breaks
- a unique identifier

**All strings are valid UTF-8**.

## Fundamental extensions

This is a list of the extensions that you most likely want to implement
and use to get a basic plugin experience:
- [log](include/clap/ext/log.h), lets the host aggregate plugin logs
- [thread-check](include/clap/ext/thread-check.h), check which thread you are currently on, useful for correctness validation
- [audio-ports](include/clap/ext/audio-ports.h), define and configure the ports layout
- [params](include/clap/ext/params.h), parameters management
- [latency](include/clap/ext/latency.h), report the plugin latency
- [state](include/clap/ext/state.h), save and load the plugin state

## GUI extensions

- [gui](include/clap/ext/gui.h), generic gui controller
- [gui-win32](include/clap/ext/gui-win32.h), win32 specific
- [gui-cocoa](include/clap/ext/gui-cocoa.h), cocoa specific
- [gui-x11](include/clap/ext/gui-x11.h), x11 specific
- [timer-support](include/clap/ext/timer-support.h), lets the plugin register timer handlers
- [fd-support](include/clap/ext/fd-support.h), lets the plugin register I/O handlers

## Extra extensions

- [render](include/clap/ext/render.h), renders realtime or offline
- [note-name](include/clap/ext/draft/note-name.h), give a name to notes, useful for drum machines
- [tuning](include/clap/ext/draft/tuning.h), host provided microtuning
- [track-info](include/clap/ext/draft/track-info.h)
- [thread-pool](include/clap/ext/draft/thread-pool.h), use the host thread pool
- [quick-controls](include/clap/ext/draft/quick-controls.h), bank of controls that can be mapped on a controlles with 8 knobs
- [file-reference](include/clap/ext/draft/file-reference.h), let the host know about the plugin's file reference, and perform "Collect & Save"
- [check-for-update](include/clap/ext/draft/check-for-update.h), check if there is a new version of a plugin

## Examples

- [clap-host](https://github.com/free-audio/clap-host)
- [clap-plugins](https://github.com/free-audio/clap-plugins)
