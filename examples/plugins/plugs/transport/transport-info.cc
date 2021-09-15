#include <cstring>

#include "../../parameter-interpolator.hh"
#include "transport-info.hh"

namespace clap {
   const clap_plugin_descriptor *TransportInfo::descriptor() {
      static const clap_plugin_descriptor desc = {

         CLAP_VERSION,
         "com.github.free-audio.clap.transport-info",
         "Transport Info",
         "clap",
         "https://github.com/free-audio/clap",
         nullptr,
         nullptr,
         "0.1",
         "Displays transport info",
         "utility",
         CLAP_PLUGIN_AUDIO_EFFECT

      };
      return &desc;
   }

   enum {
      kParamIdOffset = 0,
   };

   TransportInfo::TransportInfo(const std::string &pluginPath, const clap_host *host)
      : CorePlugin(PathProvider::create(pluginPath, "transport-info"), descriptor(), host) {}

   clap_process_status TransportInfo::process(const clap_process *process) noexcept {
      processGuiEvents(process);

      _pluginToGuiQueue.producerDone();

      return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
   }
} // namespace clap