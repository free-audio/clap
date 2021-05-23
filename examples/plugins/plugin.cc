#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>

#include "plugin.hh"

Plugin::Plugin(const clap_plugin_descriptor *desc, clap_host *host) : host_(host) {
   plugin_.plugin_data = this;
   plugin_.desc = desc;
   plugin_.init = Plugin::clapPluginInit;
   plugin_.destroy = Plugin::clapPluginDestroy;
   plugin_.extension = Plugin::clapPluginExtension;
   plugin_.process = Plugin::clapPluginProcess;
   plugin_.activate = Plugin::clapPluginActivate;
   plugin_.deactivate = Plugin::clapPluginDeactivate;
   plugin_.start_processing = Plugin::clapPluginStartProcessing;
   plugin_.stop_processing = Plugin::clapPluginStopProcessing;
}

/////////////////////
// CLAP Interfaces //
/////////////////////

// clap_plugin interface
bool Plugin::clapPluginInit(clap_plugin *plugin) {
   auto &self = from(plugin);
   self.initInterfaces();
   self.ensureMainThread("clap_plugin.init");
   return self.init();
}

void Plugin::clapPluginDestroy(clap_plugin *plugin) {
   auto &self = from(plugin);
   self.ensureMainThread("clap_plugin.destroy");
   delete &from(plugin);
}

bool Plugin::clapPluginActivate(clap_plugin *plugin, int sample_rate) {
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
         clapPluginDeactivate(plugin);
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

void Plugin::clapPluginDeactivate(clap_plugin *plugin) {
   auto &self = from(plugin);
   self.ensureMainThread("clap_plugin.deactivate");

   if (!self.isActive_) {
      self.hostMisbehaving("The plugin was deactivated twice.");
      return;
   }

   self.deactivate();
}

bool Plugin::clapPluginStartProcessing(clap_plugin *plugin) {
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

void Plugin::clapPluginStopProcessing(clap_plugin *plugin) {
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

clap_process_status Plugin::clapPluginProcess(struct clap_plugin *plugin,
                                              const clap_process *process) {
   auto &self = from(plugin);
   self.ensureAudioThread("clap_plugin.process");

   if (!self.isActive_) {
      self.hostMisbehaving("Host called clap_plugin.process() on a deactivated plugin");
      return CLAP_PROCESS_ERROR;
   }

   if (!self.isProcessing_) {
      self.hostMisbehaving("Host called clap_plugin.process() without calling clap_plugin.start_processing()");
      return CLAP_PROCESS_ERROR;
   }

   return self.process(process);
}

const void *Plugin::clapPluginExtension(struct clap_plugin *plugin, const char *id) {
   auto &self = from(plugin);
   self.ensureMainThread("clap_plugin.extension");

   if (!strcmp(id, CLAP_EXT_RENDER))
      return &self.pluginRender_;

   return from(plugin).extension(id);
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
   return hostThreadCheck_ && hostThreadCheck_->is_audio_thread && hostThreadCheck_->is_main_thread;
}

/////////////////////
// Thread Checking //
/////////////////////

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
Plugin &Plugin::from(clap_plugin *plugin) {
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