#pragma once

#include <vector>

#include <QWidget>

class MidiSettings;

class MidiSettingsWidget : public QWidget {
   Q_OBJECT
public:
   explicit MidiSettingsWidget(MidiSettings &midiSettings);

signals:

public slots:
   void selectedDeviceChanged(int index);

private:
   MidiSettings &midiSettings_;
};
