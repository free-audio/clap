<p align=center>
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/free-audio/clap/main/artwork/clap-full-logo-white.png">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/free-audio/clap/main/artwork/clap-full-logo-black.png">
    <img alt="CLAP" title="Clever Audio Plugin" src="https://raw.githubusercontent.com/free-audio/clap/main/artwork/clap-full-logo-black.png" width=200>
  </picture>
</p>

- [Learn about CLAP](#learn-about-clap)
  - [Entry point](#entry-point)
  - [Extensions](#extensions)
  - [Fundamental extensions](#fundamental-extensions)
  - [Support extensions](#support-extensions)
  - [Extra extensions](#extra-extensions)
  - [Third-party extensions](#third-party-extensions)
- [Adapters](#adapters)
- [Resources](#resources)
  - [Examples](#examples)
  - [Community related projects](#community-related-projects)
  - [Programming Language Bindings](#programming-language-bindings)
  - [Artwork](#artwork)

# Learn about CLAP

CLAP stands for **CL**ever **A**udio **P**lugin.
It is an audio plugin ABI which defines a standard for *Digital Audio Workstations* and audio plugins (synthesizers, audio effects, ...) to work together.

To work with CLAP, include [clap/clap.h](include/clap/clap.h).

The two most important objects are `clap_host` and `clap_plugin`.

[src/plugin-template.c](src/plugin-template.c) is a very minimal example which demonstrates how to wire a CLAP plugin.

## Entry point

The entry point is declared in [entry.h](include/clap/entry.h).

## Extensions

Most features come from extensions, which are in fact C interfaces.
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

The extensions are defined in the [ext](include/clap/ext) folder.

Some extensions are still in the progress of being designed and they are in
the [draft](include/clap/ext/draft) folder.

An extension comes with:
- a header `#include <clap/ext/xxx.h>`
- an extension identifier: `#define CLAP_EXT_XXX "clap/XXX"`
- host interfaces are named like: `struct clap_host_xxx`
- plugin interfaces are named like: `struct clap_plugin_xxx`
- each method must have a clear thread specification

You can create your own extensions and share them. Make sure that the extension identifier:
- includes versioning in case the ABI breaks
- is a unique identifier

**All strings are valid UTF-8**.

## Fundamental extensions

This is a list of the extensions that you most likely want to implement
and use to get a basic plugin experience:
- [log](include/clap/ext/log.h), lets the host aggregate plugin logs
- [thread-check](include/clap/ext/thread-check.h), check which thread you are currently on, useful for correctness validation
- [audio-ports](include/clap/ext/audio-ports.h), define the audio ports
- [note-ports](include/clap/ext/note-ports.h), define the note ports
- [params](include/clap/ext/params.h), parameters management
- [latency](include/clap/ext/latency.h), report the plugin latency
- [render](include/clap/ext/render.h), renders realtime or offline
- [tail](include/clap/ext/tail.h), processing tail length
- [state](include/clap/ext/state.h), save and load the plugin state
- [gui](include/clap/ext/gui.h), generic gui controller

## Support extensions

- [thread-pool](include/clap/ext/thread-pool.h), use the host thread pool
- [timer-support](include/clap/ext/timer-support.h), lets the plugin register timer handlers
- [posix-fd-support](include/clap/ext/posix-fd-support.h), lets the plugin register I/O handlers

## Extra extensions

- [note-name](include/clap/ext/note-name.h), give a name to notes, useful for drum machines
- [tuning](include/clap/ext/draft/tuning.h), host provided microtuning
- [track-info](include/clap/ext/draft/track-info.h)
- [quick-controls](include/clap/ext/draft/quick-controls.h), bank of controls that can be mapped on a controlles with 8 knobs
- [file-reference](include/clap/ext/draft/file-reference.h), let the host know about the plugin's file reference, and perform "Collect & Save"
- [check-for-update](include/clap/ext/draft/check-for-update.h), check if there is a new version of a plugin
- [audio-ports-config](include/clap/ext/audio-ports-config.h), simple list of possible configurations
- [surround](include/clap/ext/draft/surround.h), inspect surround channel mapping
- [ambisonic](include/clap/ext/draft/ambisonic.h), inspect ambisonic channel mapping

## Third-party extensions

- [`cockos.reaper_extension`](https://github.com/justinfrankel/reaper-sdk/blob/main/reaper-plugins/reaper_plugin.h#L138), access the [REAPER](http://reaper.fm) API

# Adapters

- [clap-wrapper](https://github.com/free-audio/clap-wrapper), wrappers for using CLAP in other plugin environments

# Resources

- [clap-validator](https://github.com/robbert-vdh/clap-validator), a validator and automatic test suite for CLAP plugins.
- [clapdb](https://clapdb.tech), a list of plugins and DAWs which supports CLAP

## Examples

- [clap-host](https://github.com/free-audio/clap-host), very simple host
- [clap-plugins](https://github.com/free-audio/clap-plugins), very simple plugins

## Community related projects

- [clap-juce-extension](https://github.com/free-audio/clap-juce-extension), juce add-on
- [MIP2](https://github.com/skei/MIP2), host and plugins
- [Avendish](https://github.com/celtera/avendish), a reflection-based API for media plug-ins in C++ which supports Clap
- [NIH-plug](https://github.com/robbert-vdh/nih-plug), an API-agnostic, Rust-based plugin framework aiming to reduce boilerplate without getting in your way

## Programming Language Bindings

- [clap-sys](https://github.com/glowcoil/clap-sys), rust binding
- [CLAP-for-Delphi](https://github.com/Bremmers/CLAP-for-Delphi), Delphi binding

## Artwork

 - [CLAP Logo Pack.zip](https://github.com/free-audio/clap/files/8805281/CLAP.Logo.Pack.zip)
