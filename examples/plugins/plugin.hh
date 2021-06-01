#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

#include <clap/all.h>

#include "parameters.hh"

namespace clap {
   class Plugin {
   public:
      const clap_plugin *clapPlugin() noexcept { return &plugin_; }

   protected:
      Plugin(const clap_plugin_descriptor *desc, const clap_host *host);
      virtual ~Plugin() = default;

      // not copyable, not moveable
      Plugin(const Plugin &) = delete;
      Plugin(Plugin &&)      = delete;
      Plugin &operator=(const Plugin &) = delete;
      Plugin &operator=(Plugin &&) = delete;

      /////////////////////////
      // Methods to override //
      /////////////////////////

      // clap_plugin
      virtual bool                init() { return true; }
      virtual bool                activate(int sampleRate) { return true; }
      virtual void                deactivate() {}
      virtual bool                startProcessing() { return true; }
      virtual void                stopProcessing() {}
      virtual clap_process_status process(const clap_process *process) {
         return CLAP_PROCESS_SLEEP;
      }
      virtual const void *extension(const char *id) { return nullptr; }

      // clap_plugin_track_info
      virtual void trackInfoChanged() {}

      // clap_plugin_audio_ports
      virtual bool     implementsAudioPorts() const noexcept { return false; }
      virtual uint32_t audioPortsCount(bool is_input) { return 0; }
      virtual bool     audioPortsInfo(uint32_t index, bool is_input, clap_audio_port_info *info) {
         return false;
      }
      virtual uint32_t audioPortsConfigCount() { return 0; }
      virtual bool     audioPortsGetConfig(uint32_t index, clap_audio_ports_config *config) {
         return false;
      }
      virtual bool audioPortsSetConfig(clap_id config_id) { return false; }

      // clap_plugin_params
      virtual bool     implementsParams() const noexcept { return false; }
      virtual uint32_t paramsCount() const { return 0; }
      virtual bool     paramsInfo(int32_t paramIndex, clap_param_info *info) const { return false; }
      virtual bool paramsEnumValue(clap_id paramId, int32_t valueIndex, clap_param_value *value) {
         return false;
      }
      virtual bool paramsValue(clap_id paramId, clap_param_value *value) { return false; }
      virtual bool
      paramsSetValue(clap_id paramId, clap_param_value value, clap_param_value modulation) { return false; }
      virtual bool
      paramsValueToText(clap_id paramId, clap_param_value value, char *display, uint32_t size) {
         return false;
      }
      virtual bool
      paramsTextToValue(clap_id param_id, const char *display, clap_param_value *value) {
         return false;
      }

      //////////////////
      // Invalidation //
      //////////////////
      void invalidateAudioPortsDefinition();

      /////////////
      // Logging //
      /////////////
      void log(clap_log_severity severity, const char *msg) const;
      void hostMisbehaving(const char *msg);
      void hostMisbehaving(const std::string &msg) { hostMisbehaving(msg.c_str()); }

      /////////////////////////////////
      // Interface consistency check //
      /////////////////////////////////
      bool canUseHostLog() const noexcept;
      bool canUseThreadCheck() const noexcept;
      bool canUseTrackInfo() const noexcept;
      bool canChangeAudioPorts() const noexcept;

      /////////////////////
      // Thread Checking //
      /////////////////////
      void checkMainThread() const;
      void ensureMainThread(const char *method);
      void ensureAudioThread(const char *method);

      ///////////////
      // Utilities //
      ///////////////
      static Plugin &from(const clap_plugin *plugin);

      template <typename T>
      void initInterface(const T *&ptr, const char *id);
      void initInterfaces();

      static uint32_t compareAudioPortsInfo(const clap_audio_port_info &a,
                                            const clap_audio_port_info &b) noexcept;

      virtual bool isValidParamId(clap_id param_id) const noexcept;

      //////////////////////
      // Processing State //
      //////////////////////
      bool isActive() const noexcept { return isActive_; }
      bool isProcessing() const noexcept { return isProcessing_; }
      int  sampleRate() const noexcept;

      //////////////////////
      // Cached Host Info //
      //////////////////////
      bool                   hasTrackInfo() const noexcept { return hasTrackInfo_; }
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
      clap_plugin_event_filter pluginEventFilter_;
      clap_plugin_latency      pluginLatency_;
      clap_plugin_render       pluginRender_;
      clap_plugin_note_name    pluginNoteName_;
      clap_plugin_thread_pool  pluginThreadPool_;

      /* state related */
      clap_plugin_state          pluginState_;
      clap_plugin_preset_load    pluginPresetLoad_;
      clap_plugin_file_reference pluginFileReference_;

