#include "gain.hh"

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

Gain::Gain(clap_host *host) : Plugin(descriptor(), host) {}

bool Gain::init() {
   if (canUseTrackInfo()) {
      clap_track_info info;
      if (hostTrackInfo_->get_track_info(host_, &info))
      {
         channelCount_ = info.channel_count;
         channelMap_ = info.channel_map;
      }
   }

   return true;
}

clap_process_status Gain::process(const clap_process *process)
{
   auto **in = process->audio_inputs[0].data32;
   auto **out = process->audio_outputs[0].data32;

   float k = 1;

   for (int i = 0; i < process->frames_count; ++i)
   {
      for (int j = 0; j < channelCount_; ++j)
         out[j][i] = k * in[j][i];
   }

   return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
}