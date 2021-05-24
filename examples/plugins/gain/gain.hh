#pragma once

#include <clap/all.h>

#include "../plugin.hh"

namespace clap {
   class Gain final : public Plugin {
   public:
      Gain(const clap_host *host);

      static const clap_plugin_descriptor *descriptor();

   protected:
      bool                activate(int sample_rate) override;
      void                deactivate() override;
      clap_process_status process(const clap_process *process) override;

      void defineAudioPorts(std::vector<clap_audio_port_info> &inputPorts,
                            std::vector<clap_audio_port_info> &outputPorts) override;
      bool shouldInvalidateAudioPortsDefinitionOnTrackChannelChange() const override {
         return true;
      }

   private:
      int channelCount_ = 0;
   };
} // namespace clap