#pragma once

#include <array>
#include <memory>

#include <QLibrary>
#include <QString>
#include <QTimer>
#include <QWidget>

#include <portaudio.h>
#include <portmidi.h>
#include <porttime.h>

class Application;
class Settings;
class PluginHost;

class Engine : public QObject {
   Q_OBJECT

public:
   Engine(Application &application);
   ~Engine();

   enum State {
      kStateStopped,
      kStateRunning,
      kStateStopping,
   };

   void setParentWindow(WId parentWindow) { parentWindow_ = parentWindow; }
   void start();
   void stop();

   bool loadPlugin(const QString &path, int plugin_index);
   void unloadPlugin();

   /* send events to the plugin from GUI */
   void setProgram(int8_t program, int8_t bank_msb, int8_t bank_lsb);
   void loadMidiFile(const QString &path);

   bool isRunning() const noexcept { return state_ == kStateRunning; }
   int  sampleRate() const noexcept { return sampleRate_; }

   PluginHost &pluginHost() const { return *pluginHost_; }

public:
   void callPluginIdle();

private:
   friend class AudioPlugin;
   friend class PluginHost;
   friend class Vst3Plugin;

   static int audioCallback(const void *                    input,
                            void *                          output,
                            unsigned long                   frameCount,
                            const PaStreamCallbackTimeInfo *timeInfo,
                            PaStreamCallbackFlags           statusFlags,
                            void *                          userData);

   Application &application_;
   Settings &   settings_;
   WId          parentWindow_;

   State state_ = kStateStopped;

   /* audio & midi streams */
   PaStream *audio_ = nullptr;
   PmStream *midi_ = nullptr;

   /* engine context */
   int64_t steadyTime_ = 0;
   int32_t sampleRate_ = 44100;
   int32_t nframes_ = 0;

   /* audio buffers */
   float *inputs_[2] = {nullptr, nullptr};
   float *outputs_[2] = {nullptr, nullptr};

   std::unique_ptr<PluginHost> pluginHost_;

   QTimer idleTimer_;
};
