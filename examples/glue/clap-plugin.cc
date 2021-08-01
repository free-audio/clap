#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "clap-plugin.hh"

namespace clap {

   Plugin::Plugin(const clap_plugin_descriptor *desc, const clap_host *host) : host_(host) {
      plugin_.plugin_data = this;
      plugin_.desc = desc;
      plugin_.init = Plugin::clapInit;
      plugin_.destroy = Plugin::clapDestroy;
      plugin_.extension = nullptr;
      plugin_.process = nullptr;
      plugin_.activate = nullptr;
      plugin_.deactivate = nullptr;
      plugin_.start_processing = nullptr;
      plugin_.stop_processing = nullptr;
   }

   Plugin::~Plugin() = default;

   /////////////////////
   // CLAP Interfaces //
   /////////////////////

   //-------------//
   // clap_plugin //
   //-------------//
   bool Plugin::clapInit(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);

      self.plugin_.extension = Plugin::clapExtension;
      self.plugin_.process = Plugin::clapProcess;
      self.plugin_.activate = Plugin::clapActivate;
      self.plugin_.deactivate = Plugin::clapDeactivate;
      self.plugin_.start_processing = Plugin::clapStartProcessing;
      self.plugin_.stop_processing = Plugin::clapStopProcessing;

      self.initInterfaces();
      self.ensureMainThread("clap_plugin.init");
      return self.init();
   }

   void Plugin::clapDestroy(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.destroy");
      delete &from(plugin);
   }

   bool Plugin::clapActivate(const clap_plugin *plugin, double sample_rate) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.activate");

      if (self.isActive_) {
         self.hostMisbehaving("Plugin was activated twice");

         if (sample_rate != self.sampleRate_) {
            std::ostringstream msg;
            msg << "The plugin was activated twice and with different sample rates: "
                << self.sampleRate_ << " and " << sample_rate
                << ". The host must deactivate the plugin first." << std::endl
                << "Simulating deactivation.";
            self.hostMisbehaving(msg.str());
            clapDeactivate(plugin);
         }
      }

      if (sample_rate <= 0) {
         std::ostringstream msg;
         msg << "The plugin was activated with an invalid sample rates: " << sample_rate;
         self.hostMisbehaving(msg.str());
         return false;
      }

      assert(!self.isActive_);
      assert(self.sampleRate_ == 0);

      if (!self.activate(sample_rate)) {
         assert(!self.isActive_);
         assert(self.sampleRate_ == 0);
         return false;
      }

      self.isActive_ = true;
      self.sampleRate_ = sample_rate;
      return true;
   }

   void Plugin::clapDeactivate(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.deactivate");

      if (!self.isActive_) {
         self.hostMisbehaving("The plugin was deactivated twice.");
         return;
      }

      self.deactivate();
   }

   bool Plugin::clapStartProcessing(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureAudioThread("clap_plugin.start_processing");

      if (!self.isActive_) {
         self.hostMisbehaving("Host called clap_plugin.start_processing() on a deactivated plugin");
         return false;
      }

      if (self.isProcessing_) {
         self.hostMisbehaving("Host called clap_plugin.start_processing() twice");
         return true;
      }

      self.isProcessing_ = self.startProcessing();
      return self.isProcessing_;
   }

   void Plugin::clapStopProcessing(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureAudioThread("clap_plugin.stop_processing");

      if (!self.isActive_) {
         self.hostMisbehaving("Host called clap_plugin.stop_processing() on a deactivated plugin");
         return;
      }

      if (!self.isProcessing_) {
         self.hostMisbehaving("Host called clap_plugin.stop_processing() twice");
         return;
      }

      self.stopProcessing();
      self.isProcessing_ = false;
   }

   clap_process_status Plugin::clapProcess(const clap_plugin *plugin,
                                           const clap_process *process) noexcept {
      auto &self = from(plugin);
      self.ensureAudioThread("clap_plugin.process");

      if (!self.isActive_) {
         self.hostMisbehaving("Host called clap_plugin.process() on a deactivated plugin");
         return CLAP_PROCESS_ERROR;
      }

      if (!self.isProcessing_) {
         self.hostMisbehaving(
            "Host called clap_plugin.process() without calling clap_plugin.start_processing()");
         return CLAP_PROCESS_ERROR;
      }

      return self.process(process);
   }

   const void *Plugin::clapExtension(const clap_plugin *plugin, const char *id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.extension");

      if (!strcmp(id, CLAP_EXT_STATE) && self.implementsState())
         return &pluginState_;
      if (!strcmp(id, CLAP_EXT_PRESET_LOAD) && self.implementsPresetLoad())
         return &pluginPresetLoad_;
      if (!strcmp(id, CLAP_EXT_RENDER) && self.implementsRender())
         return &pluginRender_;
      if (!strcmp(id, CLAP_EXT_TRACK_INFO) && self.implementsTrackInfo())
         return &pluginTrackInfo_;
      if (!strcmp(id, CLAP_EXT_LATENCY) && self.implementsLatency())
         return &pluginLatency_;
      if (!strcmp(id, CLAP_EXT_AUDIO_PORTS) && self.implementsAudioPorts())
         return &pluginAudioPorts_;
      if (!strcmp(id, CLAP_EXT_PARAMS) && self.implementsParams())
         return &pluginParams_;
      if (!strcmp(id, CLAP_EXT_QUICK_CONTROLS) && self.implementQuickControls())
         return &pluginQuickControls_;
      if (!strcmp(id, CLAP_EXT_NOTE_NAME) && self.implementsNoteName())
         return &pluginNoteName_;
      if (!strcmp(id, CLAP_EXT_THREAD_POOL) && self.implementsThreadPool())
         return &pluginThreadPool_;
      if (!strcmp(id, CLAP_EXT_EVENT_LOOP) && self.implementsEventLoop())
         return &pluginEventLoop_;
      if (!strcmp(id, CLAP_EXT_GUI) && self.implementsGui())
         return &pluginGui_;
      if (!strcmp(id, CLAP_EXT_GUI_X11) && self.implementsGuiX11())
         return &pluginGuiX11_;
      if (!strcmp(id, CLAP_EXT_GUI_WIN32) && self.implementsGuiWin32())
         return &pluginGuiWin32_;
      if (!strcmp(id, CLAP_EXT_GUI_COCOA) && self.implementsGuiCocoa())
         return &pluginGuiCocoa_;
      if (!strcmp(id, CLAP_EXT_GUI_FREE_STANDING) && self.implementsGuiFreeStanding())
         return &pluginGuiFreeStanding_;

      return from(plugin).extension(id);
   }

   template <typename T>
   void Plugin::initInterface(const T *&ptr, const char *id) noexcept {
      assert(!ptr);
      assert(id);

      if (host_->extension)
         ptr = static_cast<const T *>(host_->extension(host_, id));
   }

   void Plugin::initInterfaces() noexcept {
      initInterface(hostLog_, CLAP_EXT_LOG);
      initInterface(hostThreadCheck_, CLAP_EXT_THREAD_CHECK);
      initInterface(hostThreadPool_, CLAP_EXT_THREAD_POOL);
      initInterface(hostAudioPorts_, CLAP_EXT_AUDIO_PORTS);
      initInterface(hostEventLoop_, CLAP_EXT_EVENT_LOOP);
      initInterface(hostEventFilter_, CLAP_EXT_EVENT_FILTER);
      initInterface(hostFileReference_, CLAP_EXT_FILE_REFERENCE);
      initInterface(hostLatency_, CLAP_EXT_LATENCY);
      initInterface(hostGui_, CLAP_EXT_GUI);
      initInterface(hostParams_, CLAP_EXT_PARAMS);
      initInterface(hostTrackInfo_, CLAP_EXT_TRACK_INFO);
      initInterface(hostState_, CLAP_EXT_STATE);
      initInterface(hostNoteName_, CLAP_EXT_NOTE_NAME);
   }

   //-------------------//
   // clap_plugin_state //
   //-------------------//
   uint32_t Plugin::clapLatencyGet(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_latency.get");

      return self.latencyGet();
   }

   //--------------------//
   // clap_plugin_render //
   //--------------------//
   void Plugin::clapRenderSetMode(const clap_plugin *plugin,
                                  clap_plugin_render_mode mode) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_render.set_mode");

      switch (mode) {
      case CLAP_RENDER_REALTIME:
      case CLAP_RENDER_OFFLINE:
         self.renderSetMode(mode);
         break;

      default: {
         std::ostringstream msg;
         msg << "host called clap_plugin_render.set_mode with an unknown mode : " << mode;
         self.hostMisbehaving(msg.str());
         break;
      }
      }
   }

   //-------------------------//
   // clap_plugin_thread_pool //
   //-------------------------//
   void Plugin::clapThreadPoolExec(const clap_plugin *plugin, uint32_t task_index) noexcept {
      auto &self = from(plugin);

      self.threadPoolExec(task_index);
   }

   //-------------------//
   // clap_plugin_state //
   //-------------------//
   bool Plugin::clapStateSave(const clap_plugin *plugin, clap_ostream *stream) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_state.save");

      return self.stateSave(stream);
   }

   bool Plugin::clapStateLoad(const clap_plugin *plugin, clap_istream *stream) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_state.load");

      return self.stateLoad(stream);
   }

   //-------------------------//
   // clap_plugin_preset_load //
   //-------------------------//
   bool Plugin::clapPresetLoadFromFile(const clap_plugin *plugin, const char *path) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_preset_load.from_file");

      if (!path) {
         self.hostMisbehaving("host called clap_plugin_preset_load.from_file with a null path");
         return false;
      }

      // TODO check if the file is readable

      return self.presetLoadFromFile(path);
   }

   //------------------------//
   // clap_plugin_track_info //
   //------------------------//

   void Plugin::clapTrackInfoChanged(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_track_info.changed");

      if (!self.canUseTrackInfo()) {
         self.hostMisbehaving("host called clap_plugin_track_info.changed() but does not provide a "
                              "complete clap_host_track_info interface");
         return;
      }

      self.trackInfoChanged();
   }

   //-------------------------//
   // clap_plugin_audio_ports //
   //-------------------------//

   uint32_t Plugin::clapAudioPortsCount(const clap_plugin *plugin, bool is_input) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.count");

      return self.audioPortsCount(is_input);
   }

   bool Plugin::clapAudioPortsInfo(const clap_plugin *plugin,
                                   uint32_t index,
                                   bool is_input,
                                   clap_audio_port_info *info) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.info");
      auto count = clapAudioPortsCount(plugin, is_input);
      if (index >= count) {
         std::ostringstream msg;
         msg << "Host called clap_plugin_audio_ports.info() with an index out of bounds: " << index
             << " >= " << count;
         self.hostMisbehaving(msg.str());
         return false;
      }

      return self.audioPortsInfo(index, is_input, info);
   }

   uint32_t Plugin::clapAudioPortsConfigCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.config_count");
      return self.audioPortsConfigCount();
   }

   bool Plugin::clapAudioPortsGetConfig(const clap_plugin *plugin,
                                        uint32_t index,
                                        clap_audio_ports_config *config) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.get_config");

      auto count = clapAudioPortsConfigCount(plugin);
      if (index >= count) {
         std::ostringstream msg;
         msg << "called clap_plugin_audio_ports.get_config with an index out of bounds: " << index
             << " >= " << count;
         self.hostMisbehaving(msg.str());
         return false;
      }
      return self.audioPortsGetConfig(index, config);
   }

   bool Plugin::clapAudioPortsSetConfig(const clap_plugin *plugin, clap_id config_id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.get_config");

      if (self.isActive())
         self.hostMisbehaving(
            "it is illegal to call clap_audio_ports.set_config if the plugin is active");

      return self.audioPortsSetConfig(config_id);
   }

   uint32_t Plugin::clapParamsCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.count");

      return self.paramsCount();
   }

   //--------------------//
   // clap_plugin_params //
   //--------------------//
   bool Plugin::clapParamsInfo(const clap_plugin *plugin,
                               int32_t param_index,
                               clap_param_info *param_info) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.info");

      auto count = clapParamsCount(plugin);
      if (param_index >= count) {
         std::ostringstream msg;
         msg << "called clap_plugin_params.info with an index out of bounds: " << param_index
             << " >= " << count;
         self.hostMisbehaving(msg.str());
         return false;
      }

      return self.paramsInfo(param_index, param_info);
   }

   bool
   Plugin::clapParamsValue(const clap_plugin *plugin, clap_id param_id, double *value) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.value");

      if (!self.isValidParamId(param_id)) {
         std::ostringstream msg;
         msg << "clap_plugin_params.value called with invalid param_id: " << param_id;
         self.hostMisbehaving(msg.str());
         return false;
      }

      // TODO extra checks

      return self.paramsValue(param_id, value);
   }

   bool Plugin::clapParamsValueToText(const clap_plugin *plugin,
                                      clap_id param_id,
                                      double value,
                                      char *display,
                                      uint32_t size) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.value_to_text");

      if (!self.isValidParamId(param_id)) {
         std::ostringstream msg;
         msg << "clap_plugin_params.value_to_text called with invalid param_id: " << param_id;
         self.hostMisbehaving(msg.str());
         return false;
      }

      // TODO: extra checks
      return self.paramsValueToText(param_id, value, display, size);
   }

   bool Plugin::clapParamsTextToValue(const clap_plugin *plugin,
                                      clap_id param_id,
                                      const char *display,
                                      double *value) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.text_to_value");

      if (!self.isValidParamId(param_id)) {
         std::ostringstream msg;
         msg << "clap_plugin_params.text_to_value called with invalid param_id: " << param_id;
         self.hostMisbehaving(msg.str());
         return false;
      }

      // TODO: extra checks
      return self.paramsTextToValue(param_id, display, value);
   }

   bool Plugin::isValidParamId(clap_id param_id) const noexcept {
      checkMainThread();

      auto count = paramsCount();
      clap_param_info info;
      for (uint32_t i = 0; i < count; ++i) {
         if (!paramsInfo(i, &info))
            // TODO: fatal error?
            continue;

         if (info.id == param_id)
            return true;
      }
      return false;
   }

   //----------------------------//
   // clap_plugin_quick_controls //
   //----------------------------//
   uint32_t Plugin::clapQuickControlsPageCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.page_count");

      return self.quickControlsPageCount();
   }

   bool Plugin::clapQuickControlsPageInfo(const clap_plugin *plugin,
                                          uint32_t page_index,
                                          clap_quick_controls_page *page) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.page_info");

      uint32_t count = clapQuickControlsPageCount(plugin);
      if (page_index >= count) {
         std::ostringstream msg;
         msg << "Host called clap_plugin_quick_controls.page_info() with an index out of bounds: "
             << page_index << " >= " << count;
         self.hostMisbehaving(msg.str());
         return false;
      }

      return self.quickControlsPageInfo(page_index, page);
   }

   void Plugin::clapQuickControlsSelectPage(const clap_plugin *plugin, clap_id page_id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.select_page");

      return self.quickControlsSelectPage(page_id);
   }

   clap_id Plugin::clapQuickControlsSelectedPage(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.selected_page");

      return self.quickControlsSelectedPage();
   }

   //-----------------------//
   // clap_plugin_note_name //
   //-----------------------//

   uint32_t Plugin::clapNoteNameCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_note_name.count");
      return self.noteNameCount();
   }

   bool Plugin::clapNoteNameGet(const clap_plugin *plugin,
                                uint32_t index,
                                clap_note_name *note_name) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_note_name.get");

      // TODO check index
      auto count = clapNoteNameCount(plugin);
      if (index >= count) {
         std::ostringstream msg;
         msg << "host called clap_plugin_note_name.get with an index out of bounds: " << index
             << " >= " << count;
         self.hostMisbehaving(msg.str());
         return false;
      }

      return self.noteNameGet(index, note_name);
   }

   //------------------------//
   // clap_plugin_event_loop //
   //------------------------//
   void Plugin::clapEventLoopOnTimer(const clap_plugin *plugin, clap_id timer_id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_event_loop.on_timer");

      if (timer_id == CLAP_INVALID_ID) {
         self.hostMisbehaving(
            "Host called clap_plugin_event_loop.on_timer with an invalid timer_id");
         return;
      }

      self.eventLoopOnTimer(timer_id);
   }

   void Plugin::clapEventLoopOnFd(const clap_plugin *plugin, clap_fd fd, uint32_t flags) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_event_loop.on_fd");

      self.eventLoopOnFd(fd, flags);
   }

   //-----------------//
   // clap_plugin_gui //
   //-----------------//
   bool Plugin::clapGuiSize(const clap_plugin *plugin, uint32_t *width, uint32_t *height) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.size");

      return self.guiSize(width, height);
   }

   bool Plugin::clapGuiCanResize(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.can_resize");

      return self.guiCanResize();
   }

   void Plugin::clapGuiRoundSize(const clap_plugin *plugin, uint32_t *width, uint32_t *height) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.round_size");

      self.guiRoundSize(width, height);
   }

   bool Plugin::clapGuiSetSize(const clap_plugin *plugin, uint32_t width, uint32_t height) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.set_size");

      return self.guiSetSize(width, height);
   }

   void Plugin::clapGuiSetScale(const clap_plugin *plugin, double scale) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.set_scale");

      self.guiSetScale(scale);
   }

   void Plugin::clapGuiShow(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.show");

      self.guiShow();
   }

   void Plugin::clapGuiHide(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.hide");

      self.guiHide();
   }

   bool Plugin::clapGuiCreate(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.create");

      return self.guiCreate();
   }

   void Plugin::clapGuiDestroy(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.destroy");

      self.guiDestroy();
   }

   //---------------------//
   // clap_plugin_gui_x11 //
   //---------------------//
   bool Plugin::clapGuiX11Attach(const clap_plugin *plugin,
                                 const char *display_name,
                                 unsigned long window) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_x11.attach");

      return self.guiX11Attach(display_name, window);
   }

   //-----------------------//
   // clap_plugin_gui_win32 //
   //-----------------------//
   bool Plugin::clapGuiWin32Attach(const clap_plugin *plugin, clap_hwnd window) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_win32.attach");

      return self.guiWin32Attach(window);
   }

   //-----------------------//
   // clap_plugin_gui_cocoa //
   //-----------------------//
   bool Plugin::clapGuiCocoaAttach(const clap_plugin *plugin, void *nsView) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_cocoa.attach");

      return self.guiCocoaAttach(nsView);
   }

   //-------------------------------//
   // clap_plugin_gui_free_standing //
   //-------------------------------//
   bool Plugin::clapGuiFreeStandingOpen(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_win32.attach");

      return self.guiFreeStandingOpen();
   }

   /////////////
   // Logging //
   /////////////
   void Plugin::log(clap_log_severity severity, const char *msg) const noexcept {
      if (canUseHostLog())
         hostLog_->log(host_, severity, msg);
      else
         std::clog << msg << std::endl;
   }

   void Plugin::hostMisbehaving(const char *msg) const noexcept {
      log(CLAP_LOG_HOST_MISBEHAVING, msg);
   }

   /////////////////////////////////
   // Interface consistency check //
   /////////////////////////////////

   bool Plugin::canUseHostLog() const noexcept { return hostLog_ && hostLog_->log; }

   bool Plugin::canUseThreadCheck() const noexcept {
      return hostThreadCheck_ && hostThreadCheck_->is_audio_thread &&
             hostThreadCheck_->is_main_thread;
   }

   bool Plugin::canUseEventLoop() const noexcept {
      if (!hostEventLoop_)
         return false;

      auto &x = *hostEventLoop_;
      if (x.modify_fd && x.register_fd && x.unregister_fd && x.register_timer && x.unregister_timer)
         return true;

      hostMisbehaving("clap_event_loop is partially implemented");
      return false;
   }

   bool Plugin::canUseParams() const noexcept {
      if (!hostParams_)
         return false;

      if (hostParams_->rescan && hostParams_->clear)
         return true;

      hostMisbehaving("clap_host_params is partially implemented");
      return false;
   }

   bool Plugin::canUseLatency() const noexcept {
      if (!hostLatency_)
         return false;

      if (hostLatency_->changed)
         return true;

      hostMisbehaving("clap_host_latency is partially implemented");
      return false;
   }

   bool Plugin::canUseState() const noexcept {
      if (!hostState_)
         return false;

      if (hostState_->mark_dirty)
         return true;

      hostMisbehaving("clap_host_state is partially implemented");
      return false;
   }

   bool Plugin::canUseTrackInfo() const noexcept {
      if (!hostTrackInfo_)
         return false;

      if (hostTrackInfo_->get)
         return true;

      hostMisbehaving("clap_host_track_info is partially implemented");
      return false;
   }

   /////////////////////
   // Thread Checking //
   /////////////////////

   void Plugin::checkMainThread() const noexcept {
      if (!hostThreadCheck_ || !hostThreadCheck_->is_main_thread ||
          hostThreadCheck_->is_main_thread(host_))
         return;

      std::terminate();
   }

   void Plugin::ensureMainThread(const char *method) const noexcept {
      if (!hostThreadCheck_ || !hostThreadCheck_->is_main_thread ||
          hostThreadCheck_->is_main_thread(host_))
         return;

      std::ostringstream msg;
      msg << "Host called the method " << method
          << "() on wrong thread! It must be called on main thread!";
      hostMisbehaving(msg.str());
      std::terminate();
   }

   void Plugin::ensureAudioThread(const char *method) const noexcept {
      if (!hostThreadCheck_ || !hostThreadCheck_->is_audio_thread ||
          hostThreadCheck_->is_audio_thread(host_))
         return;

      std::ostringstream msg;
      msg << "Host called the method " << method
          << "() on wrong thread! It must be called on audio thread!";
      hostMisbehaving(msg.str());
      std::terminate();
   }

   ///////////////
   // Utilities //
   ///////////////
   Plugin &Plugin::from(const clap_plugin *plugin) noexcept {
      if (!plugin) {
         std::cerr << "called with a null clap_plugin pointer!" << std::endl;
         std::terminate();
      }

      if (!plugin->plugin_data) {
         std::cerr << "called with a null clap_plugin->plugin_data pointer! The host must never "
                      "change this pointer!"
                   << std::endl;
         std::terminate();
      }

      return *static_cast<Plugin *>(plugin->plugin_data);
   }

   uint32_t Plugin::compareAudioPortsInfo(const clap_audio_port_info &a,
                                          const clap_audio_port_info &b) noexcept {
      if (a.sample_size != b.sample_size || a.in_place != b.in_place || a.is_cv != b.is_cv ||
          a.is_main != b.is_main || a.channel_count != b.channel_count ||
          a.channel_map != b.channel_map || a.id != b.id)
         return CLAP_AUDIO_PORTS_RESCAN_ALL;

      if (strncmp(a.name, b.name, sizeof(a.name)))
         return CLAP_AUDIO_PORTS_RESCAN_NAMES;

      return 0;
   }
} // namespace clap