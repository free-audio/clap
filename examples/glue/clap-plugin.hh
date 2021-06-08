#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

#include <clap/all.h>

namespace clap {
   /// @brief C++ glue and checks
   ///
   /// @note for an higher level implementation, see @ref PluginHelper
   class Plugin {
   public:
      const clap_plugin *clapPlugin() noexcept { return &plugin_; }

   protected:
      Plugin(const clap_plugin_descriptor *desc, const clap_host *host);
      virtual ~Plugin();

      // not copyable, not moveable
      Plugin(const Plugin &) = delete;
      Plugin(Plugin &&) = delete;
      Plugin &operator=(const Plugin &) = delete;
      Plugin &operator=(Plugin &&) = delete;

      /////////////////////////
      // Methods to override //
      /////////////////////////

      //-------------//
      // clap_plugin //
      //-------------//
      virtual bool init() noexcept { return true; }
      virtual bool activate(int sampleRate) noexcept { return true; }
      virtual void deactivate() noexcept {}
      virtual bool startProcessing() noexcept { return true; }
      virtual void stopProcessing() noexcept {}
      virtual clap_process_status process(const clap_process *process) noexcept {
         return CLAP_PROCESS_SLEEP;
      }
      virtual const void *extension(const char *id) noexcept { return nullptr; }

      //---------------------//
      // clap_plugin_latency //
      //---------------------//
      virtual bool implementsLatency() const noexcept { return false; }
      virtual uint32_t latencyGet() const noexcept { return 0; }

      //--------------------//
      // clap_plugin_render //
      //--------------------//
      virtual bool implementsRender() const noexcept { return false; }
      virtual void renderSetMode(clap_plugin_render_mode mode) noexcept {}

      //-------------------------//
      // clap_plugin_thread_pool //
      //-------------------------//
      virtual bool implementsThreadPool() const noexcept { return false; }
      virtual void threadPoolExec(uint32_t taskIndex) noexcept {}

      //-------------------//
      // clap_plugin_state //
      //-------------------//
      virtual bool implementsState() const noexcept { return false; }
      virtual bool stateSave(clap_ostream *stream) noexcept { return false; }
      virtual bool stateLoad(clap_istream *stream) noexcept { return false; }
      virtual bool stateIsDirty() const noexcept { return false; }
      void stateMarkDirty() const noexcept {
         if (canUseState())
            hostState_->mark_dirty(host_);
      }

      //-------------------------//
      // clap_plugin_preset_load //
      //-------------------------//
      virtual bool implementsPresetLoad() const noexcept { return false; }
      virtual bool presetLoadFromFile(const char *path) noexcept { return false; }

      //------------------------//
      // clap_plugin_track_info //
      //------------------------//
      virtual bool implementsTrackInfo() const noexcept { return false; }
      virtual void trackInfoChanged() noexcept {}

      //-------------------------//
      // clap_plugin_audio_ports //
      //-------------------------//
      virtual bool implementsAudioPorts() const noexcept { return false; }
      virtual uint32_t audioPortsCount(bool isInput) const noexcept { return 0; }
      virtual bool
      audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info *info) const noexcept {
         return false;
      }
      virtual uint32_t audioPortsConfigCount() const noexcept { return 0; }
      virtual bool audioPortsGetConfig(uint32_t index,
                                       clap_audio_ports_config *config) const noexcept {
         return false;
      }
      virtual bool audioPortsSetConfig(clap_id configId) noexcept { return false; }

      //--------------------//
      // clap_plugin_params //
      //--------------------//
      virtual bool implementsParams() const noexcept { return false; }
      virtual uint32_t paramsCount() const noexcept { return 0; }
      virtual bool paramsInfo(int32_t paramIndex, clap_param_info *info) const noexcept {
         return false;
      }
      virtual bool
      paramsEnumValue(clap_id paramId, int32_t valueIndex, clap_param_value *value) noexcept {
         return false;
      }
      virtual bool paramsValue(clap_id paramId, clap_param_value *value) noexcept { return false; }
      virtual bool paramsSetValue(clap_id paramId,
                                  clap_param_value value,
                                  clap_param_value modulation) noexcept {
         return false;
      }
      virtual bool paramsValueToText(clap_id paramId,
                                     clap_param_value value,
                                     char *display,
                                     uint32_t size) noexcept {
         return false;
      }
      virtual bool
      paramsTextToValue(clap_id paramId, const char *display, clap_param_value *value) noexcept {
         return false;
      }
      virtual bool isValidParamId(clap_id paramId) const noexcept;

