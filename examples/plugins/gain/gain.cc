#include "gain.hh"

const clap_plugin_descriptor g_gain_plugin_descriptor = {
    CLAP_VERSION,
    "com.github.free-audio.clap.gain",
    "gain",
    "clap",
    "https://github.com/free-audio/clap",
    nullptr,
    nullptr,
    "0.1",
    "example gain plugin",
    "mix;gain",
    CLAP_PLUGIN_AUDIO_EFFECT
};