#pragma once

#include <clap/all.h>

#include "../plugin.hh"

class Gain : public Plugin {
public:
   Gain(clap_host *host);

   static const clap_plugin_descriptor *descriptor();

protected:
   bool                init() override;
   bool                activate(int sample_rate) override;
   void                deactivate() override;
   clap_process_status process(const clap_process *process) override;

   void trackInfoChanged() override;
   void updateChannelCount(bool shouldNotifyHost);

private:
   int channelCount_ = 2;
   bool schedulePortUpdate_ = false;
};