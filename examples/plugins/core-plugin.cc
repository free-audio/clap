#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <sstream>

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
         remoteGui_.reset();
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

   void CorePlugin::onFd(clap_fd fd, uint32_t flags) noexcept {
      if (remoteGui_ && fd == remoteGui_->fd())
         remoteGui_->onFd(flags);
   }

   void CorePlugin::onTimer(clap_id timerId) noexcept {
      if (remoteGui_ && timerId == remoteGui_->timerId())
         remoteGui_->onTimer();
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
         return remoteGui_->attachCocoa(nsView);

      return false;
   }

   bool CorePlugin::guiFreeStandingOpen() noexcept {
      // TODO
      return false;
   }

   void CorePlugin::guiAdjust(clap_id paramId, double value, clap_event_param_flags flags) {
      guiToPluginQueue_.set(paramId, {value, flags});
      guiToPluginQueue_.producerDone();
   }

   void CorePlugin::processGuiEvents(const clap_process *process) {
      guiToPluginQueue_.consume([this, process](clap_id paramId, const GuiToPluginValue &value) {
         auto p = parameters_.getById(paramId);
         if (!p)
            return;
         p->setValueSmoothed(value.value, 128);

         clap_event ev;
         ev.time = 0;
         ev.type = CLAP_EVENT_PARAM_VALUE;
         ev.param_value.param_id = paramId;
         ev.param_value.value = value.value;
         ev.param_value.channel = -1;
         ev.param_value.key = -1;
         ev.param_value.flags = value.flags;
         ev.param_value.cookie = p;

         process->out_events->push_back(process->out_events, &ev);
      });
   }

   uint32_t CorePlugin::processEvents(const clap_process *process,
                                      uint32_t &index,
                                      uint32_t count,
                                      uint32_t time) {
      for (; index < count; ++index) {
         auto ev = process->in_events->get(process->in_events, index);

         if (ev->time < time) {
            hostMisbehaving("Events must be ordered by time");
            std::terminate();
         }

         if (ev->time > time) {
            // This event is in the future
            return std::min(ev->time, process->frames_count);
         }

         switch (ev->type) {
         case CLAP_EVENT_PARAM_VALUE: {
            auto p = reinterpret_cast<Parameter *>(ev->param_value.cookie);
            if (p) {
               if (p->info().id != ev->param_value.param_id) {
                  std::ostringstream os;
                  os << "Host provided invalid cookie for param id: " << ev->param_value.param_id;
                  hostMisbehaving(os.str());
                  std::terminate();
               }

               p->setValueSmoothed(ev->param_value.value, paramSmoothingDuration_);
               //p->setValueImmediately(ev->param_value.value);
               pluginToGuiQueue_.set(p->info().id, {ev->param_value.value, p->modulation()});
            }
            break;
         }

         case CLAP_EVENT_PARAM_MOD: {
            auto p = reinterpret_cast<Parameter *>(ev->param_mod.cookie);
            if (p) {
               if (p->info().id != ev->param_mod.param_id) {
                  std::ostringstream os;
                  os << "Host provided invalid cookie for param id: " << ev->param_mod.param_id;
                  hostMisbehaving(os.str());
                  std::terminate();
               }

               p->setModulationSmoothed(ev->param_mod.amount, paramSmoothingDuration_);
               pluginToGuiQueue_.set(p->info().id, {p->value(), ev->param_mod.amount});
            }
            break;
         }
         }
      }

      return process->frames_count;
   }
} // namespace clap