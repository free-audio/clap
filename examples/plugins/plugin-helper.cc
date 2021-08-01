#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "plugin-helper.hh"
#include "stream-helper.hh"

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

   bool PluginHelper::stateSave(clap_ostream *stream) noexcept {
      try {
         OStream os(stream);
         boost::archive::text_oarchive ar(os);
         ar << parameters_;
      } catch (...) {
         return false;
      }
      return true;
   }

   bool PluginHelper::stateLoad(clap_istream *stream) noexcept {
      try {
         IStream is(stream);
         boost::archive::text_iarchive ar(is);
         ar >> parameters_;
      } catch (...) {
         return false;
      }
      return true;
   }

   bool PluginHelper::guiCreate() noexcept {
      remoteGui_.reset(new RemoteGui(*this));

      if (!remoteGui_->spawn())
      {
         remoteGui_.reset();
         return false;
      }

      return true;
   }

   void PluginHelper::guiDestroy() noexcept {
      if (remoteGui_)
         remoteGui_->destroy();
   }

   bool PluginHelper::guiSize(uint32_t *width, uint32_t *height) noexcept {
      if (!remoteGui_)
         return false;

      return remoteGui_->size(width, height);
   }

   void PluginHelper::guiSetScale(double scale) noexcept {
      if (remoteGui_)
         remoteGui_->setScale(scale);
   }

   void PluginHelper::guiShow() noexcept {
      if (remoteGui_)
         remoteGui_->show();
   }

   void PluginHelper::guiHide() noexcept {
      if (remoteGui_)
         remoteGui_->hide();
   }
} // namespace clap