      //----------------------------//
      // clap_plugin_quick_controls //
      //----------------------------//
      virtual bool implementQuickControls() const noexcept { return false; }
      virtual uint32_t quickControlsPageCount() noexcept { return 0; }
      virtual bool quickControlsPageInfo(uint32_t pageIndex,
                                         clap_quick_controls_page *page) noexcept {
         return false;
      }
      virtual void quickControlsSelectPage(clap_id pageId) noexcept {}
      virtual clap_id quickControlsSelectedPage() noexcept { return CLAP_INVALID_ID; }

      //-----------------------//
      // clap_plugin_note_name //
      //-----------------------//
      virtual bool implementsNoteName() const noexcept { return false; }
      virtual int noteNameCount() noexcept { return 0; }
      virtual bool noteNameGet(int index, clap_note_name *noteName) noexcept { return false; }

      //------------------------//
      // clap_plugin_event_loop //
      //------------------------//
      virtual bool implementsEventLoop() const noexcept { return false; }
      virtual void eventLoopOnTimer(clap_id timerId) noexcept {}
      virtual void eventLoopOnFd(clap_fd fd, uint32_t flags) noexcept {}

      //-----------------//
      // clap_plugin_gui //
      //-----------------//
      virtual bool implementsGui() const noexcept { return false; }
      virtual void guiSize(int32_t *width, int32_t *height) noexcept {}
      virtual void guiSetScale(double scale) noexcept {}
      virtual void guiShow() noexcept {}
      virtual void guiHide() noexcept {}
      virtual void guiClose() noexcept {}

      //---------------------//
      // clap_plugin_gui_x11 //
      //---------------------//
      virtual bool implementsGuiX11() const noexcept { return false; }
      virtual bool guiX11Attach(const char *displayName, unsigned long window) noexcept {
         return false;
      }

      //-----------------------//
      // clap_plugin_gui_win32 //
      //-----------------------//
      virtual bool implementsGuiWin32() const noexcept { return false; }
      virtual bool guiWin32Attach(clap_hwnd window) noexcept { return false; }

      //-----------------------//
      // clap_plugin_gui_cocoa //
      //-----------------------//
      virtual bool implementsGuiCocoa() const noexcept { return false; }
      virtual bool guiCocoaAttach(void *nsView) noexcept { return false; }

      //-------------------------------//
      // clap_plugin_gui_free_standing //
      //-------------------------------//
      virtual bool implementsGuiFreeStanding() const noexcept { return false; }
      virtual bool guiFreeStandingOpen() noexcept { return false; }

      /////////////
      // Logging //
      /////////////
      void log(clap_log_severity severity, const char *msg) const noexcept;
      void hostMisbehaving(const char *msg) const noexcept;
      void hostMisbehaving(const std::string &msg) const noexcept { hostMisbehaving(msg.c_str()); }

      /////////////////////////////////
      // Interface consistency check //
      /////////////////////////////////
      bool canUseHostLog() const noexcept;
      bool canUseThreadCheck() const noexcept;
      bool canUseTrackInfo() const noexcept;
      bool canUseState() const noexcept;
      bool canUseEventLoop() const noexcept;
      bool canUseParams() const noexcept;
      bool canUseLatency() const noexcept;

      /////////////////////
      // Thread Checking //
      /////////////////////
      void checkMainThread() const noexcept;
      void ensureMainThread(const char *method) const noexcept;
      void ensureAudioThread(const char *method) const noexcept;

      ///////////////
      // Utilities //
      ///////////////
      static Plugin &from(const clap_plugin *plugin) noexcept;

      template <typename T>
      void initInterface(const T *&ptr, const char *id) noexcept;
      void initInterfaces() noexcept;

      static uint32_t compareAudioPortsInfo(const clap_audio_port_info &a,
                                            const clap_audio_port_info &b) noexcept;

      //////////////////////
      // Processing State //
      //////////////////////
      bool isActive() const noexcept { return isActive_; }
      bool isProcessing() const noexcept { return isProcessing_; }
      int sampleRate() const noexcept {
         assert(isActive_ && "sample rate is only known if the plugin is active");
         assert(sampleRate_ > 0);
         return sampleRate_;
      }

