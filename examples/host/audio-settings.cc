#include <QSettings>

#include "audio-settings.hh"

static const char SAMPLE_RATE_KEY[] = "Audio/SampleRate";
static const char BUFFER_SIZE_KEY[] = "Audio/BufferSize";
static const char DEVICE_NAME_KEY[] = "Audio/DeviceName";
static const char DEVICE_INDEX_KEY[] = "Audio/DeviceIndex";

AudioSettings::AudioSettings() {}

void AudioSettings::load(QSettings &settings) {
   _deviceReference._name = settings.value(DEVICE_NAME_KEY).toString();
   _deviceReference._index = settings.value(DEVICE_INDEX_KEY).toInt();
   _sampleRate = settings.value(SAMPLE_RATE_KEY, 44100).toInt();
   _bufferSize = settings.value(BUFFER_SIZE_KEY, 256).toInt();
}

void AudioSettings::save(QSettings &settings) const {
   settings.setValue(SAMPLE_RATE_KEY, _sampleRate);
   settings.setValue(BUFFER_SIZE_KEY, _bufferSize);
   settings.setValue(DEVICE_NAME_KEY, _deviceReference._name);
   settings.setValue(DEVICE_INDEX_KEY, _deviceReference._index);
}
