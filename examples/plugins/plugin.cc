#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "plugin.hh"

namespace clap {

   Plugin::Plugin(const clap_plugin_descriptor *desc, const clap_host *host) : host_(host) {
      plugin_.plugin_data      = this;
      plugin_.desc             = desc;
      plugin_.init             = Plugin::clapInit;
      plugin_.destroy          = Plugin::clapDestroy;
      plugin_.extension        = nullptr;
      plugin_.process          = nullptr;
      plugin_.activate         = nullptr;
      plugin_.deactivate       = nullptr;
      plugin_.start_processing = nullptr;
      plugin_.stop_processing  = nullptr;
   }

   /////////////////////
   // CLAP Interfaces //
   /////////////////////

   // clap_plugin interface
   bool Plugin::clapInit(const clap_plugin *plugin) {
      auto &self = from(plugin);

      self.plugin_.extension        = Plugin::clapExtension;
      self.plugin_.process          = Plugin::clapProcess;
      self.plugin_.activate         = Plugin::clapActivate;
      self.plugin_.deactivate       = Plugin::clapDeactivate;
      self.plugin_.start_processing = Plugin::clapStartProcessing;
      self.plugin_.stop_processing  = Plugin::clapStopProcessing;

      self.initInterfaces();
      self.ensureMainThread("clap_plugin.init");
      self.initTrackInfo();
      self.defineAudioPorts(self.inputAudioPorts_, self.outputAudioPorts_);
      return self.init();
   }

   void Plugin::clapDestroy(const clap_plugin *plugin) {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.destroy");
      delete &from(plugin);
   }

   bool Plugin::clapActivate(const clap_plugin *plugin, int sample_rate) {
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

      self.isActive_   = true;
      self.sampleRate_ = sample_rate;
      return true;
   }

   void Plugin::clapDeactivate(const clap_plugin *plugin) {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.deactivate");

      if (!self.isActive_) {
         self.hostMisbehaving("The plugin was deactivated twice.");
         return;
      }

      if (self.scheduleAudioPortsUpdate_)
         self.updateAudioPorts();

      self.deactivate();
   }

