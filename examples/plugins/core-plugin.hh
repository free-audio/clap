#pragma once

#include <memory>

#include <clap-plugin.hh>

#include "parameters.hh"
#include "path-provider.hh"
#include "remote-gui.hh"

#include "../common/reducing-param-queue.hxx"

namespace clap {
   class CorePlugin : public Plugin {
   public:
      CorePlugin(std::unique_ptr<PathProvider> &&pathProvider,
                 const clap_plugin_descriptor *desc,
                 const clap_host *host);

      const PathProvider &pathProvider() const noexcept { return *_pathProvider; }

   protected:
      //-------------//
      // clap_plugin //
      //-------------//
      bool init() noexcept override;
      void initTrackInfo() noexcept;

      //------------------------//
      // clap_plugin_track_info //
      //------------------------//
      bool implementsTrackInfo() const noexcept override { return true; }
      void trackInfoChanged() noexcept override;

      //-------------------------//
      // clap_plugin_audio_ports //
      //-------------------------//
      bool implementsAudioPorts() const noexcept override;
      uint32_t audioPortsCount(bool is_input) const noexcept override;
      bool audioPortsInfo(uint32_t index,
                          bool is_input,
                          clap_audio_port_info *info) const noexcept override;
      uint32_t audioPortsConfigCount() const noexcept override;
      bool audioPortsGetConfig(uint32_t index,
                               clap_audio_ports_config *config) const noexcept override;
      bool audioPortsSetConfig(clap_id config_id) noexcept override;

      //--------------------//
      // clap_plugin_params //
      //--------------------//
      bool implementsParams() const noexcept override { return true; }

      uint32_t paramsCount() const noexcept override { return _parameters.count(); }

      bool paramsInfo(int32_t paramIndex, clap_param_info *info) const noexcept override {
         *info = _parameters.getByIndex(paramIndex)->info();
         return true;
      }

      virtual bool paramsValue(clap_id paramId, double *value) noexcept override {
         *value = _parameters.getById(paramId)->value();
         return true;
      }

      virtual bool paramsValueToText(clap_id paramId,
                                     double value,
                                     char *display,
                                     uint32_t size) noexcept override {
         // TODO
         return false;
      }

      virtual bool
      paramsTextToValue(clap_id param_id, const char *display, double *value) noexcept override {
         // TODO
         return false;
      }

      //-------------------//
      // clap_plugin_state //
      //-------------------//
      bool implementsState() const noexcept override { return true; }
      bool stateSave(clap_ostream *stream) noexcept override;
      bool stateLoad(clap_istream *stream) noexcept override;

      //-----------------//
      // clap_plugin_gui //
      //-----------------//
      bool implementsGui() const noexcept override { return true; }
      bool guiCreate() noexcept override;
      void guiDestroy() noexcept override;
      bool guiCanResize() const noexcept override { return false; }
      bool guiSize(uint32_t *width, uint32_t *height) noexcept override;
      void guiRoundSize(uint32_t *width, uint32_t *height) noexcept override {
         guiSize(width, height);
      }
      void guiSetScale(double scale) noexcept override;
      void guiShow() noexcept override;
      void guiHide() noexcept override;
      void guiDefineParameters();

      //---------------------//
      // clap_plugin_gui_x11 //
      //---------------------//
      bool implementsGuiX11() const noexcept override { return true; }
      bool guiX11Attach(const char *displayName, unsigned long window) noexcept override;

      //-----------------------//
      // clap_plugin_gui_win32 //
      //-----------------------//
      bool implementsGuiWin32() const noexcept override { return true; }
      bool guiWin32Attach(clap_hwnd window) noexcept override;

      //-----------------------//
      // clap_plugin_gui_cocoa //
      //-----------------------//
      bool implementsGuiCocoa() const noexcept override { return true; }
      bool guiCocoaAttach(void *nsView) noexcept override;

      //-------------------------------//
      // clap_plugin_gui_free_standing //
      //-------------------------------//
      bool implementsGuiFreeStanding() const noexcept override { return true; }
      bool guiFreeStandingOpen() noexcept override;

      //////////////////////
      // Cached Host Info //
      //////////////////////
      bool hasTrackInfo() const noexcept { return _hasTrackInfo; }
      const clap_track_info &trackInfo() const noexcept {
         assert(_hasTrackInfo);
         return _trackInfo;
      }
      uint32_t trackChannelCount() const noexcept {
         return _hasTrackInfo ? _trackInfo.channel_count : 2;
      }
      clap_chmap trackChannelMap() const noexcept {
         return _hasTrackInfo ? _trackInfo.channel_map : CLAP_CHMAP_STEREO;
      }

      //---------------------------//
      // clap_plugin_timer_support //
      //---------------------------//
      bool implementsTimerSupport() const noexcept override { return true; }
      void onTimer(clap_id timerId) noexcept override;

      //------------------------//
      // clap_plugin_fd_support //
      //------------------------//
      bool implementsFdSupport() const noexcept override { return true; }
      void onFd(clap_fd fd, uint32_t flags) noexcept override;

   protected:
      friend class RemoteGui;

      void guiAdjust(clap_id paramId, double value, clap_event_param_flags flags);
      void processGuiEvents(const clap_process *process);
      uint32_t
      processEvents(const clap_process *process, uint32_t& index, uint32_t count, uint32_t time);

      struct GuiToPluginValue {
         double value;
         clap_event_param_flags flags;
      };

      struct PluginToGuiValue {
         double value;
         double mod;
      };

      ReducingParamQueue<GuiToPluginValue> _guiToPluginQueue;
      ReducingParamQueue<PluginToGuiValue> _pluginToGuiQueue;

      std::unique_ptr<PathProvider> _pathProvider;

      bool _hasTrackInfo = false;
      clap_track_info _trackInfo;

      std::vector<clap_audio_port_info> _audioInputs;
      std::vector<clap_audio_port_info> _audioOutputs;
      std::vector<clap_audio_ports_config> _audioConfigs;

      std::unique_ptr<RemoteGui> _remoteGui;

      Parameters _parameters;

      static const constexpr uint32_t _paramSmoothingDuration = 64;
   };
} // namespace clap