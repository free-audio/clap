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

signals:

public slots:

private:
   QTabWidget *         tabWidget_ = nullptr;
   AudioSettingsWidget *audioSettingsWidget_ = nullptr;
   MidiSettingsWidget * midiSettingsWidget_ = nullptr;
   Settings &           settings_;
};
