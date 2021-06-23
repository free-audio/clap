#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_set>

#include <QDebug>

#include "application.hh"
#include "engine.hh"
#include "main-window.hh"
#include "plugin-host.hh"

enum ThreadType {
   Unknown,
   MainThread,
   AudioThread,
   AudioThreadPool,
};

thread_local ThreadType g_thread_type = Unknown;

PluginHost::PluginHost(Engine &engine) : QObject(&engine), engine_(engine) {
   g_thread_type = MainThread;

   host_.host_data = this;
   host_.clap_version = CLAP_VERSION;
   host_.extension = PluginHost::clapExtension;
   host_.name = "Clap Test Host";
   host_.version = "0.0.1";
   host_.vendor = "clap";
   host_.url = "https://github.com/free-audio/clap";

   hostLog_.log = PluginHost::clapLog;

   hostGui_.resize = PluginHost::clapGuiResize;

   hostThreadCheck_.is_main_thread = PluginHost::clapIsMainThread;
   hostThreadCheck_.is_audio_thread = PluginHost::clapIsAudioThread;

   hostThreadPool_.request_exec = PluginHost::clapThreadPoolRequestExec;

   hostEventLoop_.register_timer = PluginHost::clapEventLoopRegisterTimer;
   hostEventLoop_.unregister_timer = PluginHost::clapEventLoopUnregisterTimer;
   hostEventLoop_.register_fd = PluginHost::clapEventLoopRegisterFd;
   hostEventLoop_.modify_fd = PluginHost::clapEventLoopModifyFd;
   hostEventLoop_.unregister_fd = PluginHost::clapEventLoopUnregisterFd;

   hostParams_.rescan = PluginHost::clapParamsRescan;

   hostQuickControls_.pages_changed = PluginHost::clapQuickControlsPagesChanged;
   hostQuickControls_.selected_page_changed = PluginHost::clapQuickControlsSelectedPageChanged;

   hostState_.mark_dirty = PluginHost::clapStateMarkDirty;

   initThreadPool();
}

PluginHost::~PluginHost() {
   checkForMainThread();

   terminateThreadPool();
}

void PluginHost::initThreadPool() {
   checkForMainThread();

   threadPoolStop_ = false;
   threadPoolTaskIndex_ = 0;
   auto N = QThread::idealThreadCount();
   threadPool_.resize(N);
   for (int i = 0; i < N; ++i) {
      threadPool_[i].reset(QThread::create(&PluginHost::threadPoolEntry, this));
      threadPool_[i]->start(QThread::HighestPriority);
   }
}

void PluginHost::terminateThreadPool() {
   checkForMainThread();

   threadPoolStop_ = true;
   threadPoolSemaphoreProd_.release(threadPool_.size());
   for (auto &thr : threadPool_)
      if (thr)
         thr->wait();
}

void PluginHost::threadPoolEntry() {
   g_thread_type = AudioThreadPool;
   while (true) {
      threadPoolSemaphoreProd_.acquire();
      if (threadPoolStop_)
         return;

      int taskIndex = threadPoolTaskIndex_++;
      pluginThreadPool_->exec(plugin_, taskIndex);
      threadPoolSemaphoreDone_.release();
   }
}

bool PluginHost::load(const QString &path, int pluginIndex) {
   checkForMainThread();

   if (library_.isLoaded())
      unload();

   library_.setFileName(path);
   library_.setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::DeepBindHint);
   if (!library_.load()) {
      QString err = library_.errorString();
      qWarning() << "failed to load " << path << ": " << err;
      return false;
   }

   pluginEntry_ =
      reinterpret_cast<const struct clap_plugin_entry *>(library_.resolve("clap_plugin_entry"));
   if (!pluginEntry_) {
      library_.unload();
      return false;
   }

   pluginEntry_->init(path.toStdString().c_str());

   auto count = pluginEntry_->get_plugin_count();
   if (pluginIndex > count) {
      qWarning() << "plugin index greater than count :" << count;
      return false;
   }

   auto desc = pluginEntry_->get_plugin_descriptor(pluginIndex);
   if (!desc) {
      qWarning() << "no plugin descriptor";
      return false;
   }

   if (desc->clap_version != CLAP_VERSION) {
      qWarning() << "incompatible clap version: " << CLAP_VERSION_MAJ(desc->clap_version) << "."
                 << CLAP_VERSION_MIN(desc->clap_version) << "."
                 << CLAP_VERSION_REV(desc->clap_version);
      return false;
   }

   plugin_ = pluginEntry_->create_plugin(&host_, desc->id);
   if (!plugin_) {
      qWarning() << "could not create the plugin with id: " << desc->id;
      return false;
   }
   plugin_->init(plugin_);

   initPluginExtensions();
   scanParams();
   scanQuickControls();
   return true;
}

