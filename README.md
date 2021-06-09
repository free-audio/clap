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

**CL**ap is an **A**udio **P**lugin API.

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
- an header
- an extension identifier: `#define CLAP_EXT_XXX "clap/XXX"`
- host interfaces are named like: `stuct clap_host_xxx`
- plugin interfaces are named like: `stuct clap_plugin_xxx`
- each methods must have a clear thread specification

You can create your own extensions and share them, make sure that the extension identifier
- includes versioning in case the ABI breaks
- a unique identifier

**All strings are valid UTF-8**.

## Examples

Visit the [examples](examples) folder.
