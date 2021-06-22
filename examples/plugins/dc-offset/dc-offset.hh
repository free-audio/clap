#pragma once

#include "../plugin-helper.hh"

namespace clap {
   class DcOffset final : public PluginHelper {
   private:
      using super = PluginHelper;

   public:
      DcOffset(const clap_host *host);

      static const clap_plugin_descriptor *descriptor();

   protected:
      // clap_plugin
      bool                init() noexcept override;
      void                defineAudioPorts() noexcept;
      bool                activate(int sample_rate) noexcept override;
      void                deactivate() noexcept override;
      clap_process_status process(const clap_process *process) noexcept override;

   private:
      int channelCount_ = 1;
      Parameter *offsetParam_ = nullptr;
   };
} // namespace clap