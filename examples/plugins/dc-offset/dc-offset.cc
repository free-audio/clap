#include <cstring>

#include "../parameter-interpolator.hh"
#include "dc-offset.hh"

namespace clap {
   const clap_plugin_descriptor *DcOffset::descriptor() {
      static const clap_plugin_descriptor desc = {

         CLAP_VERSION,
         "com.github.free-audio.clap.dc-offset",
         "dc offset",
         "clap",
         "https://github.com/free-audio/clap",
         nullptr,
         nullptr,
         "0.1",
         "example DC offset plugin",
         "utility",
         CLAP_PLUGIN_AUDIO_EFFECT

      };
      return &desc;
   }

   enum {
      kParamIdOffset = 0,
   };

   DcOffset::DcOffset(const std::string &pluginPath, const clap_host *host)
      : CorePlugin(PathProvider::create(pluginPath, "dc-offset"), descriptor(), host) {
      parameters_.addParameter(clap_param_info{
         kParamIdOffset,
         0,
         nullptr,
         "offset",
         "/",
         -1,
         1,
         0,
      });

      offsetParam_ = parameters_.getById(kParamIdOffset);
   }

   bool DcOffset::init() noexcept {
      if (!super::init())
         return false;

      defineAudioPorts();
      return true;
   }

   void DcOffset::defineAudioPorts() noexcept {
      assert(!isActive());

      channelCount_ = trackChannelCount();

      clap_audio_port_info info;
      info.id = 0;
      strncpy(info.name, "main", sizeof(info.name));
      info.is_main = true;
      info.is_cv = false;
      info.sample_size = 32;
      info.in_place = true;
      info.channel_count = channelCount_;
      info.channel_map = CLAP_CHMAP_UNSPECIFIED;

      audioInputs_.clear();
      audioInputs_.push_back(info);
      audioOutputs_.clear();
      audioOutputs_.push_back(info);
   }

   clap_process_status DcOffset::process(const clap_process *process) noexcept {
      float **in = process->audio_inputs[0].data32;
      float **out = process->audio_outputs[0].data32;
      uint32_t evCount = process->in_events->size(process->in_events);
      uint32_t nextEvIndex = 0;
      uint32_t N = process->frames_count;

      processGuiEvents(process);

      /* foreach frames */
      for (uint32_t i = 0; i < process->frames_count;) {

         N = processEvents(process, nextEvIndex, evCount, i);

         /* Process as many samples as possible until the next event */
         for (; i < N; ++i) {
            const float offset = offsetParam_->step();
            for (int c = 0; c < channelCount_; ++c)
               out[c][i] = in[c][i] + offset;
         }
      }

      pluginToGuiQueue_.producerDone();

      return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
   }
} // namespace clap