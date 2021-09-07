#pragma once

#include <QDialog>

class Settings;
class SettingsWidget;

class SettingsDialog : public QDialog {
public:
   SettingsDialog(Settings &settings, QWidget *parent = nullptr);

private:
   Settings &_settings;
   SettingsWidget *_settingsWidget = nullptr;
};
