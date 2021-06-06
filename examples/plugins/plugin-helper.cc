#include "plugin-helper.hh"

namespace clap {

   PluginHelper::PluginHelper(const clap_plugin_descriptor *desc, const clap_host *host)
      : Plugin(desc, host) {}

   bool PluginHelper::init() noexcept {
      initTrackInfo();
      return true;
   }

   void PluginHelper::initTrackInfo() noexcept {
      checkMainThread();

      assert(!hasTrackInfo_);
      if (!canUseTrackInfo())
         return;

      hasTrackInfo_ = hostTrackInfo_->get(host_, &trackInfo_);
   }

   void PluginHelper::trackInfoChanged() noexcept {
      if (!hostTrackInfo_->get(host_, &trackInfo_)) {
         hasTrackInfo_ = false;
         hostMisbehaving(
            "clap_host_track_info.get() failed after calling clap_plugin_track_info.changed()");
         return;
      }

      hasTrackInfo_ = true;
   }

   bool PluginHelper::implementsAudioPorts() const noexcept { return true; }

   uint32_t PluginHelper::audioPortsCount(bool is_input) const noexcept {
      return is_input ? audioInputs_.size() : audioOutputs_.size();
   }

   bool PluginHelper::audioPortsInfo(uint32_t index,
                                     bool is_input,
                                     clap_audio_port_info *info) const noexcept {
      *info = is_input ? audioInputs_[index] : audioOutputs_[index];
      return true;
   }

   uint32_t PluginHelper::audioPortsConfigCount() const noexcept { return audioConfigs_.size(); }

   bool PluginHelper::audioPortsGetConfig(uint32_t index,
                                          clap_audio_ports_config *config) const noexcept {
      *config = audioConfigs_[index];
      return true;
   }

   bool PluginHelper::audioPortsSetConfig(clap_id config_id) noexcept { return false; }
} // namespace clap