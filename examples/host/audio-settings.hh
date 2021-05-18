#pragma once

#include "device-reference.hh"

class QSettings;

class AudioSettings {
public:
   AudioSettings();

   void load(QSettings &settings);
   void save(QSettings &settings) const;

   int  sampleRate() const { return sampleRate_; }
   void setSampleRate(int sampleRate) { sampleRate_ = sampleRate; }

   void                   setDeviceReference(DeviceReference dr) { deviceReference_ = dr; }
   const DeviceReference &deviceReference() const { return deviceReference_; }

   int  bufferSize() const { return bufferSize_; }
   void setBufferSize(int bufferSize) { bufferSize_ = bufferSize; }

private:
   DeviceReference deviceReference_;
   int             sampleRate_ = 44100;
   int             bufferSize_ = 128;
};