   bool Plugin::clapStartProcessing(const clap_plugin *plugin) {
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

   void Plugin::clapStopProcessing(const clap_plugin *plugin) {
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

   clap_process_status Plugin::clapProcess(const clap_plugin *plugin, const clap_process *process) {
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

   const void *Plugin::clapExtension(const clap_plugin *plugin, const char *id) {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.extension");

      if (!strcmp(id, CLAP_EXT_RENDER))
         return &self.pluginRender_;
      if (!strcmp(id, CLAP_EXT_TRACK_INFO))
         return &pluginTrackInfo_;
      if (!strcmp(id, CLAP_EXT_AUDIO_PORTS))
         return &pluginAudioPorts_;

      return from(plugin).extension(id);
   }

   void Plugin::clapTrackInfoChanged(const clap_plugin *plugin) {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_track_info.changed");

      if (!self.canUseTrackInfo()) {
         self.hostMisbehaving("Host called clap_plugin_track_info.changed() but does not provide a "
                              "complete clap_host_track_info interface");
         return;
      }

      clap_track_info info;
      if (!self.hostTrackInfo_->get(self.host_, &info)) {
         self.hasTrackInfo_ = false;
         self.hostMisbehaving(
            "clap_host_track_info.get() failed after calling clap_plugin_track_info.changed()");
         return;
      }

      const bool didChannelChange = info.channel_count != self.trackInfo_.channel_count ||
                                    info.channel_map != self.trackInfo_.channel_map;
      self.trackInfo_    = info;
      self.hasTrackInfo_ = true;

      if (didChannelChange && self.canChangeAudioPorts() &&
          self.shouldInvalidateAudioPortsDefinitionOnTrackChannelChange())
         self.invalidateAudioPortsDefinition();

      self.trackInfoChanged();
   }

   void Plugin::invalidateAudioPortsDefinition() {
      checkMainThread();

      if (isActive()) {
         scheduleAudioPortsUpdate_ = true;
         hostAudioPorts_->rescan(host_, CLAP_AUDIO_PORTS_RESCAN_ALL);
         return;
      }

      updateAudioPorts();
   }

   void Plugin::initTrackInfo() {
      checkMainThread();

      assert(!hasTrackInfo_);
      if (!canUseTrackInfo())
         return;

      hasTrackInfo_ = hostTrackInfo_->get(host_, &trackInfo_);
   }

   uint32_t Plugin::clapAudioPortsCount(const clap_plugin *plugin, bool is_input) {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.count");

      return is_input ? self.inputAudioPorts_.size() : self.outputAudioPorts_.size();
   }

   bool Plugin::clapAudioPortsInfo(const clap_plugin *   plugin,
                                   uint32_t              index,
                                   bool                  is_input,
                                   clap_audio_port_info *info) {
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

      *info = is_input ? self.inputAudioPorts_[index] : self.outputAudioPorts_[index];
      return true;
   }

   void Plugin::updateAudioPorts() {
      checkMainThread();
      assert(!isActive());

      scheduleAudioPortsUpdate_ = false;

      // Get the new list of ports
      std::vector<clap_audio_port_info> inputs;
      std::vector<clap_audio_port_info> outputs;
      defineAudioPorts(inputAudioPorts_, outputAudioPorts_);

      uint32_t flags = 0;

      if (inputs.size() != inputAudioPorts_.size() || outputs.size() != outputAudioPorts_.size())
         flags = CLAP_AUDIO_PORTS_RESCAN_ALL;
      else {
         for (int i = 0; i < inputs.size() && !(flags & CLAP_AUDIO_PORTS_RESCAN_ALL); ++i)
            flags |= compareAudioPortsInfo(inputs[i], inputAudioPorts_[i]);
         for (int i = 0; i < outputs.size() && !(flags & CLAP_AUDIO_PORTS_RESCAN_ALL); ++i)
            flags |= compareAudioPortsInfo(outputs[i], outputAudioPorts_[i]);
      }

      if (flags == 0)
         return;

      // compare the list of ports to the old one
      inputAudioPorts_  = inputs;
      outputAudioPorts_ = outputs;

      hostAudioPorts_->rescan(host_, flags);
   }

   /////////////
   // Logging //
   /////////////
   void Plugin::log(clap_log_severity severity, const char *msg) const {
      if (canUseHostLog())
         hostLog_->log(host_, severity, msg);
      else
         std::clog << msg << std::endl;
   }

   void Plugin::hostMisbehaving(const char *msg) { log(CLAP_LOG_HOST_MISBEHAVING, msg); }

   /////////////////////////////////
   // Interface consistency check //
   /////////////////////////////////

   bool Plugin::canUseHostLog() const noexcept { return hostLog_ && hostLog_->log; }

   bool Plugin::canUseThreadCheck() const noexcept {
      return hostThreadCheck_ && hostThreadCheck_->is_audio_thread &&
             hostThreadCheck_->is_main_thread;
   }

   /////////////////////
   // Thread Checking //
   /////////////////////

   void Plugin::checkMainThread() {
      if (!hostThreadCheck_ || !hostThreadCheck_->is_main_thread ||
          hostThreadCheck_->is_main_thread(host_))
         return;

      std::terminate();
   }

   void Plugin::ensureMainThread(const char *method) {
      if (!hostThreadCheck_ || !hostThreadCheck_->is_main_thread ||
          hostThreadCheck_->is_main_thread(host_))
         return;

      std::ostringstream msg;
      msg << "Host called the method " << method
          << "() on wrong thread! It must be called on main thread!";
      hostMisbehaving(msg.str());
      std::terminate();
   }

   void Plugin::ensureAudioThread(const char *method) {
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
   Plugin &Plugin::from(const clap_plugin *plugin) {
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

   template <typename T>
   void Plugin::initInterface(const T *&ptr, const char *id) {
      assert(!ptr);
      assert(id);

      if (host_->extension)
         ptr = static_cast<const T *>(host_->extension(host_, id));
   }

   void Plugin::initInterfaces() {
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

   int Plugin::sampleRate() const noexcept {
      assert(isActive_ &&
             "there is no point in querying the sample rate if the plugin isn't activated");
      assert(isActive_ ? sampleRate_ > 0 : sampleRate_ == 0);
      return sampleRate_;
   }
} // namespace clap