void PluginHost::initPluginExtensions() {
   if (pluginExtensionsAreInitialized_)
      return;

   initPluginExtension(pluginParams_, CLAP_EXT_PARAMS);
   initPluginExtension(pluginQuickControls_, CLAP_EXT_QUICK_CONTROLS);
   initPluginExtension(pluginAudioPorts_, CLAP_EXT_AUDIO_PORTS);
   initPluginExtension(pluginGui_, CLAP_EXT_GUI);
   initPluginExtension(pluginGuiX11_, CLAP_EXT_GUI_X11);
   initPluginExtension(pluginGuiWin32_, CLAP_EXT_GUI_WIN32);
   initPluginExtension(pluginGuiCocoa_, CLAP_EXT_GUI_COCOA);
   initPluginExtension(pluginGuiFreeStanding_, CLAP_EXT_GUI_FREE_STANDING);
   initPluginExtension(pluginEventLoop_, CLAP_EXT_EVENT_LOOP);
   initPluginExtension(pluginThreadPool_, CLAP_EXT_THREAD_POOL);
   initPluginExtension(pluginPresetLoad_, CLAP_EXT_PRESET_LOAD);
   initPluginExtension(pluginState_, CLAP_EXT_STATE);

   pluginExtensionsAreInitialized_ = true;
}

void PluginHost::unload() {
   checkForMainThread();

   if (!library_.isLoaded())
      return;

   if (pluginGui_)
      pluginGui_->close(plugin_);

   if (isPluginActive()) {
      plugin_->deactivate(plugin_);
      setPluginState(Inactive);
   }

   plugin_->destroy(plugin_);
   plugin_ = nullptr;
   pluginGui_ = nullptr;
   pluginAudioPorts_ = nullptr;

   pluginEntry_->deinit();
   pluginEntry_ = nullptr;

   library_.unload();
}

bool PluginHost::canActivate() const {
   if (!engine_.isRunning())
      return false;
   if (isPluginActive())
      return false;
   if (scheduleDeactivateForParameterScan_)
      return false;
   return true;
}

void PluginHost::activate(int32_t sample_rate) {
   if (plugin_->activate(plugin_, sample_rate))
      setPluginState(ActiveAndSleeping);
   else
      setPluginState(InactiveWithError);
}

void PluginHost::setPorts(int numInputs, float **inputs, int numOutputs, float **outputs) {
   audioIn_.channel_count = numInputs;
   audioIn_.data32 = inputs;
   audioIn_.data64 = nullptr;
   audioIn_.constant_mask = 0;
   audioIn_.latency = 0;

   audioOut_.channel_count = numOutputs;
   audioOut_.data32 = outputs;
   audioOut_.data64 = nullptr;
   audioOut_.constant_mask = 0;
   audioOut_.latency = 0;
}

void PluginHost::setParentWindow(WId parentWindow) {
   checkForMainThread();

#if defined(Q_OS_LINUX)
   if (pluginGuiX11_)
      pluginGuiX11_->attach(plugin_, nullptr, parentWindow);
#elif defined(Q_OS_MACX)
   if (pluginEmbedCocoa_)
      pluginGuiCocoa_->attach(plugin_, (void *)parentWindow);
#elif defined(Q_OS_WIN32)
   if (pluginEmbedWin32_)
      pluginGuiWin32_->attach(plugin_, parentWindow);
#endif
   // else (pluginGuiFreeStanding_)
   //   pluginGuiFreeStanding_->open(plugin_);

   int width = 0;
   int height = 0;

   if (pluginGui_)
      pluginGui_->size(plugin_, &width, &height);

   Application::instance().mainWindow()->resizePluginView(width, height);
}

void PluginHost::clapLog(const clap_host *host, clap_log_severity severity, const char *msg) {
   switch (severity) {
   case CLAP_LOG_DEBUG:
      qDebug() << msg;
      break;

   case CLAP_LOG_INFO:
      qInfo() << msg;
      break;

   case CLAP_LOG_WARNING:
   case CLAP_LOG_ERROR:
   case CLAP_LOG_FATAL:
   case CLAP_LOG_HOST_MISBEHAVING:
      qWarning() << msg;
      break;
   }
}

