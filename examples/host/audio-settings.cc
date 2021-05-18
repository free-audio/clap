#include <QSettings>

#include "audio-settings.hh"

static const char SAMPLE_RATE_KEY[] = "Audio/SampleRate";
static const char BUFFER_SIZE_KEY[] = "Audio/BufferSize";
static const char DEVICE_NAME_KEY[] = "Audio/DeviceName";
static const char DEVICE_INDEX_KEY[] = "Audio/DeviceIndex";

AudioSettings::AudioSettings() {}

void AudioSettings::load(QSettings &settings) {
   deviceReference_.name_ = settings.value(DEVICE_NAME_KEY).toString();
   deviceReference_.index_ = settings.value(DEVICE_INDEX_KEY).toInt();
   sampleRate_ = settings.value(SAMPLE_RATE_KEY, 44100).toInt();
   bufferSize_ = settings.value(BUFFER_SIZE_KEY, 256).toInt();
}

void AudioSettings::save(QSettings &settings) const {
   settings.setValue(SAMPLE_RATE_KEY, sampleRate_);
   settings.setValue(BUFFER_SIZE_KEY, bufferSize_);
   settings.setValue(DEVICE_NAME_KEY, deviceReference_.name_);
   settings.setValue(DEVICE_INDEX_KEY, deviceReference_.index_);
}
