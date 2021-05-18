#include <QSettings>

#include "midi-settings.hh"

static const char DEVICE_NAME_KEY[] = "Midi/DeviceName";
static const char DEVICE_INDEX_KEY[] = "Midi/DeviceIndex";
static const char LATCH_KEY[] = "Midi/Latch";
static const char ARP_KEY[] = "Midi/Arp";

MidiSettings::MidiSettings() {}

void MidiSettings::load(QSettings &settings) {
   deviceReference_.name_ = settings.value(DEVICE_NAME_KEY).toString();
   deviceReference_.index_ = settings.value(DEVICE_INDEX_KEY).toInt();
}

void MidiSettings::save(QSettings &settings) const {
   settings.setValue(DEVICE_NAME_KEY, deviceReference_.name_);
   settings.setValue(DEVICE_INDEX_KEY, deviceReference_.index_);
}
