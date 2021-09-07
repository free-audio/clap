#include <cassert>

#ifdef Q_UNIX
#   include <unistd.h>
#endif

#include <QApplication>
#include <QCommandLineParser>
#include <QSettings>

#include "application.hh"
#include "main-window.hh"
#include "settings.hh"

Application *Application::_instance = nullptr;

Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(uint32_t)

Application::Application(int argc, char **argv)
   : QApplication(argc, argv), _settings(new Settings) {
   assert(!_instance);
   _instance = this;

   setOrganizationDomain("github.com/free-audio/clap");
   setOrganizationName("clap");
   setApplicationName("uhost");
   setApplicationVersion("1.0");

   parseCommandLine();

   loadSettings();

   _engine = new Engine(*this);

   _mainWindow = new MainWindow(*this);
   _mainWindow->show();

   _engine->setParentWindow(_mainWindow->getEmbedWindowId());

   /*
    * This is here JUST because macOS and QT don't process command lines properly
    * and I'm not sure why yet.
    */
   if (getenv("CLAP_HOST_FORCE_PLUGIN")) {
      qWarning() << "Warning: Loading plugin from ENV, not command line";
      _pluginPath = getenv("CLAP_HOST_FORCE_PLUGIN");
      _pluginIndex = 0;
   }

   if (_engine->loadPlugin(_pluginPath, _pluginIndex))
      _engine->start();
}

Application::~Application() {
   saveSettings();

   delete _engine;
   _engine = nullptr;

   delete _mainWindow;
   _mainWindow = nullptr;

   delete _settings;
   _settings = nullptr;
}

void Application::parseCommandLine() {
   QCommandLineParser parser;

   QCommandLineOption pluginOpt(QStringList() << "p"
                                              << "plugin",
                                tr("path to the plugin"),
                                tr("path"));
   QCommandLineOption pluginIndexOpt(QStringList() << "i"
                                                   << "plugin-index",
                                     tr("index of the plugin to create"),
                                     tr("plugin-index"),
                                     "0");

   parser.setApplicationDescription("clap standalone host");
   parser.addHelpOption();
   parser.addVersionOption();
   parser.addOption(pluginOpt);
   parser.addOption(pluginIndexOpt);

   parser.process(*this);

   _pluginPath = parser.value(pluginOpt);
   _pluginIndex = parser.value(pluginIndexOpt).toInt();
}

void Application::loadSettings() {
   QSettings s;
   _settings->load(s);
}

void Application::saveSettings() const {
   QSettings s;
   _settings->save(s);
}

void Application::restartEngine() {
   _engine->stop();
   _engine->start();
}
