#pragma once

#include "audio-settings.hh"
#include "midi-settings.hh"

class QSettings;

class Settings {
public:
   Settings();

   void load(QSettings &settings);
   void save(QSettings &settings) const;

   AudioSettings &audioSettings() { return audioSettings_; }
   MidiSettings & midiSettings() { return midiSettings_; }

private:
   AudioSettings audioSettings_;
   MidiSettings  midiSettings_;
};
