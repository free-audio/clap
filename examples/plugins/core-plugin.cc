#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <sstream>

#include "core-plugin.hh"
#include "stream-helper.hh"

namespace clap {

   CorePlugin::CorePlugin(std::unique_ptr<PathProvider> &&pathProvider,
                          const clap_plugin_descriptor *desc,
                          const clap_host *host)
      : Plugin(desc, host), _pathProvider(std::move(pathProvider)) {}

   bool CorePlugin::init() noexcept {
      initTrackInfo();
      return true;
   }

   void CorePlugin::initTrackInfo() noexcept {
      checkMainThread();

      assert(!_hasTrackInfo);
      if (!canUseTrackInfo())
         return;

      _hasTrackInfo = _hostTrackInfo->get(_host, &_trackInfo);
   }

   void CorePlugin::trackInfoChanged() noexcept {
      if (!_hostTrackInfo->get(_host, &_trackInfo)) {
         _hasTrackInfo = false;
         hostMisbehaving(
            "clap_host_track_info.get() failed after calling clap_plugin_track_info.changed()");
         return;
      }

      _hasTrackInfo = true;
   }

   bool CorePlugin::implementsAudioPorts() const noexcept { return true; }

   uint32_t CorePlugin::audioPortsCount(bool is_input) const noexcept {
      return is_input ? _audioInputs.size() : _audioOutputs.size();
   }

   bool CorePlugin::audioPortsInfo(uint32_t index,
                                   bool is_input,
                                   clap_audio_port_info *info) const noexcept {
      *info = is_input ? _audioInputs[index] : _audioOutputs[index];
      return true;
   }

   uint32_t CorePlugin::audioPortsConfigCount() const noexcept { return _audioConfigs.size(); }

   bool CorePlugin::audioPortsGetConfig(uint32_t index,
                                        clap_audio_ports_config *config) const noexcept {
      *config = _audioConfigs[index];
      return true;
   }

   bool CorePlugin::audioPortsSetConfig(clap_id config_id) noexcept { return false; }

   bool CorePlugin::stateSave(clap_ostream *stream) noexcept {
      try {
         OStream os(stream);
         boost::archive::text_oarchive ar(os);
         ar << _parameters;
      } catch (...) {
         return false;
      }
      return true;
   }

   bool CorePlugin::stateLoad(clap_istream *stream) noexcept {
      try {
         IStream is(stream);
         boost::archive::text_iarchive ar(is);
         ar >> _parameters;
      } catch (...) {
         return false;
      }
      return true;
   }

   bool CorePlugin::guiCreate() noexcept {
      _remoteGui.reset(new RemoteGui(*this));

      if (!_remoteGui->spawn()) {
         _remoteGui.reset();
         return false;
      }

      if (!_remoteGui)
         return false;

      guiDefineParameters();
      return true;
   }

   void CorePlugin::guiDefineParameters() {
      for (int i = 0; i < paramsCount(); ++i) {
         clap_param_info info;
         paramsInfo(i, &info);
         _remoteGui->defineParameter(info);
      }
   }

   void CorePlugin::guiDestroy() noexcept {
      if (_remoteGui)
         _remoteGui.reset();
   }

   bool CorePlugin::guiSize(uint32_t *width, uint32_t *height) noexcept {
      if (!_remoteGui)
         return false;

      return _remoteGui->size(width, height);
   }

   void CorePlugin::guiSetScale(double scale) noexcept {
      if (_remoteGui)
         _remoteGui->setScale(scale);
   }

   void CorePlugin::guiShow() noexcept {
      if (_remoteGui)
         _remoteGui->show();
   }

   void CorePlugin::guiHide() noexcept {
      if (_remoteGui)
         _remoteGui->hide();
   }

   void CorePlugin::onFd(clap_fd fd, uint32_t flags) noexcept {
      if (_remoteGui && fd == _remoteGui->fd())
         _remoteGui->onFd(flags);
   }

   void CorePlugin::onTimer(clap_id timerId) noexcept {
      if (_remoteGui && timerId == _remoteGui->timerId())
         _remoteGui->onTimer();
   }

   bool CorePlugin::guiX11Attach(const char *displayName, unsigned long window) noexcept {
      if (_remoteGui)
         return _remoteGui->attachX11(displayName, window);

      return false;
   }

   bool CorePlugin::guiWin32Attach(clap_hwnd window) noexcept {
      if (_remoteGui)
         return _remoteGui->attachWin32(window);

      return false;
   }

   bool CorePlugin::guiCocoaAttach(void *nsView) noexcept {
      if (_remoteGui)
         return _remoteGui->attachCocoa(nsView);

      return false;
   }

   bool CorePlugin::guiFreeStandingOpen() noexcept {
      // TODO
      return false;
   }

   void CorePlugin::guiAdjust(clap_id paramId, double value, clap_event_param_flags flags) {
      _guiToPluginQueue.push({paramId, value, flags});
   }

   void CorePlugin::processGuiEvents(const clap_process *process) {
      GuiToPluginValue value;
      while (_guiToPluginQueue.tryPop(value)) {
         auto p = _parameters.getById(value.paramId);
         if (!p)
            return;
         p->setValueSmoothed(value.value, std::max<int>(process->frames_count, 128));

         clap_event ev;
         ev.time = 0;
         ev.type = CLAP_EVENT_PARAM_VALUE;
         ev.param_value.param_id = value.paramId;
         ev.param_value.value = value.value;
         ev.param_value.channel = -1;
         ev.param_value.key = -1;
         ev.param_value.flags = value.flags;
         ev.param_value.cookie = p;

         process->out_events->push_back(process->out_events, &ev);
      }

      if (!_hasTransportCopy) {
         if (process->transport) {
            _hasTransport = true;
            memcpy(&_transportCopy, &process->transport, sizeof(_transportCopy));
         } else
            _hasTransport = false;

         _hasTransportCopy = true;
      }
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

               p->setValueSmoothed(ev->param_value.value, _paramSmoothingDuration);
               // p->setValueImmediately(ev->param_value.value);
               _pluginToGuiQueue.set(p->info().id, {ev->param_value.value, p->modulation()});
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

               p->setModulationSmoothed(ev->param_mod.amount, _paramSmoothingDuration);
               _pluginToGuiQueue.set(p->info().id, {p->value(), ev->param_mod.amount});
            }
            break;
         }
         }
      }

      return process->frames_count;
   }
} // namespace clap