template <typename T>
void PluginHost::initPluginExtension(const T *&ext, const char *id) {
   if (ext)
      return;

   checkForMainThread();
   ext = static_cast<const T *>(plugin_->extension(plugin_, id));
}

const void *PluginHost::clapExtension(const clap_host *host, const char *extension) {
   checkForMainThread();

   PluginHost *h = static_cast<PluginHost *>(host->host_data);
   if (!h->plugin_)
      throw std::logic_error("The plugin can't query for extensions during the create method. Wait "
                             "for clap_plugin.init() call.");

   if (!strcmp(extension, CLAP_EXT_GUI))
      return &h->hostGui_;
   if (!strcmp(extension, CLAP_EXT_LOG))
      return &h->hostLog_;
   if (!strcmp(extension, CLAP_EXT_THREAD_CHECK))
      return &h->hostThreadCheck_;
   if (!strcmp(extension, CLAP_EXT_EVENT_LOOP))
      return &h->hostEventLoop_;
   if (!strcmp(extension, CLAP_EXT_PARAMS))
      return &h->hostParams_;
   if (!strcmp(extension, CLAP_EXT_QUICK_CONTROLS))
      return &h->hostQuickControls_;
   if (!strcmp(extension, CLAP_EXT_STATE))
      return &h->hostState_;
   return nullptr;
}

PluginHost *PluginHost::fromHost(const clap_host *host) {
   if (!host)
      throw std::invalid_argument("Passed a null host pointer");

   auto h = static_cast<PluginHost *>(host->host_data);
   if (!h)
      throw std::invalid_argument("Passed an invalid host pointer because the host_data is null");

   if (!h->plugin_)
      throw std::logic_error(
         "Called into host interfaces befores the host knows the plugin pointer");

   return h;
}

bool PluginHost::clapIsMainThread(const clap_host *host) { return g_thread_type == MainThread; }

bool PluginHost::clapIsAudioThread(const clap_host *host) { return g_thread_type == AudioThread; }

void PluginHost::checkForMainThread() {
   if (g_thread_type != MainThread)
      throw std::logic_error("Requires Main Thread!");
}

void PluginHost::checkForAudioThread() {
   if (g_thread_type != AudioThread)
      throw std::logic_error("Requires Audio Thread!");
}

bool PluginHost::clapThreadPoolRequestExec(const clap_host *host, uint32_t num_tasks) {
   checkForAudioThread();

   auto h = fromHost(host);
   if (!h->pluginThreadPool_ || !h->pluginThreadPool_->exec)
      throw std::logic_error("Called request_exec() without providing clap_plugin_thread_pool to "
                             "execute the job.");

   Q_ASSERT(!h->threadPoolStop_);
   Q_ASSERT(!h->threadPool_.empty());
   h->threadPoolTaskIndex_ = 0;
   h->threadPoolSemaphoreProd_.release(num_tasks);
   h->threadPoolSemaphoreDone_.acquire(num_tasks);
   return true;
}

bool PluginHost::clapEventLoopRegisterTimer(const clap_host *host,
                                            uint32_t period_ms,
                                            clap_id *timer_id) {
   checkForMainThread();

   auto h = fromHost(host);
   h->initPluginExtensions();
   if (!h->pluginEventLoop_ || !h->pluginEventLoop_->on_timer)
      throw std::logic_error("Called register_timer() without providing clap_plugin_event_loop to "
                             "receive the timer event.");

   auto id = h->nextTimerId_++;
   *timer_id = id;
   auto timer = std::make_unique<QTimer>();

   QObject::connect(timer.get(), &QTimer::timeout, [h, id] {
      checkForMainThread();
      h->pluginEventLoop_->on_timer(h->plugin_, id);
   });

   auto t = timer.get();
   h->timers_.emplace(*timer_id, std::move(timer));
   t->start(period_ms);
   return true;
}

