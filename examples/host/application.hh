#pragma once

#include <QApplication>

#include "engine.hh"

class MainWindow;
class Settings;
class Engine;
class SpectrumAnalyzer;
class OscilloscopeAnalyzer;
class AudioRecorder;

class Application : public QApplication {
   Q_OBJECT

public:
   Application(int argc, char **argv);
   ~Application();

   Settings &settings() { return *settings_; }

   void parseCommandLine();

   void loadSettings();
   void saveSettings() const;

   MainWindow *mainWindow() const { return mainWindow_; }

   static Application &instance() { return *instance_; }

   Engine *engine() { return engine_; }

public slots:
   void restartEngine();

private:
   static Application *instance_;

   Settings *  settings_ = nullptr;
   MainWindow *mainWindow_ = nullptr;
   Engine *    engine_ = nullptr;

   QString pluginPath_;
   int     pluginIndex_ = 0;
};
