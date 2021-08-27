#include <cassert>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QtGlobal>

#include "application.hh"
#include "engine.hh"
#include "main-window.hh"
#include "plugin-host.hh"
#include "settings.hh"

enum MidiStatus {
   MIDI_STATUS_NOTE_OFF = 0x8,
   MIDI_STATUS_NOTE_ON = 0x9,
   MIDI_STATUS_NOTE_AT = 0xA, // after touch
   MIDI_STATUS_CC = 0xB,      // control change
   MIDI_STATUS_PGM_CHANGE = 0xC,
   MIDI_STATUS_CHANNEL_AT = 0xD, // after touch
   MIDI_STATUS_PITCH_BEND = 0xE,
};

Engine::Engine(Application &application)
   : QObject(&application), application_(application), settings_(application.settings()),
     idleTimer_(this) {
   pluginHost_.reset(new PluginHost(*this));

   connect(&idleTimer_, &QTimer::timeout, this, QOverload<>::of(&Engine::callPluginIdle));
   idleTimer_.start(1000 / 30);
}

Engine::~Engine() {
   std::clog << "     ####### STOPING ENGINE #########" << std::endl;
   stop();
   unloadPlugin();
   std::clog << "     ####### ENGINE STOPPED #########" << std::endl;
}

void Engine::start() {
   assert(!audio_);
   assert(state_ == kStateStopped);

   auto &    as = settings_.audioSettings();
   const int bufferSize = 4 * 2 * as.bufferSize();

   inputs_[0] = (float *)calloc(1, bufferSize);
   inputs_[1] = (float *)calloc(1, bufferSize);
   outputs_[0] = (float *)calloc(1, bufferSize);
   outputs_[1] = (float *)calloc(1, bufferSize);

   pluginHost_->setPorts(2, inputs_, 2, outputs_);

   /* midi */
   PmError midi_err = Pm_OpenInput(
      &midi_, settings_.midiSettings().deviceReference().index_, nullptr, 512, nullptr, nullptr);
   if (midi_err != pmNoError) {
      midi_ = nullptr;
   }

   pluginHost_->activate(as.sampleRate());

   /* audio */
   auto deviceInfo = Pa_GetDeviceInfo(as.deviceReference().index_);

   PaStreamParameters params;
   params.channelCount = 2;
   params.device = as.deviceReference().index_;
   params.hostApiSpecificStreamInfo = nullptr;
   params.sampleFormat = paFloat32;
   params.suggestedLatency = 0;

   state_ = kStateRunning;
   nframes_ = as.bufferSize();
   PaError err = Pa_OpenStream(&audio_,
                               deviceInfo->maxInputChannels >= 2 ? &params : nullptr,
                               &params,
                               as.sampleRate(),
                               as.bufferSize(),
                               paClipOff | paDitherOff,
                               &Engine::audioCallback,
                               this);
   if (err != paNoError) {
      qWarning() << tr("Failed to initialize PortAudio: ") << Pa_GetErrorText(err);
      stop();
      return;
   }

   err = Pa_StartStream(audio_);
}

void Engine::stop() {
   pluginHost_->deactivate();

   if (state_ == kStateRunning)
      state_ = kStateStopping;

   if (audio_) {
      Pa_StopStream(audio_);
      Pa_CloseStream(audio_);
      audio_ = nullptr;
   }

   if (midi_) {
      Pm_Close(midi_);
      midi_ = nullptr;
   }

   state_ = kStateStopped;
}

int Engine::audioCallback(const void *  input,
                          void *        output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo * /*timeInfo*/,
                          PaStreamCallbackFlags /*statusFlags*/,
                          void *userData) {
   Engine *const      thiz = (Engine *)userData;
   const float *const in = (const float *)input;
   float *const       out = (float *)output;

   assert(thiz->inputs_[0] != nullptr);
   assert(thiz->inputs_[1] != nullptr);
   assert(thiz->outputs_[0] != nullptr);
   assert(thiz->outputs_[1] != nullptr);
   assert(frameCount == thiz->nframes_);

   // copy input
   if (in) {
      for (int i = 0; i < thiz->nframes_; ++i) {
         thiz->inputs_[0][i] = in[2 * i];
         thiz->inputs_[1][i] = in[2 * i + 1];
      }
   }

   thiz->pluginHost_->processInit(frameCount);

   MidiSettings &ms = thiz->settings_.midiSettings();

   if (thiz->midi_) {
      PmEvent evBuffer[512];
      int     numRead = Pm_Read(thiz->midi_, evBuffer, sizeof(evBuffer) / sizeof(evBuffer[0]));

      const PtTimestamp currentTime = Pt_Time();

      PmEvent *ev = evBuffer;
      for (int i = 0; i < numRead; ++i) {
         uint8_t eventType = Pm_MessageStatus(ev->message) >> 4;
         uint8_t channel = Pm_MessageStatus(ev->message) & 0xf;
         uint8_t data1 = Pm_MessageData1(ev->message);
         uint8_t data2 = Pm_MessageData2(ev->message);

         int32_t deltaMs = currentTime - ev->timestamp;
         int32_t deltaSample = (deltaMs * thiz->sampleRate_) / 1000;

         if (deltaSample >= thiz->nframes_)
            deltaSample = thiz->nframes_ - 1;

         int32_t sampleOffset = thiz->nframes_ - deltaSample;

         switch (eventType) {
         case MIDI_STATUS_NOTE_ON:
            thiz->pluginHost_->processNoteOn(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_NOTE_OFF:
            thiz->pluginHost_->processNoteOff(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_CC:
            thiz->pluginHost_->processCC(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_NOTE_AT:
            std::cerr << "Note AT key: " << (int)data1 << ", pres: " << (int)data2 << std::endl;
            thiz->pluginHost_->processNoteAt(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_CHANNEL_AT:
            ++ev;
            std::cerr << "Channel after touch" << std::endl;
            break;

         case MIDI_STATUS_PITCH_BEND:
            thiz->pluginHost_->processPitchBend(sampleOffset, channel, (data2 << 7) | data1);
            ++ev;
            break;

         default:
            std::cerr << "unknown event type: " << (int)eventType << std::endl;
            ++ev;
            break;
         }
      }
   }

   thiz->pluginHost_->process();

   // copy output
   for (int i = 0; i < thiz->nframes_; ++i) {
      out[2 * i] = thiz->outputs_[0][i];
      out[2 * i + 1] = thiz->outputs_[1][i];
   }

   thiz->steadyTime_ += frameCount;

   switch (thiz->state_) {
   case kStateRunning:
      return paContinue;
   case kStateStopping:
      thiz->state_ = kStateStopped;
      return paComplete;
   default:
      assert(false && "unreachable");
      return paAbort;
   }
}

bool Engine::loadPlugin(const QString &path, int plugin_index) {
   if (!pluginHost_->load(path, plugin_index))
      return false;

   pluginHost_->setParentWindow(parentWindow_);
   return true;
}

void Engine::unloadPlugin() {
   pluginHost_->unload();

   free(inputs_[0]);
   free(inputs_[1]);
   free(outputs_[0]);
   free(outputs_[1]);

   inputs_[0] = nullptr;
   inputs_[1] = nullptr;
   outputs_[0] = nullptr;
   outputs_[1] = nullptr;
}

void Engine::callPluginIdle() {
   if (pluginHost_)
      pluginHost_->idle();
}
