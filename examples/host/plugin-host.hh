#pragma once

#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <QLibrary>
#include <QSemaphore>
#include <QSocketNotifier>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include <clap/all.h>

#include "../common/param-queue.hh"
#include "engine.hh"
#include "plugin-param.hh"

class Engine;
class PluginHost final : public QObject {
   Q_OBJECT;

public:
   PluginHost(Engine &engine);
   ~PluginHost();

   bool load(const QString &path, int pluginIndex);
   void unload();

   bool canActivate() const;
   void activate(int32_t sample_rate);
   void deactivate();

   void recreatePluginWindow();
   void setPluginWindowVisibility(bool isVisible);

   void setPorts(int numInputs, float **inputs, int numOutputs, float **outputs);
   void setParentWindow(WId parentWindow);

   void processBegin(int nframes);
   void processNoteOn(int sampleOffset, int channel, int key, int velocity);
   void processNoteOff(int sampleOffset, int channel, int key, int velocity);
   void processNoteAt(int sampleOffset, int channel, int key, int pressure);
   void processPitchBend(int sampleOffset, int channel, int value);
   void processCC(int sampleOffset, int channel, int cc, int value);
   void process();
   void processEnd(int nframes);

   void idle();

   void initPluginExtensions();
   void initThreadPool();
   void terminateThreadPool();
   void threadPoolEntry();

   void setParamValueByHost(PluginParam &param, double value);
   void setParamModulationByHost(PluginParam &param, double value);

   auto &params() const { return params_; }
   auto &quickControlsPages() const { return quickControlsPages_; }
   auto quickControlsSelectedPage() const { return quickControlsSelectedPage_; }
   void setQuickControlsSelectedPageByHost(clap_id page_id);

   bool loadNativePluginPreset(const std::string &path);
   bool loadStateFromFile(const std::string &path);
   bool saveStateToFile(const std::string &path);

   static void checkForMainThread();
   static void checkForAudioThread();

signals:
   void paramsChanged();
   void quickControlsPagesChanged();
   void quickControlsSelectedPageChanged();

private:
   static PluginHost *fromHost(const clap_host *host);
   template <typename T>
   void initPluginExtension(const T *&ext, const char *id);

   /* clap host callbacks */
   static void clapLog(const clap_host *host, clap_log_severity severity, const char *msg);

   static bool clapIsMainThread(const clap_host *host);
   static bool clapIsAudioThread(const clap_host *host);

   static void clapParamsRescan(const clap_host *host, uint32_t flags);
   static void clapParamsClear(const clap_host *host, uint32_t flags);
   void scanParams();
   void scanParam(int32_t index);
   PluginParam &checkValidParamId(const std::string_view &function,
                                  const std::string_view &param_name,
                                  clap_id param_id);
   void checkValidParamValue(const PluginParam &param, double value);
   double getParamValue(const clap_param_info &info);
   static bool clapParamsRescanMayValueChange(uint32_t flags) {
      return flags & (CLAP_PARAM_RESCAN_ALL | CLAP_PARAM_RESCAN_VALUES);
   }
   static bool clapParamsRescanMayInfoChange(uint32_t flags) {
      return flags & (CLAP_PARAM_RESCAN_ALL | CLAP_PARAM_RESCAN_INFO);
   }

   void scanQuickControls();
   void quickControlsSetSelectedPage(clap_id pageId);
   static void clapQuickControlsChanged(const clap_host *host, clap_quick_controls_changed_flags flags);

   static bool
   clapRegisterTimer(const clap_host *host, uint32_t period_ms, clap_id *timer_id);
   static bool clapUnregisterTimer(const clap_host *host, clap_id timer_id);
   static bool clapRegisterFd(const clap_host *host, clap_fd fd, uint32_t flags);
   static bool clapModifyFd(const clap_host *host, clap_fd fd, uint32_t flags);
   static bool clapUnregisterFd(const clap_host *host, clap_fd fd);
   void eventLoopSetFdNotifierFlags(clap_fd fd, uint32_t flags);

   static bool clapThreadPoolRequestExec(const clap_host *host, uint32_t num_tasks);

   static const void *clapExtension(const clap_host *host, const char *extension);

   /* clap host gui callbacks */
   static bool clapGuiResize(const clap_host *host, uint32_t width, uint32_t height);

   static void clapStateMarkDirty(const clap_host *host);

private:
   Engine &engine_;

   QLibrary library_;

