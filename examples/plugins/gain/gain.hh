#pragma once

#include <clap/all.h>

#include "../plugin.hh"

class Gain : public Plugin
{
public:
    Gain(clap_host *host);
};

extern const clap_plugin_descriptor g_gain_plugin_descriptor;