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
   updateChannelCount(false);
   return true;
}

void Gain::updateChannelCount(bool shouldNotifyHost) {
   if (!canUseTrackInfo())
      return;

   clap_track_info info;
   if (!hostTrackInfo_->get(host_, &info))
      return;

   if (channelCount_ == info.channel_count)
      return;

   if (isActive_)
      // we can't change our ports now, delay it
      schedulePortUpdate_ = true;
   else
      channelCount_ = info.channel_count;

   if (canChangeAudioPorts())
      hostAudioPorts_->changed(host_);
}

bool Gain::activate(int sample_rate) { return true; }

void Gain::deactivate() {
   if (schedulePortUpdate_) {
      schedulePortUpdate_ = false;
      updateChannelCount(true);
   }
}

void Gain::trackInfoChanged() { updateChannelCount(true); }

clap_process_status Gain::process(const clap_process *process) {
   float *in = process->audio_inputs[0].data32i;
   float *out = process->audio_outputs[0].data32i;

   float k = 1;
   const auto N = process->frames_count * channelCount_;
   for (int i = 0; i < N; ++i) {
      out[i] = k * in[i];
   }

   return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
}
