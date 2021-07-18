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

Application *Application::instance_ = nullptr;

Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(uint32_t)

Application::Application(int argc, char **argv)
   : QApplication(argc, argv), settings_(new Settings) {
   assert(!instance_);
   instance_ = this;

   setOrganizationDomain("github.com/free-audio/clap");
   setOrganizationName("clap");
   setApplicationName("uhost");
   setApplicationVersion("1.0");

   parseCommandLine();

   loadSettings();

   engine_ = new Engine(*this);

   mainWindow_ = new MainWindow(*this);
   mainWindow_->show();

   engine_->setParentWindow(mainWindow_->getEmbedWindowId());

   /*
    * This is here JUST because macOS and QT don't process command lines properly
    * and I'm not sure why yet.
    */
   if (getenv("CLAP_HOST_FORCE_PLUGIN")) {
      qWarning() << "Warning: Loading plugin from ENV, not command line";
      pluginPath_ = getenv("CLAP_HOST_FORCE_PLUGIN");
      pluginIndex_ = 0;
   }

   if (engine_->loadPlugin(pluginPath_, pluginIndex_))
      engine_->start();
}

Application::~Application() {
   saveSettings();

   delete mainWindow_;
   mainWindow_ = nullptr;

   delete engine_;
   engine_ = nullptr;

   delete settings_;
   settings_ = nullptr;
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

   pluginPath_ = parser.value(pluginOpt);
   pluginIndex_ = parser.value(pluginIndexOpt).toInt();
}

void Application::loadSettings() {
   QSettings s;
   settings_->load(s);
}

void Application::saveSettings() const {
   QSettings s;
   settings_->save(s);
}

void Application::restartEngine() {
   engine_->stop();
   engine_->start();
}
