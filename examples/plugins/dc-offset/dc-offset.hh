#pragma once

#include "../core-plugin.hh"

namespace clap {
   class DcOffset final : public CorePlugin {
   private:
      using super = CorePlugin;

   public:
      DcOffset(const std::string& pluginPath, const clap_host *host);

      static const clap_plugin_descriptor *descriptor();

   protected:
      bool init() noexcept override;
      void defineAudioPorts() noexcept;
      clap_process_status process(const clap_process *process) noexcept override;

   private:
      int _channelCount = 2;
      Parameter *_offsetParam = nullptr;
   };
} // namespace clap