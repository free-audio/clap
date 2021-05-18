#pragma once

#include <QDialog>

class Settings;
class SettingsWidget;

class SettingsDialog : public QDialog {
public:
   SettingsDialog(Settings &settings, QWidget *parent = nullptr);

private:
   Settings &      settings_;
   SettingsWidget *settingsWidget_ = nullptr;
};
