#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "core-plugin.hh"
#include "stream-helper.hh"

namespace clap {

   CorePlugin::CorePlugin(std::unique_ptr<PathProvider> &&pathProvider,
                          const clap_plugin_descriptor *desc,
                          const clap_host *host)
      : Plugin(desc, host), pathProvider_(std::move(pathProvider)) {}

   bool CorePlugin::init() noexcept {
      initTrackInfo();
      return true;
   }

   void CorePlugin::initTrackInfo() noexcept {
      checkMainThread();

      assert(!hasTrackInfo_);
      if (!canUseTrackInfo())
         return;

      hasTrackInfo_ = hostTrackInfo_->get(host_, &trackInfo_);
   }

   void CorePlugin::trackInfoChanged() noexcept {
      if (!hostTrackInfo_->get(host_, &trackInfo_)) {
         hasTrackInfo_ = false;
         hostMisbehaving(
            "clap_host_track_info.get() failed after calling clap_plugin_track_info.changed()");
         return;
      }

      hasTrackInfo_ = true;
   }

   bool CorePlugin::implementsAudioPorts() const noexcept { return true; }

   uint32_t CorePlugin::audioPortsCount(bool is_input) const noexcept {
      return is_input ? audioInputs_.size() : audioOutputs_.size();
   }

   bool CorePlugin::audioPortsInfo(uint32_t index,
                                   bool is_input,
                                   clap_audio_port_info *info) const noexcept {
      *info = is_input ? audioInputs_[index] : audioOutputs_[index];
      return true;
   }

   uint32_t CorePlugin::audioPortsConfigCount() const noexcept { return audioConfigs_.size(); }

   bool CorePlugin::audioPortsGetConfig(uint32_t index,
                                        clap_audio_ports_config *config) const noexcept {
      *config = audioConfigs_[index];
      return true;
   }

   bool CorePlugin::audioPortsSetConfig(clap_id config_id) noexcept { return false; }

   bool CorePlugin::stateSave(clap_ostream *stream) noexcept {
      try {
         OStream os(stream);
         boost::archive::text_oarchive ar(os);
         ar << parameters_;
      } catch (...) {
         return false;
      }
      return true;
   }

   bool CorePlugin::stateLoad(clap_istream *stream) noexcept {
      try {
         IStream is(stream);
         boost::archive::text_iarchive ar(is);
         ar >> parameters_;
      } catch (...) {
         return false;
      }
      return true;
   }

   bool CorePlugin::guiCreate() noexcept {
      remoteGui_.reset(new RemoteGui(*this));

      if (!remoteGui_->spawn()) {
         remoteGui_.reset();
         return false;
      }

      if (!remoteGui_)
         return false;

      guiDefineParameters();
      return true;
   }

   void CorePlugin::guiDefineParameters() {
      for (int i = 0; i < paramsCount(); ++i) {
         clap_param_info info;
         paramsInfo(i, &info);
         remoteGui_->defineParameter(info);
      }
   }

   void CorePlugin::guiDestroy() noexcept {
      if (remoteGui_)
         remoteGui_->destroy();
   }

   bool CorePlugin::guiSize(uint32_t *width, uint32_t *height) noexcept {
      if (!remoteGui_)
         return false;

      return remoteGui_->size(width, height);
   }

   void CorePlugin::guiSetScale(double scale) noexcept {
      if (remoteGui_)
         remoteGui_->setScale(scale);
   }

   void CorePlugin::guiShow() noexcept {
      if (remoteGui_)
         remoteGui_->show();
   }

   void CorePlugin::guiHide() noexcept {
      if (remoteGui_)
         remoteGui_->hide();
   }

   void CorePlugin::eventLoopOnFd(clap_fd fd, uint32_t flags) noexcept {
      if (remoteGui_ && fd == remoteGui_->fd())
         remoteGui_->onFd(flags);
   }

   bool CorePlugin::guiX11Attach(const char *displayName, unsigned long window) noexcept {
      if (remoteGui_)
         return remoteGui_->attachX11(displayName, window);

      return false;
   }

   bool CorePlugin::guiWin32Attach(clap_hwnd window) noexcept {
      if (remoteGui_)
         return remoteGui_->attachWin32(window);

      return false;
   }

   bool CorePlugin::guiCocoaAttach(void *nsView) noexcept {
      if (remoteGui_)
         return remoteGui_->attachWin32(nsView);

      return false;
   }

   bool CorePlugin::guiFreeStandingOpen() noexcept {
      // TODO
      return false;
   }
} // namespace clap