      /* GUI related */
      clap_plugin_gui        pluginGui_;
      clap_plugin_gui_win32  pluginGuiWin32_;
      clap_plugin_gui_cocoa  pluginGuiCocoa_;
      clap_plugin_gui_x11    pluginGuiX11_;
      clap_plugin_event_loop pluginEventLoop_;

      const clap_host *const          host_              = nullptr;
      const clap_host_log *           hostLog_           = nullptr;
      const clap_host_thread_check *  hostThreadCheck_   = nullptr;
      const clap_host_thread_pool *   hostThreadPool_    = nullptr;
      const clap_host_audio_ports *   hostAudioPorts_    = nullptr;
      const clap_host_event_filter *  hostEventFilter_   = nullptr;
      const clap_host_file_reference *hostFileReference_ = nullptr;
      const clap_host_latency *       hostLatency_       = nullptr;
      const clap_host_gui *           hostGui_           = nullptr;
      const clap_host_event_loop *    hostEventLoop_     = nullptr;
      const clap_host_params *        hostParams_        = nullptr;
      const clap_host_track_info *    hostTrackInfo_     = nullptr;
      const clap_host_state *         hostState_         = nullptr;
      const clap_host_note_name *     hostNoteName_      = nullptr;

   private:
      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      clap_plugin plugin_;
      // clap_plugin
      static bool                clapInit(const clap_plugin *plugin);
      static void                clapDestroy(const clap_plugin *plugin);
      static bool                clapActivate(const clap_plugin *plugin, int sample_rate);
      static void                clapDeactivate(const clap_plugin *plugin);
      static bool                clapStartProcessing(const clap_plugin *plugin);
      static void                clapStopProcessing(const clap_plugin *plugin);
      static clap_process_status clapProcess(const clap_plugin * plugin,
                                             const clap_process *process);
      static const void *        clapExtension(const clap_plugin *plugin, const char *id);

      // clap_plugin_track_info
      static void clapTrackInfoChanged(const clap_plugin *plugin);
      void        initTrackInfo();

      // clap_plugin_audio_ports
      static uint32_t clapAudioPortsCount(const clap_plugin *plugin, bool is_input);
      static bool     clapAudioPortsInfo(const clap_plugin *   plugin,
                                         uint32_t              index,
                                         bool                  is_input,
                                         clap_audio_port_info *info);
      static uint32_t clapAudioPortsConfigCount(const clap_plugin *plugin);
      static bool     clapAudioPortsGetConfig(const clap_plugin *      plugin,
                                              uint32_t                 index,
                                              clap_audio_ports_config *config);
      static bool     clapAudioPortsSetConfig(const clap_plugin *plugin, clap_id config_id);

      // clap_plugin_params
      static uint32_t clapParamsCount(const clap_plugin *plugin);
      static bool
      clapParamsIinfo(const clap_plugin *plugin, int32_t param_index, clap_param_info *param_info);
      static bool clapParamsEnumValue(const clap_plugin *plugin,
                                      clap_id            param_id,
                                      int32_t            value_index,
                                      clap_param_value * value);
      static bool
      clapParamsValue(const clap_plugin *plugin, clap_id param_id, clap_param_value *value);
      static bool clapParamsSetValue(const clap_plugin *plugin,
                                     clap_id            param_id,
                                     clap_param_value   value,
                                     clap_param_value   modulation);
      static bool clapParamsValueToText(const clap_plugin *plugin,
                                        clap_id            param_id,
                                        clap_param_value   value,
                                        char *             display,
                                        uint32_t           size);
      static bool clapParamsTextToValue(const clap_plugin *plugin,
                                        clap_id            param_id,
                                        const char *       display,
                                        clap_param_value * value);

      // interfaces
      static const constexpr clap_plugin_track_info  pluginTrackInfo_  = {clapTrackInfoChanged};
      static const constexpr clap_plugin_audio_ports pluginAudioPorts_ = {clapAudioPortsCount,
                                                                          clapAudioPortsInfo,
                                                                          clapAudioPortsConfigCount,
                                                                          clapAudioPortsGetConfig,
                                                                          clapAudioPortsSetConfig};
      static const constexpr clap_plugin_params      pluginParams_     = {clapParamsCount,
                                                                 clapParamsIinfo,
                                                                 clapParamsEnumValue,
                                                                 clapParamsValue,
                                                                 clapParamsSetValue,
                                                                 clapParamsValueToText,
                                                                 clapParamsTextToValue};

      // state
      bool isActive_     = false;
      bool isProcessing_ = false;
      int  sampleRate_   = 0;

      bool            hasTrackInfo_ = false;
      clap_track_info trackInfo_;
   };
} // namespace clap