#pragma once

#include <QWidget>

class QTabWidget;
class Settings;
class AudioSettingsWidget;
class MidiSettingsWidget;

class SettingsWidget : public QWidget {
   Q_OBJECT
public:
   explicit SettingsWidget(Settings &settings);

private:
   QTabWidget *_tabWidget = nullptr;
   AudioSettingsWidget *_audioSettingsWidget = nullptr;
   MidiSettingsWidget *_midiSettingsWidget = nullptr;
   Settings &_settings;
};
