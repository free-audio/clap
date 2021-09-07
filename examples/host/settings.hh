#pragma once

#include "audio-settings.hh"
#include "midi-settings.hh"

class QSettings;

class Settings {
public:
   Settings();

   void load(QSettings &settings);
   void save(QSettings &settings) const;

   AudioSettings &audioSettings() { return _audioSettings; }
   MidiSettings & midiSettings() { return _midiSettings; }

private:
   AudioSettings _audioSettings;
   MidiSettings  _midiSettings;
};
