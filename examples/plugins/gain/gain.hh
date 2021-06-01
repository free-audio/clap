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

      uint32_t audioPortsCount(bool is_input) override { return 2; }
      bool     audioPortsInfo(uint32_t index, bool is_input, clap_audio_port_info *info) override;

   private:
      int channelCount_ = 0;

      Parameters parameters_;
   };
} // namespace clap