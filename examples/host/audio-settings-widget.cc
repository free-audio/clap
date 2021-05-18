#include <iostream>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <portaudio.h>

#include "audio-settings-widget.hh"
#include "audio-settings.hh"

static const std::vector<int> SAMPLE_RATES = {
   44100,
   48000,
   88200,
   96000,
   176400,
   192000,
};

static const std::vector<int> BUFFER_SIZES = {32, 48, 64, 96, 128, 192, 256, 384, 512};

AudioSettingsWidget::AudioSettingsWidget(AudioSettings &audioSettings)
   : audioSettings_(audioSettings) {
   /* devices */
   auto deviceComboBox = new QComboBox(this);
   auto deviceCount = Pa_GetDeviceCount();
   bool deviceFound = false;

   for (int i = 0; i < deviceCount; ++i) {
      auto deviceInfo = Pa_GetDeviceInfo(i);
      deviceComboBox->addItem(deviceInfo->name);

      if (!deviceFound && audioSettings_.deviceReference().index_ == i &&
          audioSettings_.deviceReference().name_ == deviceInfo->name) {
         deviceComboBox->setCurrentIndex(i);
         deviceFound = true;
         selectedDeviceChanged(i);
      }
   }

   // try to find the device just by its name.
   for (int i = 0; !deviceFound && i < deviceCount; ++i) {
      auto deviceInfo = Pa_GetDeviceInfo(i);
      if (audioSettings_.deviceReference().name_ == deviceInfo->name) {
         deviceComboBox->setCurrentIndex(i);
         deviceFound = true;
         selectedDeviceChanged(i);
      }
   }

   connect(
      deviceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedDeviceChanged(int)));

   /* sample rate */
   sampleRateWidget_ = new QComboBox(this);
   for (size_t i = 0; i < SAMPLE_RATES.size(); ++i) {
      int sr = SAMPLE_RATES[i];
      sampleRateWidget_->addItem(QString::number(sr));
      if (sr == audioSettings_.sampleRate()) {
         sampleRateWidget_->setCurrentIndex(i);
         selectedSampleRateChanged(i);
      }
   }
   connect(sampleRateWidget_,
           SIGNAL(currentIndexChanged(int)),
           this,
           SLOT(selectedSampleRateChanged(int)));

   /* buffer size */
   bufferSizeWidget_ = new QComboBox(this);
   for (size_t i = 0; i < BUFFER_SIZES.size(); ++i) {
      int bs = BUFFER_SIZES[i];
      bufferSizeWidget_->addItem(QString::number(bs));
      if (bs == audioSettings_.bufferSize()) {
         bufferSizeWidget_->setCurrentIndex(i);
         selectedBufferSizeChanged(i);
      }
   }
   connect(bufferSizeWidget_,
           SIGNAL(currentIndexChanged(int)),
           this,
           SLOT(selectedBufferSizeChanged(int)));

   auto layout = new QGridLayout(this);
   layout->addWidget(new QLabel(tr("Device")), 0, 0);
   layout->addWidget(new QLabel(tr("Sample rate")), 1, 0);
   layout->addWidget(new QLabel(tr("Buffer size")), 2, 0);

   layout->addWidget(deviceComboBox, 0, 1);
   layout->addWidget(sampleRateWidget_, 1, 1);
   layout->addWidget(bufferSizeWidget_, 2, 1);

   QGroupBox *groupBox = new QGroupBox(this);
   groupBox->setLayout(layout);
   groupBox->setTitle(tr("Audio"));

   QLayout *groupLayout = new QVBoxLayout();
   groupLayout->addWidget(groupBox);
   setLayout(groupLayout);
}

void AudioSettingsWidget::selectedDeviceChanged(int index) {
   auto deviceInfo = Pa_GetDeviceInfo(index);

   DeviceReference ref;
   ref.index_ = index;
   ref.name_ = deviceInfo->name;
   audioSettings_.setDeviceReference(ref);
}

void AudioSettingsWidget::selectedSampleRateChanged(int index) {
   audioSettings_.setSampleRate(sampleRateWidget_->itemText(index).toInt());
}

void AudioSettingsWidget::selectedBufferSizeChanged(int index) {
   audioSettings_.setBufferSize(bufferSizeWidget_->itemText(index).toInt());
}
