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

   Settings &settings() { return *_settings; }

   void parseCommandLine();

   void loadSettings();
   void saveSettings() const;

   MainWindow *mainWindow() const { return _mainWindow; }

   static Application &instance() { return *_instance; }

   Engine *engine() { return _engine; }

public slots:
   void restartEngine();

private:
   static Application *_instance;

   Settings *  _settings = nullptr;
   MainWindow *_mainWindow = nullptr;
   Engine *    _engine = nullptr;

   QString _pluginPath;
   int     _pluginIndex = 0;
};
