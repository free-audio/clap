#pragma once

// This file provides a set of standard preset features meant to be used
// within clap_plugin_descriptor.features.
//
// For practical reasons we'll avoid spaces and use `-` instead to facilitate
// scripts that generate the feature array.
//
// Non-standard features should be formated as follow: "$namespace:$feature"

#include "plugin-features.h"

#define CLAP_PRESET_FEATURE_KICK "kick"
#define CLAP_PRESET_FEATURE_TOM "tom"
#define CLAP_PRESET_FEATURE_SNARE "snare"
#define CLAP_PRESET_FEATURE_CLAP "clap"
#define CLAP_PRESET_FEATURE_CYMBAL "cymbal"

#define CLAP_PRESET_FEATURE_BASS "bass"
#define CLAP_PRESET_FEATURE_LEAD "lead"
#define CLAP_PRESET_FEATURE_KEY "key"
#define CLAP_PRESET_FEATURE_PLUCK "pluck"
#define CLAP_PRESET_FEATURE_STAB "stab"
#define CLAP_PRESET_FEATURE_WIND "wind"
#define CLAP_PRESET_FEATURE_STRING "string"
#define CLAP_PRESET_FEATURE_PAD "pad"
#define CLAP_PRESET_FEATURE_DRONE "drone"

#define CLAP_PRESET_FEATURE_BRIGHT "bright"
#define CLAP_PRESET_FEATURE_DARK "dark"
#define CLAP_PRESET_FEATURE_SATURATED "saturated"
#define CLAP_PRESET_FEATURE_TAPE "tape"
#define CLAP_PRESET_FEATURE_DISTORDED "distorded"
#define CLAP_PRESET_FEATURE_METALIC "metalic"
#define CLAP_PRESET_FEATURE_ATONAL "atonal"
#define CLAP_PRESET_FEATURE_NOISE "noise"
#define CLAP_PRESET_FEATURE_DETUNED "detuned"
#define CLAP_PRESET_FEATURE_ACID "acid"
#define CLAP_PRESET_FEATURE_ANALOG "analog"
#define CLAP_PRESET_FEATURE_DIGITAL "digital"
#define CLAP_PRESET_FEATURE_CHIP "chip" // 8-bits, chiptune...

#define CLAP_PRESET_FEATURE_SEQUENCE "sequence"
#define CLAP_PRESET_FEATURE_ARPEGGIATOR "arpeggiator"
#define CLAP_PRESET_FEATURE_CHORD "chord"