   clap_host host_;
   static const constexpr clap_host_log hostLog_ = {
      PluginHost::clapLog,
   };
   static const constexpr clap_host_gui hostGui_ = {
      PluginHost::clapGuiResize,
   };
   //static const constexpr clap_host_audio_ports hostAudioPorts_;
   //static const constexpr clap_host_audio_ports_config hostAudioPortsConfig_;
   static const constexpr clap_host_params hostParams_ = {
PluginHost::clapParamsRescan,
   };
   static const constexpr clap_host_quick_controls hostQuickControls_ = {
      PluginHost::clapQuickControlsChanged,
   };
   static const constexpr clap_host_timer_support hostTimerSupport_ = {
      PluginHost::clapRegisterTimer,
      PluginHost::clapUnregisterTimer,
   };
   static const constexpr clap_host_fd_support hostFdSupport_ = {
      PluginHost::clapRegisterFd,
      PluginHost::clapModifyFd,
      PluginHost::clapUnregisterFd,
   };
   static const constexpr clap_host_thread_check hostThreadCheck_ = {
      PluginHost::clapIsMainThread,
      PluginHost::clapIsAudioThread,
   };
   static const constexpr clap_host_thread_pool hostThreadPool_ = {
      PluginHost::clapThreadPoolRequestExec,
   };
   static const constexpr clap_host_state hostState_ = {
      PluginHost::clapStateMarkDirty,
   };

   const struct clap_plugin_entry *pluginEntry_ = nullptr;
   const clap_plugin *plugin_ = nullptr;
   const clap_plugin_params *pluginParams_ = nullptr;
   const clap_plugin_quick_controls *pluginQuickControls_ = nullptr;
   const clap_plugin_audio_ports *pluginAudioPorts_ = nullptr;
   const clap_plugin_gui *pluginGui_ = nullptr;
   const clap_plugin_gui_x11 *pluginGuiX11_ = nullptr;
   const clap_plugin_gui_win32 *pluginGuiWin32_ = nullptr;
   const clap_plugin_gui_cocoa *pluginGuiCocoa_ = nullptr;
   const clap_plugin_gui_free_standing *pluginGuiFreeStanding_ = nullptr;
   const clap_plugin_timer_support *pluginTimerSupport_ = nullptr;
   const clap_plugin_fd_support *pluginFdSupport_ = nullptr;
   const clap_plugin_thread_pool *pluginThreadPool_ = nullptr;
   const clap_plugin_preset_load *pluginPresetLoad_ = nullptr;
   const clap_plugin_state *pluginState_ = nullptr;

   bool pluginExtensionsAreInitialized_ = false;

   /* timers */
   clap_id nextTimerId_ = 0;
   std::unordered_map<clap_id, std::unique_ptr<QTimer>> timers_;

   /* fd events */
   struct Notifiers {
      std::unique_ptr<QSocketNotifier> rd;
      std::unique_ptr<QSocketNotifier> wr;
      std::unique_ptr<QSocketNotifier> err;
   };
   std::unordered_map<clap_fd, std::unique_ptr<Notifiers>> fds_;

   /* thread pool */
   std::vector<std::unique_ptr<QThread>> threadPool_;
   std::atomic<bool> threadPoolStop_ = {false};
   std::atomic<int> threadPoolTaskIndex_ = {0};
   QSemaphore threadPoolSemaphoreProd_;
   QSemaphore threadPoolSemaphoreDone_;

   /* process stuff */
   clap_audio_buffer audioIn_ = {};
   clap_audio_buffer audioOut_ = {};
   std::vector<clap_event> evIn_;
   std::vector<clap_event> evOut_;
   clap_process process_;

   /* param update queues */
   std::unordered_map<clap_id, std::unique_ptr<PluginParam>> params_;

   struct ParamQueueValue {
      void *cookie;
      double value;
   };

   ParamQueue<ParamQueueValue> appToEngineValueQueue_;
   ParamQueue<ParamQueueValue> appToEngineModQueue_;
   ParamQueue<ParamQueueValue> engineToAppValueQueue_;

   std::unordered_map<clap_id, std::unique_ptr<clap_quick_controls_page>> quickControlsPages_;
   clap_id quickControlsSelectedPage_ = CLAP_INVALID_ID;

   /* delayed actions */
   enum PluginState {
      // The plugin is inactive, only the main thread uses it
      Inactive,

      // Activation failed
      InactiveWithError,

      // The plugin is active and sleeping, the audio engine can call set_processing()
      ActiveAndSleeping,

      // The plugin is processing
      ActiveAndProcessing,

      // The plugin did process but is in error
      ActiveWithError,

      // The plugin is not used anymore by the audio engine and can be deactivated on the main
      // thread
      ActiveAndReadyToDeactivate,
   };

   bool isPluginActive() const;
   bool isPluginProcessing() const;
   bool isPluginSleeping() const;
   void setPluginState(PluginState state);

   PluginState state_ = Inactive;
   bool stateIsDirty_ = false;

   bool scheduleRestart_ = false;
   bool scheduleDeactivate_ = false;

   bool isGuiCreated_ = false;
   bool isGuiVisible_ = false;
};
