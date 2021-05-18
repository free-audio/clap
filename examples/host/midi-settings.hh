#pragma once

#include "device-reference.hh"

class QSettings;

class MidiSettings {
public:
   MidiSettings();

   void load(QSettings &settings);
   void save(QSettings &settings) const;

   const DeviceReference &deviceReference() const { return deviceReference_; }
   void                   setDeviceReference(const DeviceReference &ref) { deviceReference_ = ref; }

private:
   DeviceReference deviceReference_;
};