bool PluginHost::clapEventLoopUnregisterTimer(const clap_host *host, clap_id timer_id) {
   checkForMainThread();

   auto h = fromHost(host);
   if (!h->pluginEventLoop_ || !h->pluginEventLoop_->on_timer)
      throw std::logic_error(
         "Called unregister_timer() without providing clap_plugin_event_loop to "
         "receive the timer event.");

   auto it = h->timers_.find(timer_id);
   if (it == h->timers_.end())
      throw std::logic_error("Called unregister_timer() for a timer_id that was not registered.");

   h->timers_.erase(it);
   return true;
}

bool PluginHost::clapEventLoopRegisterFd(const clap_host *host, clap_fd fd, uint32_t flags) {
   checkForMainThread();

   auto h = fromHost(host);
   h->initPluginExtensions();
   if (!h->pluginEventLoop_ || !h->pluginEventLoop_->on_fd)
      throw std::logic_error(
         "Called unregister_timer() without providing clap_plugin_event_loop to "
         "receive the timer event.");

   auto it = h->fds_.find(fd);
   if (it != h->fds_.end())
      throw std::logic_error(
         "Called register_fd() for a fd that was already registered, use modify_fd() instead.");

   h->fds_.emplace(fd, std::make_unique<Notifiers>());
   h->eventLoopSetFdNotifierFlags(fd, flags);
   return true;
}

bool PluginHost::clapEventLoopModifyFd(const clap_host *host, clap_fd fd, uint32_t flags) {
   checkForMainThread();

   auto h = fromHost(host);
   if (!h->pluginEventLoop_ || !h->pluginEventLoop_->on_fd)
      throw std::logic_error(
         "Called unregister_timer() without providing clap_plugin_event_loop to "
         "receive the timer event.");

   auto it = h->fds_.find(fd);
   if (it == h->fds_.end())
      throw std::logic_error(
         "Called modify_fd() for a fd that was not registered, use register_fd() instead.");

   h->fds_.emplace(fd, std::make_unique<Notifiers>());
   h->eventLoopSetFdNotifierFlags(fd, flags);
   return true;
}

bool PluginHost::clapEventLoopUnregisterFd(const clap_host *host, clap_fd fd) {
   checkForMainThread();

   auto h = fromHost(host);
   if (!h->pluginEventLoop_ || !h->pluginEventLoop_->on_fd)
      throw std::logic_error(
         "Called unregister_timer() without providing clap_plugin_event_loop to "
         "receive the timer event.");

   auto it = h->fds_.find(fd);
   if (it == h->fds_.end())
      throw std::logic_error("Called unregister_fd() for a fd that was not registered.");

   h->fds_.erase(it);
   return true;
}

void PluginHost::eventLoopSetFdNotifierFlags(clap_fd fd, uint32_t flags) {
   checkForMainThread();

   auto it = fds_.find(fd);
   Q_ASSERT(it != fds_.end());

   if (flags & CLAP_FD_READ) {
      if (!it->second->rd) {
         it->second->rd.reset(new QSocketNotifier(fd, QSocketNotifier::Read));
         QObject::connect(it->second->rd.get(), &QSocketNotifier::activated, [this, fd] {
            checkForMainThread();
            this->pluginEventLoop_->on_fd(this->plugin_, fd, CLAP_FD_READ);
         });
      }
      it->second->rd->setEnabled(true);
   } else if (it->second->rd)
      it->second->rd->setEnabled(false);

   if (flags & CLAP_FD_WRITE) {
      if (!it->second->wr) {
         it->second->wr.reset(new QSocketNotifier(fd, QSocketNotifier::Write));
         QObject::connect(it->second->wr.get(), &QSocketNotifier::activated, [this, fd] {
            checkForMainThread();
            this->pluginEventLoop_->on_fd(this->plugin_, fd, CLAP_FD_WRITE);
         });
      }
      it->second->wr->setEnabled(true);
   } else if (it->second->wr)
      it->second->wr->setEnabled(false);

   if (flags & CLAP_FD_ERROR) {
      if (!it->second->err) {
         it->second->err.reset(new QSocketNotifier(fd, QSocketNotifier::Exception));
         QObject::connect(it->second->err.get(), &QSocketNotifier::activated, [this, fd] {
            checkForMainThread();
            this->pluginEventLoop_->on_fd(this->plugin_, fd, CLAP_FD_ERROR);
         });
      }
      it->second->err->setEnabled(true);
   } else if (it->second->err)
      it->second->err->setEnabled(false);
}