   protected:
      const clap_host *const host_ = nullptr;
      const clap_host_log *hostLog_ = nullptr;
      const clap_host_thread_check *hostThreadCheck_ = nullptr;
      const clap_host_thread_pool *hostThreadPool_ = nullptr;
      const clap_host_audio_ports *hostAudioPorts_ = nullptr;
      const clap_host_event_filter *hostEventFilter_ = nullptr;
      const clap_host_file_reference *hostFileReference_ = nullptr;
      const clap_host_latency *hostLatency_ = nullptr;
      const clap_host_gui *hostGui_ = nullptr;
      const clap_host_event_loop *hostEventLoop_ = nullptr;
      const clap_host_params *hostParams_ = nullptr;
      const clap_host_track_info *hostTrackInfo_ = nullptr;
      const clap_host_state *hostState_ = nullptr;
      const clap_host_note_name *hostNoteName_ = nullptr;

   private:
      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      clap_plugin plugin_;
      // clap_plugin
      static bool clapInit(const clap_plugin *plugin) noexcept;
      static void clapDestroy(const clap_plugin *plugin) noexcept;
      static bool clapActivate(const clap_plugin *plugin, int sample_rate) noexcept;
      static void clapDeactivate(const clap_plugin *plugin) noexcept;
      static bool clapStartProcessing(const clap_plugin *plugin) noexcept;
      static void clapStopProcessing(const clap_plugin *plugin) noexcept;
      static clap_process_status clapProcess(const clap_plugin *plugin,
                                             const clap_process *process) noexcept;
      static const void *clapExtension(const clap_plugin *plugin, const char *id) noexcept;

      // latency
      static uint32_t clapLatencyGet(const clap_plugin *plugin) noexcept;

      // clap_plugin_render
      static void clapRenderSetMode(const clap_plugin *plugin,
                                    clap_plugin_render_mode mode) noexcept;

      // clap_plugin_thread_pool
      static void clapThreadPoolExec(const clap_plugin *plugin, uint32_t task_index) noexcept;

      // clap_plugin_state
      static bool clapStateSave(const clap_plugin *plugin, clap_ostream *stream) noexcept;
      static bool clapStateLoad(const clap_plugin *plugin, clap_istream *stream) noexcept;
      static bool clapStateIsDirty(const clap_plugin *plugin) noexcept;

      // clap_plugin_preset
      static bool clapPresetLoadFromFile(const clap_plugin *plugin, const char *path) noexcept;

      // clap_plugin_track_info
      static void clapTrackInfoChanged(const clap_plugin *plugin) noexcept;

      // clap_plugin_audio_ports
      static uint32_t clapAudioPortsCount(const clap_plugin *plugin, bool is_input) noexcept;
      static bool clapAudioPortsInfo(const clap_plugin *plugin,
                                     uint32_t index,
                                     bool is_input,
                                     clap_audio_port_info *info) noexcept;
      static uint32_t clapAudioPortsConfigCount(const clap_plugin *plugin) noexcept;
      static bool clapAudioPortsGetConfig(const clap_plugin *plugin,
                                          uint32_t index,
                                          clap_audio_ports_config *config) noexcept;
      static bool clapAudioPortsSetConfig(const clap_plugin *plugin, clap_id config_id) noexcept;

      // clap_plugin_params
      static uint32_t clapParamsCount(const clap_plugin *plugin) noexcept;
      static bool clapParamsIinfo(const clap_plugin *plugin,
                                  int32_t param_index,
                                  clap_param_info *param_info) noexcept;
      static bool clapParamsEnumValue(const clap_plugin *plugin,
                                      clap_id param_id,
                                      int32_t value_index,
                                      clap_param_value *value) noexcept;
      static bool clapParamsValue(const clap_plugin *plugin,
                                  clap_id param_id,
                                  clap_param_value *value) noexcept;
      static bool clapParamsSetValue(const clap_plugin *plugin,
                                     clap_id param_id,
                                     clap_param_value value,
                                     clap_param_value modulation) noexcept;
      static bool clapParamsValueToText(const clap_plugin *plugin,
                                        clap_id param_id,
                                        clap_param_value value,
                                        char *display,
                                        uint32_t size) noexcept;
      static bool clapParamsTextToValue(const clap_plugin *plugin,
                                        clap_id param_id,
                                        const char *display,
                                        clap_param_value *value) noexcept;

      // clap_plugin_quick_controls
      static uint32_t clapQuickControlsPageCount(const clap_plugin *plugin) noexcept;
      static bool clapQuickControlsPageInfo(const clap_plugin *plugin,
                                            uint32_t page_index,
                                            clap_quick_controls_page *page) noexcept;
      static void clapQuickControlsSelectPage(const clap_plugin *plugin, clap_id page_id) noexcept;
      static clap_id clapQuickControlsSelectedPage(const clap_plugin *plugin) noexcept;

