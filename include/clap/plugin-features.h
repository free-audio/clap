#pragma once

#include "private/macros.h"

// This files provides a set of standard plugin features meant to be use
// within clap_plugin_descriptor.features.
//
// For practical reasons we'll avoid spaces and use `-` instead to facilitate
// scripts that generate the feature array.
//
// Non standard feature should be formated as follow: "$namespace:$feature"

/////////////////////
// Plugin category //
/////////////////////

// Add this feature if your plugin can process note events and then produce audio
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_INSTRUMENT = "instrument";

// Add this feature if your plugin is an audio effect
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_AUDIO_EFFECT = "audio-effect";

// Add this feature if your plugin is a note effect or a note generator/sequencer
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_NOTE_EFFECT = "note-effect";

// Add this feature if your plugin is an analyzer
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_ANALYZER = "analyzer";

/////////////////////////
// Plugin sub-category //
/////////////////////////

static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_FILTER = "filter";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_PHASER = "phaser";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_EQUALIZER = "equalizer";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_DEESSER = "de-esser";

static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_DISTORTION = "distortion";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_TRANSIENT_SHAPER = "transient-shaper";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_COMPRESSOR = "compressor";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_LIMITER = "limiter";

static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_FLANGER = "flanger";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_CHORUS = "chorus";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_DELAY = "delay";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_REVERB = "reverb";

static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_UTILITY = "utility";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_GLITCH = "glitch";

////////////////////////
// Audio Capabilities //
////////////////////////

static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_MONO = "mono";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_STEREO = "stereo";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_SURROUND = "surround";
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_AMBISONIC = "ambisonic";

/////////////////
// GUI related //
/////////////////

// Add this feature if the plugin is DPI aware on Windows.
static CLAP_CONSTEXPR const char *CLAP_PLUGIN_FEATURE_WIN32_DPI_AWARE = "win32-dpi-aware";