bool PluginHost::clapGuiResize(const clap_host *host, int32_t width, int32_t height) {
   checkForMainThread();

   PluginHost *h = static_cast<PluginHost *>(host->host_data);

   Application::instance().mainWindow()->resizePluginView(width, height);
   return true;
}

void PluginHost::processInit(int nframes) {
   process_.frames_count = nframes;
   process_.steady_time = engine_.steadyTime_;
}

void PluginHost::processNoteOn(int sampleOffset, int channel, int key, int velocity) {
   clap_event ev;

   ev.type = CLAP_EVENT_NOTE_ON;
   ev.time = sampleOffset;
   ev.note.key = key;
   ev.note.channel = channel;
   ev.note.velocity = velocity / 127.0;

   evIn_.push_back(ev);
}

void PluginHost::processNoteOff(int sampleOffset, int channel, int key, int velocity) {
   clap_event ev;

   ev.type = CLAP_EVENT_NOTE_OFF;
   ev.time = sampleOffset;
   ev.note.key = key;
   ev.note.channel = channel;
   ev.note.velocity = velocity / 127.0;

   evIn_.push_back(ev);
}

void PluginHost::processNoteAt(int sampleOffset, int channel, int key, int pressure) {
   // TODO
}

void PluginHost::processPitchBend(int sampleOffset, int channel, int value) {
   // TODO
}

void PluginHost::processCC(int sampleOffset, int channel, int cc, int value) {
   clap_event ev;

   ev.type = CLAP_EVENT_MIDI;
   ev.time = sampleOffset;
   ev.midi.data[0] = 0;
   ev.midi.data[1] = 0xB0 | channel;
   ev.midi.data[2] = cc;
   ev.midi.data[3] = value;

   evIn_.push_back(ev);
}

static uint32_t clap_host_event_list_size(const struct clap_event_list *list) {
   PluginHost::checkForAudioThread();

   auto vec = reinterpret_cast<std::vector<clap_event> *>(list->ctx);
   return vec->size();
}

const struct clap_event *clap_host_event_list_get(const struct clap_event_list *list,
                                                  uint32_t index) {
   PluginHost::checkForAudioThread();

   auto vec = reinterpret_cast<std::vector<clap_event> *>(list->ctx);
   if (index < 0 || index >= vec->size())
      return nullptr;
   return vec->data() + index;
}

// Makes a copy of the event
void clap_host_event_list_push_back(const struct clap_event_list *list,
                                    const struct clap_event *event) {
   PluginHost::checkForAudioThread();

   auto vec = reinterpret_cast<std::vector<clap_event> *>(list->ctx);
   vec->push_back(*event);
}

void PluginHost::process() {
   g_thread_type = AudioThread;

   if (!isPluginActive())
      return;

   process_.transport = nullptr;

   clap_event_list in_ev = {
      &evIn_, clap_host_event_list_size, clap_host_event_list_get, clap_host_event_list_push_back};

   clap_event_list out_ev = {
      &evOut_, clap_host_event_list_size, clap_host_event_list_get, clap_host_event_list_push_back};

   process_.in_events = &in_ev;
   process_.out_events = &out_ev;

   process_.audio_inputs = &audioIn_;
   process_.audio_inputs_count = 1;
   process_.audio_outputs = &audioOut_;
   process_.audio_outputs_count = 1;

   evOut_.clear();
   appToEngineQueue_.consume([this](clap_id param_id, double value) {
      clap_event ev;
      ev.time = 0;
      ev.type = CLAP_EVENT_PARAM_SET;
      ev.param.param_id = param_id;
      ev.param.key = -1;
      ev.param.channel = -1;
      ev.param.val0 = value;
      ev.param.val1 = value;
      ev.param.mod0 = 0;
      ev.param.mod1 = 0;
      ev.param.distance = process_.frames_count;
      evIn_.push_back(ev);
   });

   // TODO if the plugin was not processing and had audio or events that should
   // wake it, then we should set it as processing
   if (!isPluginProcessing()) {
      plugin_->start_processing(plugin_);
      setPluginState(ActiveAndProcessing);
   }

   int32_t status;
   if (plugin_ && plugin_->process)
      status = plugin_->process(plugin_, &process_);

   for (auto &ev : evOut_) {
      switch (ev.type) {
      case CLAP_EVENT_PARAM_SET:
         engineToAppQueue_.set(ev.param.param_id, ev.param.val0);
         break;
      }
   }
   evOut_.clear();
   evIn_.clear();

   if (scheduleDeactivateForParameterScan_) {
      plugin_->stop_processing(plugin_);
      setPluginState(ActiveAndReadyToDeactivate);
   }

   engineToAppQueue_.producerDone();
   g_thread_type = Unknown;
}

