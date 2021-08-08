#pragma once

#include "../core-plugin.hh"

namespace clap {
   class Gain final : public CorePlugin {
   private:
      using super = CorePlugin;

   public:
      Gain(const std::string& pluginPath, const clap_host *host);

      static const clap_plugin_descriptor *descriptor();

   protected:
      // clap_plugin
      bool                init() noexcept override;
      void                defineAudioPorts() noexcept;
      void                deactivate() noexcept override;
      clap_process_status process(const clap_process *process) noexcept override;

   private:
      int channelCount_ = 1;
   };
} // namespace clap