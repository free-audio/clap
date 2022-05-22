#pragma once

/////////////////////
// Plugin category //
/////////////////////

// Add this feature if your plugin can process note events and then produce audio
static const char *CLAP_PLUGIN_FEATURE_INSTRUMENT = "instrument";

// Add this feature if your plugin is an audio effect
static const char *CLAP_PLUGIN_FEATURE_AUDIO_EFFECT = "audio effect";

// Add this feature if your plugin is a note effect or a note generator/sequencer
static const char *CLAP_PLUGIN_FEATURE_NOTE_EFFECT = "note effect";

// Add this feature if your plugin is an analyzer
static const char *CLAP_PLUGIN_FEATURE_ANALYZER = "analyzer";