      // clap_plugin_note_name
      static uint32_t clapNoteNameCount(const clap_plugin *plugin) noexcept;
      static bool clapNoteNameGet(const clap_plugin *plugin,
                                  uint32_t index,
                                  clap_note_name *note_name) noexcept;

      // clap_plugin_event_loop
      static void clapEventLoopOnTimer(const clap_plugin *plugin, clap_id timer_id) noexcept;
      static void clapEventLoopOnFd(const clap_plugin *plugin, clap_fd fd, uint32_t flags) noexcept;

      // clap_plugin_gui
      static void clapGuiSize(const clap_plugin *plugin, int32_t *width, int32_t *height) noexcept;
      static void clapGuiSetScale(const clap_plugin *plugin, double scale) noexcept;
      static void clapGuiShow(const clap_plugin *plugin) noexcept;
      static void clapGuiHide(const clap_plugin *plugin) noexcept;
      static void clapGuiClose(const clap_plugin *plugin) noexcept;

      // clap_plugin_gui_x11
      static bool clapGuiX11Attach(const clap_plugin *plugin,
                                   const char *display_name,
                                   unsigned long window) noexcept;

      // clap_plugin_gui_win32
      static bool clapGuiWin32Attach(const clap_plugin *plugin, clap_hwnd window) noexcept;

      // clap_plugin_gui_cocoa
      static bool clapGuiCocoaAttach(const clap_plugin *plugin, void *nsView) noexcept;

      // clap_plugin_gui_free_standing
      static bool clapGuiFreeStandingOpen(const clap_plugin *plugin) noexcept;

      // interfaces
      static const constexpr clap_plugin_render pluginRender_ = {
         clapRenderSetMode,
      };

      static const constexpr clap_plugin_thread_pool pluginThreadPool_ = {
         clapThreadPoolExec,
      };

      static const constexpr clap_plugin_state pluginState_ = {
         clapStateSave,
         clapStateLoad,
         clapStateIsDirty,
      };

      static const constexpr clap_plugin_preset_load pluginPresetLoad_ = {
         clapPresetLoadFromFile,
      };

      static const constexpr clap_plugin_track_info pluginTrackInfo_ = {
         clapTrackInfoChanged,
      };

      static const constexpr clap_plugin_audio_ports pluginAudioPorts_ = {
         clapAudioPortsCount,
         clapAudioPortsInfo,
         clapAudioPortsConfigCount,
         clapAudioPortsGetConfig,
         clapAudioPortsSetConfig,
      };

      static const constexpr clap_plugin_params pluginParams_ = {
         clapParamsCount,
         clapParamsIinfo,
         clapParamsEnumValue,
         clapParamsValue,
         clapParamsSetValue,
         clapParamsValueToText,
         clapParamsTextToValue,
      };

      static const constexpr clap_plugin_quick_controls pluginQuickControls_ = {
         clapQuickControlsPageCount,
         clapQuickControlsPageInfo,
         clapQuickControlsSelectPage,
         clapQuickControlsSelectedPage,
      };

      static const constexpr clap_plugin_latency pluginLatency_ = {
         clapLatencyGet,
      };

      static const constexpr clap_plugin_note_name pluginNoteName_ = {
         clapNoteNameCount,
         clapNoteNameGet,
      };

      static const constexpr clap_plugin_event_loop pluginEventLoop_ = {
         clapEventLoopOnTimer,
         clapEventLoopOnFd,
      };

      static const constexpr clap_plugin_gui pluginGui_ = {
         clapGuiSize,
         clapGuiSetScale,
         clapGuiShow,
         clapGuiHide,
         clapGuiClose,
      };

      static const constexpr clap_plugin_gui_x11 pluginGuiX11_ = {
         clapGuiX11Attach,
      };

      static const constexpr clap_plugin_gui_win32 pluginGuiWin32_ = {
         clapGuiWin32Attach,
      };

      static const constexpr clap_plugin_gui_cocoa pluginGuiCocoa_ = {
         clapGuiCocoaAttach,
      };

      static const constexpr clap_plugin_gui_free_standing pluginGuiFreeStanding_ = {
         clapGuiFreeStandingOpen,
      };

      // state
      bool isActive_ = false;
      bool isProcessing_ = false;
      int sampleRate_ = 0;
   };
} // namespace clap