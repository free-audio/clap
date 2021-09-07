#pragma once

#include "device-reference.hh"

class QSettings;

class MidiSettings {
public:
   MidiSettings();

   void load(QSettings &settings);
   void save(QSettings &settings) const;

   const DeviceReference &deviceReference() const { return _deviceReference; }
   void setDeviceReference(const DeviceReference &ref) { _deviceReference = ref; }

private:
   DeviceReference _deviceReference;
};
