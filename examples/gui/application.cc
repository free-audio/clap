#include <QCommandLineParser>
#include <QQmlApplicationEngine>

#include "application.hh"

Application::Application(int argc, char **argv)
   : QGuiApplication(argc, argv), qmlEngine_(new QQmlApplicationEngine(this)) {

   QCommandLineParser parser;

   QCommandLineOption qmlOpt("qml", tr("path to the QML skin"), tr("path"));
   QCommandLineOption socketOpt("socket", tr("path to the QML skin"), tr("path"));

   parser.addOption(qmlOpt);
   parser.addOption(socketOpt);
   parser.addHelpOption();

   parser.process(*this);

   qmlEngine_->load(parser.value(qmlOpt));
   if (qmlEngine_->rootObjects().empty())
      throw std::invalid_argument("bad qml file");
}
