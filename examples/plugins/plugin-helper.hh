#pragma once

#include <clap-plugin.hh>

#include "parameters.hh"
#include "remote-gui.hh"

namespace clap {
   class PluginHelper : public Plugin {
   public:
      PluginHelper(const clap_plugin_descriptor *desc, const clap_host *host);

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

      uint32_t paramsCount() const noexcept override { return parameters_.count(); }

      bool paramsInfo(int32_t paramIndex, clap_param_info *info) const noexcept override {
         *info = parameters_.getByIndex(paramIndex)->info();
         return true;
      }

      virtual bool paramsValue(clap_id paramId, double *value) noexcept override {
         *value = parameters_.getById(paramId)->value();
         return true;
      }

      virtual bool paramsValueToText(clap_id paramId,
                                     double value,
                                     char *display,
                                     uint32_t size) noexcept override {
         // TODO
         return false;
      }

      virtual bool paramsTextToValue(clap_id param_id,
                                     const char *display,
                                     double *value) noexcept override {
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

      //---------------------//
      // clap_plugin_gui_x11 //
      //---------------------//
      bool implementsGuiX11() const noexcept override { return false; }
      bool guiX11Attach(const char *displayName, unsigned long window) noexcept override {
         return false;
      }

      //-----------------------//
      // clap_plugin_gui_win32 //
      //-----------------------//
      bool implementsGuiWin32() const noexcept override { return false; }
      bool guiWin32Attach(clap_hwnd window) noexcept override { return false; }

      //-----------------------//
      // clap_plugin_gui_cocoa //
      //-----------------------//
      bool implementsGuiCocoa() const noexcept override { return false; }
      bool guiCocoaAttach(void *nsView) noexcept override { return false; }

      //-------------------------------//
      // clap_plugin_gui_free_standing //
      //-------------------------------//
      bool implementsGuiFreeStanding() const noexcept override { return false; }
      bool guiFreeStandingOpen() noexcept override { return false; }

      //////////////////////
      // Cached Host Info //
      //////////////////////
      bool hasTrackInfo() const noexcept { return hasTrackInfo_; }
      const clap_track_info &trackInfo() const noexcept {
         assert(hasTrackInfo_);
         return trackInfo_;
      }
      uint32_t trackChannelCount() const noexcept {
         return hasTrackInfo_ ? trackInfo_.channel_count : 2;
      }
      clap_chmap trackChannelMap() const noexcept {
         return hasTrackInfo_ ? trackInfo_.channel_map : CLAP_CHMAP_STEREO;
      }

   protected:
      friend class RemoteGui;

      bool hasTrackInfo_ = false;
      clap_track_info trackInfo_;

      std::vector<clap_audio_port_info> audioInputs_;
      std::vector<clap_audio_port_info> audioOutputs_;
      std::vector<clap_audio_ports_config> audioConfigs_;

      std::unique_ptr<RemoteGui> remoteGui_;

      Parameters parameters_;
   };
} // namespace clap