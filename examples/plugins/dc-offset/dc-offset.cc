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

   DcOffset::DcOffset(const std::string& pluginPath, const clap_host *host) : CorePlugin(PathProvider::create(pluginPath, "dc-offset"), descriptor(), host) {
      parameters_.addParameter(clap_param_info{
         .id = kParamIdOffset,
         .flags = 0,
         .name = "offset",
         .module = "/",
         .min_value = -1,
         .max_value = 1,
         .default_value = 0,
      });
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
      const clap_event *ev = nullptr;
      uint32_t N = process->frames_count;

      /* foreach frames */
      for (uint32_t i = 0; i < process->frames_count; ++i) {

         /* check if there are events to process */
         for (; nextEvIndex < evCount; ++nextEvIndex) {
            ev = process->in_events->get(process->in_events, nextEvIndex);

            if (ev->time < i) {
               hostMisbehaving("Events must be ordered by time");
               std::terminate();
            }

            if (ev->time > i) {
               // This event is in the future
               N = std::min(ev->time, process->frames_count);
               break;
            }

            switch (ev->type) {
            case CLAP_EVENT_PARAM_VALUE: {
               auto p = parameters_.getById(ev->param_value.param_id);
               if (p)
                  p->setValue(ev->param_value.value);
               break;
            }

            case CLAP_EVENT_PARAM_MOD: {
               auto p = parameters_.getById(ev->param_mod.param_id);
               if (p)
                  p->setModulation(ev->param_mod.amount);
               break;
            }
            }
         }

         /* Process as many samples as possible until the next event */
         for (; i < N; ++i) {
            float offset = offsetParam_->value();
            for (int c = 0; c < channelCount_; ++c)
               out[c][i] = in[c][i] + offset;
            offsetParam_->step(1);
         }
      }

      return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
   }
} // namespace clap