void PluginHost::idle() {
   checkForMainThread();

   // Try to send events to the audio engine
   appToEngineQueue_.producerDone();
   engineToAppQueue_.consume([this](clap_id param_id, double value) {
      auto it = params_.find(param_id);
      if (it == params_.end()) {
         std::ostringstream msg;
         msg << "Plugin produced a CLAP_EVENT_PARAM_SET with an unknown param_id: " << param_id;
         throw std::invalid_argument(msg.str());
      }

      it->second->setValue(value);
   });
}

PluginParam &PluginHost::checkValidParamId(const std::string_view &function,
                                           const std::string_view &param_name,
                                           clap_id param_id) {
   checkForMainThread();

   if (param_id == CLAP_INVALID_ID) {
      std::ostringstream msg;
      msg << "Plugin called " << function << " with " << param_name << " == CLAP_INVALID_ID";
      throw std::invalid_argument(msg.str());
   }

   auto it = params_.find(param_id);
   if (it == params_.end()) {
      std::ostringstream msg;
      msg << "Plugin called " << function << " with  an invalid " << param_name
          << " == " << param_id;
      throw std::invalid_argument(msg.str());
   }

   Q_ASSERT(it->first == param_id);
   Q_ASSERT(it->second->info().id == param_id);
   return *it->second;
}

void PluginHost::checkValidParamValue(const PluginParam &param, double value) {
   checkForMainThread();
   if (!param.isValueValid(value)) {
      std::ostringstream msg;
      msg << "Invalid value for param. ";
      param.printInfo(msg);
      msg << "; value: " << value;
      // std::cerr << msg.str() << std::endl;
      throw std::invalid_argument(msg.str());
   }
}

void PluginHost::setParamValueByHost(PluginParam &param, double value) {
   checkForMainThread();

   param.setValue(value);

   appToEngineQueue_.set(param.info().id, value);
   appToEngineQueue_.producerDone();
}

void PluginHost::scanParams() { clapParamsRescan(&host_, CLAP_PARAM_RESCAN_ALL); }

