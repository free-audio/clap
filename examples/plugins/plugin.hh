#pragma once

#include <string_view>
#include <string>

#include <clap/all.h>

class Plugin {
protected:
   Plugin(const clap_plugin_descriptor *desc, clap_host *host);
   virtual ~Plugin() = default;

   Plugin(const Plugin &) = delete;
   Plugin(Plugin &&) = delete;
   Plugin &operator=(const Plugin &) = delete;
   Plugin &operator=(Plugin &&) = delete;

   virtual bool                init() { return true; }
   virtual bool                activate(int sample_rate) { return true; }
   virtual void                deactivate() {}
   virtual bool                startProcessing() { return true; }
   virtual void                stopProcessing() {}
   virtual clap_process_status process(const clap_process *process) { return CLAP_PROCESS_SLEEP; }
   virtual const void *        extension(const char *id) { return nullptr; }

   /////////////////////
   // CLAP Interfaces //
   /////////////////////

   // clap_plugin interface
   static bool                clapPluginInit(clap_plugin *plugin);
   static void                clapPluginDestroy(clap_plugin *plugin);
   static bool                clapPluginActivate(clap_plugin *plugin, int sample_rate);
   static void                clapPluginDeactivate(clap_plugin *plugin);
   static bool                clapPluginStartProcessing(clap_plugin *plugin);
   static void                clapPluginStopProcessing(clap_plugin *plugin);
   static clap_process_status clapPluginProcess(struct clap_plugin *plugin,
                                                const clap_process *process);
   static const void *        clapPluginExtension(struct clap_plugin *plugin, const char *id);

   /////////////
   // Logging //
   /////////////
   void log(clap_log_severity severity, const char *msg) const;
   void hostMisbehaving(const char *msg);
   void hostMisbehaving(const std::string& msg) { hostMisbehaving(msg.c_str()); }

   /////////////////////////////////
   // Interface consistency check //
   /////////////////////////////////
   bool canUseHostLog() const noexcept;
   bool canUseThreadCheck() const noexcept;

   /////////////////////
   // Thread Checking //
   /////////////////////
   void ensureMainThread(const char *method);
   void ensureAudioThread(const char *method);

   ///////////////
   // Utilities //
   ///////////////
   static Plugin &from(clap_plugin *plugin);

   template <typename T>
   void initInterface(const T *&ptr, const char *id);
   void initInterfaces();

protected:
   clap_plugin              plugin_;
   clap_plugin_audio_ports  pluginAudioPorts_;
   clap_plugin_event_filter pluginEventFilter_;
   clap_plugin_latency      pluginLatency_;
   clap_plugin_params       pluginParams_;
   clap_plugin_render       pluginRender_;
   clap_plugin_track_info   pluginTrackInfo_;
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

   clap_host *const                host_ = nullptr;
   const clap_host_log *           hostLog_ = nullptr;
   const clap_host_thread_check *  hostThreadCheck_ = nullptr;
   const clap_host_thread_pool *   hostThreadPool_ = nullptr;
   const clap_host_audio_ports *   hostAudioPorts_ = nullptr;
   const clap_host_event_filter *  hostEventFilter_ = nullptr;
   const clap_host_file_reference *hostFileReference_ = nullptr;
   const clap_host_latency *       hostLatency_ = nullptr;
   const clap_host_gui *           hostGui_ = nullptr;
   const clap_host_event_loop *    hostEventLoop_ = nullptr;
   const clap_host_params *        hostParams_ = nullptr;
   const clap_host_track_info *    hostTrackInfo_ = nullptr;
   const clap_host_state *         hostState_ = nullptr;
   const clap_host_note_name *     hostNoteName_ = nullptr;

   // state
   bool isActive_ = false;
   bool isProcessing_ = false;
   int  sampleRate_ = 0;
};