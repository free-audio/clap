#include <iostream>

#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>

#include <portmidi.h>

#include "midi-settings-widget.hh"
#include "midi-settings.hh"

MidiSettingsWidget::MidiSettingsWidget(MidiSettings &midiSettings) : midiSettings_(midiSettings) {
   auto layout = new QVBoxLayout(this);

   auto deviceComboBox = new QComboBox;
   bool deviceFound = false;
   auto deviceCount = Pm_CountDevices();
   int  inputIndex = 0;

   if (deviceCount <= 0) {
      std::cerr << "warning: no midi device found!" << std::endl;
   }

   for (int i = 0; i < deviceCount; ++i) {
      auto deviceInfo = Pm_GetDeviceInfo(i);
      if (!deviceInfo->input)
         continue;

      deviceComboBox->addItem(deviceInfo->name);

      if (!deviceFound && midiSettings_.deviceReference().index_ == i &&
          midiSettings_.deviceReference().name_ == deviceInfo->name) {
         deviceComboBox->setCurrentIndex(inputIndex);
         deviceFound = true;
         selectedDeviceChanged(inputIndex);
      }

      ++inputIndex;
   }

   // try to find the device just by its name.
   inputIndex = 0;
   for (int i = 0; !deviceFound && i < deviceCount; ++i) {
      auto deviceInfo = Pm_GetDeviceInfo(i);
      if (!deviceInfo->input)
         continue;

      if (midiSettings_.deviceReference().name_ == deviceInfo->name) {
         deviceComboBox->setCurrentIndex(inputIndex);
         deviceFound = true;
         selectedDeviceChanged(inputIndex);
      }

      ++inputIndex;
   }

   connect(
      deviceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedDeviceChanged(int)));

   layout->addWidget(deviceComboBox);

   QGroupBox *groupBox = new QGroupBox;
   groupBox->setLayout(layout);
   groupBox->setTitle(tr("MIDI"));

   QVBoxLayout *groupLayout = new QVBoxLayout;
   groupLayout->addWidget(groupBox);
   setLayout(groupLayout);
}

void MidiSettingsWidget::selectedDeviceChanged(int index) {
   int  inputIndex = 0;
   auto deviceCount = Pm_CountDevices();
   for (int i = 0; i < deviceCount; ++i) {
      auto deviceInfo = Pm_GetDeviceInfo(i);
      if (!deviceInfo->input)
         continue;

      if (inputIndex != index) {
         ++inputIndex;
         continue;
      }

      DeviceReference ref;
      ref.index_ = i;
      ref.name_ = deviceInfo->name;
      midiSettings_.setDeviceReference(ref);
      break;
   }
}
