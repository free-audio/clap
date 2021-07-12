#include <cstring>

#include "gain.hh"

namespace clap {
   const clap_plugin_descriptor *Gain::descriptor() {
      static const clap_plugin_descriptor desc = {

         CLAP_VERSION,
         "com.github.free-audio.clap.gain",
         "gain",
         "clap",
         "https://github.com/free-audio/clap",
         nullptr,
         nullptr,
         "0.1",
         "example gain plugin",
         "mix;gain",
         CLAP_PLUGIN_AUDIO_EFFECT

      };
      return &desc;
   }

   enum {
      kParamIdGain = 0,
   };

   Gain::Gain(const clap_host *host) : PluginHelper(descriptor(), host) {
      parameters_.addParameter(clap_param_info{
         .id = kParamIdGain,
         .name = "gain",
         .module = "/",
         .flags = 0,
         .min_value = -1,
         .max_value = 1,
         .default_value = 0,
      });
   }

   bool Gain::init() noexcept {
      if (!super::init())
         return false;

      defineAudioPorts();
      return true;
   }

   void Gain::defineAudioPorts() noexcept {
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

   void Gain::deactivate() noexcept { channelCount_ = 0; }

   clap_process_status Gain::process(const clap_process *process) noexcept {
      float **in = process->audio_inputs[0].data32;
      float **out = process->audio_outputs[0].data32;

      float k = 1;
      for (int i = 0; i < process->frames_count; ++i) {
         for (int c = 0; c < channelCount_; ++c)
            out[c][i] = k * in[c][i];
      }

      return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
   }
} // namespace clap