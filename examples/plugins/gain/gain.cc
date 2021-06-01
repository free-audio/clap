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

   Gain::Gain(const clap_host *host) : Plugin(descriptor(), host) {
      parameters_.addParameter(Parameter{
         .info =
            clap_param_info{
               .id               = kParamIdGain,
               .name             = "gain",
               .module           = "/",
               .is_per_note      = false,
               .is_per_channel   = false,
               .is_used          = true,
               .is_periodic      = false,
               .is_locked        = false,
               .is_automatable   = true,
               .is_hidden        = false,
               .is_bypass        = false,
               .type             = CLAP_PARAM_FLOAT,
               .min_value        = {.d = -120},
               .max_value        = {.d = 20},
               .default_value    = {.d = 0},
               .enum_entry_count = 0,
            },
         .enumDefinition = {},
         .value          = {.d = 0},
         .modulation     = {.d = 0},
      });
   }

   bool Gain::activate(int sample_rate) {
      channelCount_ = trackChannelCount();
      return true;
   }

   void Gain::deactivate() { channelCount_ = 0; }

   clap_process_status Gain::process(const clap_process *process) {
      float **in  = process->audio_inputs[0].data32;
      float **out = process->audio_outputs[0].data32;

      float k = 1;
      for (int i = 0; i < process->frames_count; ++i) {
         for (int c = 0; c < channelCount_; ++c)
            out[c][i] = k * in[c][i];
      }

      return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
   }

   bool Gain::audioPortsInfo(uint32_t index, bool is_input, clap_audio_port_info *info) {
      assert(index == 0);

      info->id = 0;
      strncpy(info->name, "main", sizeof(info->name));
      info->is_main       = true;
      info->is_cv         = false;
      info->sample_size   = 32;
      info->in_place      = true;
      info->channel_count = channelCount_;
      info->channel_map   = CLAP_CHMAP_UNSPECIFIED;

      return true;
   }
} // namespace clap