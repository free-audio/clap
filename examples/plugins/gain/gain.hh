#pragma once

#include <clap/all.h>

#include "../plugin.hh"

class Gain : public Plugin {
public:
   Gain(clap_host *host);

   static const clap_plugin_descriptor *descriptor();

   bool init() override;
   clap_process_status process(const clap_process *process) override;

private:
   int channelCount_ = 2;
   int channelMap_ = CLAP_CHMAP_STEREO;
};