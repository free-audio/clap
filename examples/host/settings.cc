#include "settings.hh"

Settings::Settings() {}

void Settings::load(QSettings &settings) {
   audioSettings_.load(settings);
   midiSettings_.load(settings);
}

void Settings::save(QSettings &settings) const {
   audioSettings_.save(settings);
   midiSettings_.save(settings);
}