void PluginHost::clapParamsRescan(const clap_host *host, uint32_t flags) {
   checkForMainThread();
   auto h = fromHost(host);

   // 1. if the plugin is activated, check if we need to deactivate it
   if (h->isPluginActive() && (flags & CLAP_PARAM_RESCAN_ALL)) {
      h->scheduleDeactivateForParameterScan_ = true;
      h->scheduleParamsRescanFlags_ |= flags;
      return;
   }

   // 2. scan the params.
   auto count = h->pluginParams_->count(h->plugin_);
   std::unordered_set<clap_id> paramIds(count * 2);

   for (int32_t i = 0; i < count; ++i) {
      clap_param_info info;
      if (!h->pluginParams_->info(h->plugin_, i, &info))
         throw std::logic_error("clap_plugin_params.get_info did return false!");

      if (info.id == CLAP_INVALID_ID) {
         std::ostringstream msg;
         msg << "clap_plugin_params.get_info() reported a parameter with id = CLAP_INVALID_ID"
             << std::endl
             << " 2. name: " << info.name << ", module: " << info.module << std::endl;
         throw std::logic_error(msg.str());
      }

      auto it = h->params_.find(info.id);

      // check that the parameter is not declared twice
      if (paramIds.count(info.id) > 0) {
         Q_ASSERT(it != h->params_.end());

         std::ostringstream msg;
         msg << "the parameter with id: " << info.id << " was declared twice." << std::endl
             << " 1. name: " << it->second->info().name << ", module: " << it->second->info().module
             << std::endl
             << " 2. name: " << info.name << ", module: " << info.module << std::endl;
         throw std::logic_error(msg.str());
      }
      paramIds.insert(info.id);

      if (it == h->params_.end()) {
         if (!(flags & CLAP_PARAM_RESCAN_ALL)) {
            std::ostringstream msg;
            msg << "a new parameter was declared, but the flag CLAP_PARAM_RESCAN_ALL was not "
                   "specified; id: "
                << info.id << ", name: " << info.name << ", module: " << info.module << std::endl;
            throw std::logic_error(msg.str());
         }

         double value = h->getParamValue(info);
         auto param = std::make_unique<PluginParam>(*h, info, value);
         h->checkValidParamValue(*param, value);
         h->params_.emplace(info.id, std::move(param));
      } else {
         // update param info
         if (!it->second->isInfoEqualTo(info)) {
            if (!clapParamsRescanMayInfoChange(flags)) {
               std::ostringstream msg;
               msg << "a parameter's info did change, but the flag CLAP_PARAM_RESCAN_INFO "
                      "was not specified; id: "
                   << info.id << ", name: " << info.name << ", module: " << info.module
                   << std::endl;
               throw std::logic_error(msg.str());
            }

            if (!(flags & CLAP_PARAM_RESCAN_ALL) &&
                !it->second->isInfoCriticallyDifferentTo(info)) {
               std::ostringstream msg;
               msg << "a parameter's info has critical changes, but the flag CLAP_PARAM_RESCAN_ALL "
                      "was not specified; id: "
                   << info.id << ", name: " << info.name << ", module: " << info.module
                   << std::endl;
               throw std::logic_error(msg.str());
            }

            it->second->setInfo(info);
         }

         double value = h->getParamValue(info);
         if (it->second->value() != value) {
            if (!clapParamsRescanMayValueChange(flags)) {
               std::ostringstream msg;
               msg << "a parameter's value did change but, but the flag CLAP_PARAM_RESCAN_VALUES "
                      "was not specified; id: "
                   << info.id << ", name: " << info.name << ", module: " << info.module
                   << std::endl;
               throw std::logic_error(msg.str());
            }

            // update param value
            h->checkValidParamValue(*it->second, value);
            it->second->setValue(value);
            it->second->setModulatedValue(value);
         }
      }
   }

   // remove parameters which are gone
   for (auto it = h->params_.begin(); it != h->params_.end();) {
      if (paramIds.find(it->first) != paramIds.end())
         ++it;
      else {
         if (!(flags & CLAP_PARAM_RESCAN_ALL)) {
            std::ostringstream msg;
            auto &info = it->second->info();
            msg << "a parameter was removed, but the flag CLAP_PARAM_RESCAN_ALL was not "
                   "specified; id: "
                << info.id << ", name: " << info.name << ", module: " << info.module << std::endl;
            throw std::logic_error(msg.str());
         }
         it = h->params_.erase(it);
      }
   }

   if (flags & CLAP_PARAM_RESCAN_ALL) {
      h->scheduleDeactivateForParameterScan_ = false;
      h->scheduleParamsRescanFlags_ = 0;

      if (h->canActivate())
         h->plugin_->activate(h->plugin_, h->engine_.sampleRate());

      h->paramsChanged();
   }
}

double PluginHost::getParamValue(const clap_param_info &info) {
   double value;
   if (pluginParams_->value(plugin_, info.id, &value))
      return value;

   std::ostringstream msg;
   msg << "failed to get the param value, id: " << info.id << ", name: " << info.name
       << ", module: " << info.module;
   throw std::logic_error(msg.str());
}

void PluginHost::scanQuickControls() {
   checkForMainThread();

   if (!pluginQuickControls_)
      return;

   if (!pluginQuickControls_->page_info || !pluginQuickControls_->page_count) {
      std::ostringstream msg;
      msg << "clap_plugin_quick_controls is partially implemented.";
      throw std::logic_error(msg.str());
   }

   quickControlsSetSelectedPage(CLAP_INVALID_ID);
   quickControlsPages_.clear();

   const auto N = pluginQuickControls_->page_count(plugin_);
   if (N == 0)
      return;

   quickControlsPages_.reserve(N);

   clap_id firstPageId = CLAP_INVALID_ID;
   for (int i = 0; i < N; ++i) {
      auto page = std::make_unique<clap_quick_controls_page>();
      if (!pluginQuickControls_->page_info(plugin_, i, page.get())) {
         std::ostringstream msg;
         msg << "clap_plugin_quick_controls.get_page(" << i << ") failed, while the page count is "
             << N;
         throw std::logic_error(msg.str());
      }

      if (page->id == CLAP_INVALID_ID) {
         std::ostringstream msg;
         msg << "clap_plugin_quick_controls.get_page(" << i << ") gave an invalid page_id";
         throw std::invalid_argument(msg.str());
      }

      if (i == 0)
         firstPageId = page->id;

      auto it = quickControlsPages_.find(page->id);
      if (it != quickControlsPages_.end()) {
         std::ostringstream msg;
         msg << "clap_plugin_quick_controls.get_page(" << i
             << ") gave twice the same page_id:" << page->id << std::endl
             << " 1. name: " << it->second->name << std::endl
             << " 2. name: " << page->name;
         throw std::invalid_argument(msg.str());
      }

      quickControlsPages_.emplace(page->id, std::move(page));
   }

   quickControlsPagesChanged();

   auto pageId = pluginQuickControls_->selected_page(plugin_);
   quickControlsSetSelectedPage(pageId == CLAP_INVALID_ID ? firstPageId : pageId);
}

