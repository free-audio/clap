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
      bool init() noexcept override;
      void defineAudioPorts() noexcept;
      clap_process_status process(const clap_process *process) noexcept override;

   private:
      int channelCount_ = 2;
      Parameter *offsetParam_ = nullptr;
   };
} // namespace clap