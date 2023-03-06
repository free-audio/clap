#pragma once

// This file provides a set of standard plugin features meant to be used
// within clap_plugin_descriptor.features.
//
// For practical reasons we'll avoid spaces and use `-` instead to facilitate
// scripts that generate the feature array.
//
// Non-standard features should be formatted as follow: "$namespace:$feature"

/////////////////////
// Plugin category //
/////////////////////

// Add this feature if your plugin can process note events and then produce audio
#define CLAP_PLUGIN_FEATURE_INSTRUMENT "instrument"

// Add this feature if your plugin is an audio effect
#define CLAP_PLUGIN_FEATURE_AUDIO_EFFECT "audio-effect"

// Add this feature if your plugin is a note effect or a note generator/sequencer
#define CLAP_PLUGIN_FEATURE_NOTE_EFFECT "note-effect"

// Add this feature if your plugin converts audio to notes
#define CLAP_PLUGIN_FEATURE_NOTE_DETECTOR "note-detector"

// Add this feature if your plugin is an analyzer
#define CLAP_PLUGIN_FEATURE_ANALYZER "analyzer"

/////////////////////////
// Plugin sub-category //
/////////////////////////

#define CLAP_PLUGIN_FEATURE_SYNTHESIZER "synthesizer"
#define CLAP_PLUGIN_FEATURE_SAMPLER "sampler"
#define CLAP_PLUGIN_FEATURE_DRUM "drum" // For single drum
#define CLAP_PLUGIN_FEATURE_DRUM_MACHINE "drum-machine"

#define CLAP_PLUGIN_FEATURE_FILTER "filter"
#define CLAP_PLUGIN_FEATURE_PHASER "phaser"
#define CLAP_PLUGIN_FEATURE_EQUALIZER "equalizer"
#define CLAP_PLUGIN_FEATURE_DEESSER "de-esser"
#define CLAP_PLUGIN_FEATURE_PHASE_VOCODER "phase-vocoder"
#define CLAP_PLUGIN_FEATURE_GRANULAR "granular"
#define CLAP_PLUGIN_FEATURE_FREQUENCY_SHIFTER "frequency-shifter"
#define CLAP_PLUGIN_FEATURE_PITCH_SHIFTER "pitch-shifter"

#define CLAP_PLUGIN_FEATURE_DISTORTION "distortion"
#define CLAP_PLUGIN_FEATURE_TRANSIENT_SHAPER "transient-shaper"
#define CLAP_PLUGIN_FEATURE_COMPRESSOR "compressor"
#define CLAP_PLUGIN_FEATURE_EXPANDER "expander"
#define CLAP_PLUGIN_FEATURE_GATE "gate"
#define CLAP_PLUGIN_FEATURE_LIMITER "limiter"

#define CLAP_PLUGIN_FEATURE_FLANGER "flanger"
#define CLAP_PLUGIN_FEATURE_CHORUS "chorus"
#define CLAP_PLUGIN_FEATURE_DELAY "delay"
#define CLAP_PLUGIN_FEATURE_REVERB "reverb"

#define CLAP_PLUGIN_FEATURE_TREMOLO "tremolo"
#define CLAP_PLUGIN_FEATURE_GLITCH "glitch"

#define CLAP_PLUGIN_FEATURE_UTILITY "utility"
#define CLAP_PLUGIN_FEATURE_PITCH_CORRECTION "pitch-correction"
#define CLAP_PLUGIN_FEATURE_RESTORATION "restoration" // repair the sound

#define CLAP_PLUGIN_FEATURE_MULTI_EFFECTS "multi-effects"

#define CLAP_PLUGIN_FEATURE_MIXING "mixing"
#define CLAP_PLUGIN_FEATURE_MASTERING "mastering"

////////////////////////
// Audio Capabilities //
////////////////////////

#define CLAP_PLUGIN_FEATURE_MONO "mono"
#define CLAP_PLUGIN_FEATURE_STEREO "stereo"
#define CLAP_PLUGIN_FEATURE_SURROUND "surround"
#define CLAP_PLUGIN_FEATURE_AMBISONIC "ambisonic"
