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
   : QObject(&application), _application(application), _settings(application.settings()),
     _idleTimer(this) {
   _pluginHost.reset(new PluginHost(*this));

   connect(&_idleTimer, &QTimer::timeout, this, QOverload<>::of(&Engine::callPluginIdle));
   _idleTimer.start(1000 / 30);
}

Engine::~Engine() {
   std::clog << "     ####### STOPING ENGINE #########" << std::endl;
   stop();
   unloadPlugin();
   std::clog << "     ####### ENGINE STOPPED #########" << std::endl;
}

void Engine::start() {
   assert(!_audio);
   assert(_state == kStateStopped);

   auto &    as = _settings.audioSettings();
   const int bufferSize = 4 * 2 * as.bufferSize();

   _inputs[0] = (float *)calloc(1, bufferSize);
   _inputs[1] = (float *)calloc(1, bufferSize);
   _outputs[0] = (float *)calloc(1, bufferSize);
   _outputs[1] = (float *)calloc(1, bufferSize);

   _pluginHost->setPorts(2, _inputs, 2, _outputs);

   /* midi */
   PmError midi_err = Pm_OpenInput(
      &_midi, _settings.midiSettings().deviceReference()._index, nullptr, 512, nullptr, nullptr);
   if (midi_err != pmNoError) {
      _midi = nullptr;
   }

   _pluginHost->activate(as.sampleRate());

   /* audio */
   auto deviceInfo = Pa_GetDeviceInfo(as.deviceReference()._index);

   PaStreamParameters params;
   params.channelCount = 2;
   params.device = as.deviceReference()._index;
   params.hostApiSpecificStreamInfo = nullptr;
   params.sampleFormat = paFloat32;
   params.suggestedLatency = 0;

   _state = kStateRunning;
   _nframes = as.bufferSize();
   PaError err = Pa_OpenStream(&_audio,
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

   err = Pa_StartStream(_audio);
}

void Engine::stop() {
   _pluginHost->deactivate();

   if (_state == kStateRunning)
      _state = kStateStopping;

   if (_audio) {
      Pa_StopStream(_audio);
      Pa_CloseStream(_audio);
      _audio = nullptr;
   }

   if (_midi) {
      Pm_Close(_midi);
      _midi = nullptr;
   }

   _state = kStateStopped;
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

   assert(thiz->_inputs[0] != nullptr);
   assert(thiz->_inputs[1] != nullptr);
   assert(thiz->_outputs[0] != nullptr);
   assert(thiz->_outputs[1] != nullptr);
   assert(frameCount == thiz->_nframes);

   // copy input
   if (in) {
      for (int i = 0; i < thiz->_nframes; ++i) {
         thiz->_inputs[0][i] = in[2 * i];
         thiz->_inputs[1][i] = in[2 * i + 1];
      }
   }

   thiz->_pluginHost->processBegin(frameCount);

   MidiSettings &ms = thiz->_settings.midiSettings();

   if (thiz->_midi) {
      PmEvent evBuffer[512];
      int     numRead = Pm_Read(thiz->_midi, evBuffer, sizeof(evBuffer) / sizeof(evBuffer[0]));

      const PtTimestamp currentTime = Pt_Time();

      PmEvent *ev = evBuffer;
      for (int i = 0; i < numRead; ++i) {
         uint8_t eventType = Pm_MessageStatus(ev->message) >> 4;
         uint8_t channel = Pm_MessageStatus(ev->message) & 0xf;
         uint8_t data1 = Pm_MessageData1(ev->message);
         uint8_t data2 = Pm_MessageData2(ev->message);

         int32_t deltaMs = currentTime - ev->timestamp;
         int32_t deltaSample = (deltaMs * thiz->_sampleRate) / 1000;

         if (deltaSample >= thiz->_nframes)
            deltaSample = thiz->_nframes - 1;

         int32_t sampleOffset = thiz->_nframes - deltaSample;

         switch (eventType) {
         case MIDI_STATUS_NOTE_ON:
            thiz->_pluginHost->processNoteOn(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_NOTE_OFF:
            thiz->_pluginHost->processNoteOff(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_CC:
            thiz->_pluginHost->processCC(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_NOTE_AT:
            std::cerr << "Note AT key: " << (int)data1 << ", pres: " << (int)data2 << std::endl;
            thiz->_pluginHost->processNoteAt(sampleOffset, channel, data1, data2);
            ++ev;
            break;

         case MIDI_STATUS_CHANNEL_AT:
            ++ev;
            std::cerr << "Channel after touch" << std::endl;
            break;

         case MIDI_STATUS_PITCH_BEND:
            thiz->_pluginHost->processPitchBend(sampleOffset, channel, (data2 << 7) | data1);
            ++ev;
            break;

         default:
            std::cerr << "unknown event type: " << (int)eventType << std::endl;
            ++ev;
            break;
         }
      }
   }

   thiz->_pluginHost->process();

   // copy output
   for (int i = 0; i < thiz->_nframes; ++i) {
      out[2 * i] = thiz->_outputs[0][i];
      out[2 * i + 1] = thiz->_outputs[1][i];
   }

   thiz->_steadyTime += frameCount;

   switch (thiz->_state) {
   case kStateRunning:
      return paContinue;
   case kStateStopping:
      thiz->_state = kStateStopped;
      return paComplete;
   default:
      assert(false && "unreachable");
      return paAbort;
   }
}

bool Engine::loadPlugin(const QString &path, int plugin_index) {
   if (!_pluginHost->load(path, plugin_index))
      return false;

   _pluginHost->setParentWindow(_parentWindow);
   return true;
}

void Engine::unloadPlugin() {
   _pluginHost->unload();

   free(_inputs[0]);
   free(_inputs[1]);
   free(_outputs[0]);
   free(_outputs[1]);

   _inputs[0] = nullptr;
   _inputs[1] = nullptr;
   _outputs[0] = nullptr;
   _outputs[1] = nullptr;
}

void Engine::callPluginIdle() {
   if (_pluginHost)
      _pluginHost->idle();
}