void PluginHost::quickControlsSetSelectedPage(clap_id pageId) {
   if (pageId == quickControlsSelectedPage_)
      return;

   if (pageId != CLAP_INVALID_ID) {
      auto it = quickControlsPages_.find(pageId);
      if (it == quickControlsPages_.end()) {
         std::ostringstream msg;
         msg << "quick control page_id " << pageId << " not found";
         throw std::invalid_argument(msg.str());
      }
   }

   quickControlsSelectedPage_ = pageId;
   quickControlsSelectedPageChanged();
}

void PluginHost::setQuickControlsSelectedPageByHost(clap_id page_id) {
   Q_ASSERT(page_id != CLAP_INVALID_ID);

   checkForMainThread();

   quickControlsSelectedPage_ = page_id;

   if (pluginQuickControls_ && pluginQuickControls_->select_page)
      pluginQuickControls_->select_page(plugin_, page_id);
}

void PluginHost::clapQuickControlsPagesChanged(const clap_host *host) {
   checkForMainThread();

   auto h = fromHost(host);
   if (!h->pluginQuickControls_) {
      std::ostringstream msg;
      msg << "Plugin called clap_host_quick_controls.pages_changed() but does not provide "
             "clap_plugin_quick_controls";
      throw std::logic_error(msg.str());
   }
   h->scanQuickControls();
}

void PluginHost::clapQuickControlsSelectedPageChanged(const clap_host *host, clap_id page_id) {
   checkForMainThread();

   auto h = fromHost(host);
   if (!h->pluginQuickControls_) {
      std::ostringstream msg;
      msg << "Plugin called clap_host_quick_controls.selected_page_changed() but does not provide "
             "clap_plugin_quick_controls";
      throw std::logic_error(msg.str());
   }
   h->quickControlsSetSelectedPage(page_id);
}

bool PluginHost::loadNativePluginPreset(const std::string &path) {
   checkForMainThread();

   if (!pluginPresetLoad_)
      return false;

   if (!pluginPresetLoad_->from_file)
      throw std::logic_error("clap_plugin_preset_load does not implement load_from_file");

   return pluginPresetLoad_->from_file(plugin_, path.c_str());
}

void PluginHost::clapStateMarkDirty(const clap_host *host) {
   checkForMainThread();

   auto h = fromHost(host);

   if (!h->pluginState_ || !h->pluginState_->save || !h->pluginState_->load)
      throw std::logic_error("Plugin called clap_host_state.set_dirty() but the host does not "
                             "provide a complete clap_plugin_state interface.");

   h->stateIsDirty_ = true;
}

void PluginHost::setPluginState(PluginState state) {
   switch (state) {
   case Inactive:
      Q_ASSERT(state_ == ActiveAndReadyToDeactivate);
      break;

   case InactiveWithError:
      Q_ASSERT(state_ == Inactive);
      break;

   case ActiveAndSleeping:
      Q_ASSERT(state_ == Inactive || state_ == ActiveAndProcessing);
      break;

   case ActiveAndProcessing:
      Q_ASSERT(state_ == ActiveAndSleeping);
      break;

   case ActiveWithError:
      Q_ASSERT(state_ == ActiveAndProcessing);
      break;

   case ActiveAndReadyToDeactivate:
      Q_ASSERT(state_ == ActiveAndSleeping || state_ == ActiveWithError);
      break;

   default:
      std::terminate();
   }

   state_ = state;
}

bool PluginHost::isPluginActive() const {
   switch (state_) {
   case Inactive:
   case InactiveWithError:
      return false;
   default:
      return true;
   }
}

bool PluginHost::isPluginProcessing() const { return state_ == ActiveAndProcessing; }