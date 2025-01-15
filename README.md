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
  - [Deeper Host integration](#deeper-host-integration)
  - [Third-party extensions](#third-party-extensions)
- [Adapters](#adapters)
- [Resources](#resources)
  - [Examples](#examples)
  - [Community related projects](#community-related-projects)
  - [Programming Language Bindings](#programming-language-bindings)
  - [Artwork](#artwork)

# Learn about CLAP

CLAP stands for **CL**ever **A**udio **P**lugin. It is an interface that
provides a stable ABI to define a standard for *Digital Audio Workstations* and
audio plugins (synthesizers, audio effects, ...) to work together.

The ABI, or **A**pplication **B**inary **I**nterface, serves as a means of
communication between a host and a plugin. It provides backwards compatibility,
that is, a plugin binary compiled with CLAP 1.x can be loaded by any other
CLAP 1.y.

To work with CLAP, include [clap/clap.h](include/clap/clap.h).
To also include the draft extensions, include [clap/all.h](include/clap/all.h).

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
- [state](include/clap/ext/state.h), save and load the plugin state
  - [state-context](include/clap/ext/state-context.h), same as state but with additional context info (preset, duplicate, project)
  - [resource-directory](include/clap/ext/draft/resource-directory.h), host provided folder for the plugin to save extra resource like multi-samples, ... (draft)
- [params](include/clap/ext/params.h), parameters management
- [note-ports](include/clap/ext/note-ports.h), define the note ports
- [audio-ports](include/clap/ext/audio-ports.h), define the audio ports
  - [surround](include/clap/ext/surround.h), inspect surround channel mapping
  - [ambisonic](include/clap/ext/draft/ambisonic.h), inspect ambisonic channel mapping
  - [configurable-audio-ports](include/clap/ext/configurable-audio-ports.h), request the plugin to apply a given configuration
  - [audio-ports-config](include/clap/ext/audio-ports-config.h), simple list of pre-defined audio ports configurations, meant to be exposed to the user
  - [audio-ports-activation](include/clap/ext/audio-ports-activation.h), activate and deactivate a given audio port
  - [extensible-audio-ports](include/clap/ext/draft/extensible-audio-ports.h), let the host add audio ports to the plugin, this is useful for dynamic number of audio inputs (draft)
- [render](include/clap/ext/render.h), renders realtime or offline
- [latency](include/clap/ext/latency.h), report the plugin latency
- [tail](include/clap/ext/tail.h), processing tail length
- [gui](include/clap/ext/gui.h), generic gui controller
- [voice-info](include/clap/ext/voice-info.h), let the host know how many voices the plugin has, this is important for polyphonic modulations
- [track-info](include/clap/ext/track-info.h), give some info to the plugin about the track it belongs to
- [tuning](include/clap/ext/draft/tuning.h), host provided microtuning (draft)
- [triggers](include/clap/ext/draft/triggers.h), plugin's triggers, similar to parameters but stateless

## Support extensions

- [thread-check](include/clap/ext/thread-check.h), check which thread you are currently on, useful for correctness validation
- [thread-pool](include/clap/ext/thread-pool.h), use the host thread pool
- [log](include/clap/ext/log.h), lets the host aggregate plugin logs
- [timer-support](include/clap/ext/timer-support.h), lets the plugin register timer handlers
- [posix-fd-support](include/clap/ext/posix-fd-support.h), lets the plugin register I/O handlers

## Deeper Host integration

- [remote-controls](include/clap/ext/remote-controls.h), bank of controls that can be mapped on a controlles with 8 knobs
- [preset-discovery](include/clap/factory/preset-discovery.h), let the host index the plugin's preset in their native file format
- [preset-load](include/clap/ext/preset-load.h), let the host ask the plugin to load a preset
- [param-indication](include/clap/ext/param-indication.h), let the plugin know when a physical control is mapped to a parameter and if there is automation data
- [note-name](include/clap/ext/note-name.h), give a name to notes, useful for drum machines
- [transport-control](include/clap/ext/draft/transport-control.h), let the plugin control the host's transport (draft)
- [context-menu](include/clap/ext/context-menu.h), exchange context menu entries between host and plugin, let the plugin ask the host to popup its own context menu

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
- [iPlug2](https://iplug2.github.io), a liberally licensed C++ audio plug-in framework that supports Clap

## Programming Language Bindings

- [clap-sys](https://github.com/glowcoil/clap-sys), rust binding
- [CLAP-for-Delphi](https://github.com/Bremmers/CLAP-for-Delphi), Delphi binding
- [clap-zig-bindings](https://sr.ht/~interpunct/clap-zig-bindings/), Zig bindings
- [CLAP for Ada](https://github.com/ficorax/cfa), Ada 2012 binding

## Artwork

 - [CLAP Logo Pack.zip](https://github.com/free-audio/clap/files/8805281/CLAP.Logo.Pack.zip)
