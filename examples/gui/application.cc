#include <QCommandLineParser>
#include <QQmlApplicationEngine>

#include "../io/messages.hh"
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

   auto socket = parser.value(socketOpt).toULongLong();

   socketReadNotifier_ = new QSocketNotifier(socket, QSocketNotifier::Read, this);
   socketReadNotifier_->setEnabled(true);
   connect(
      socketReadNotifier_,
      &QSocketNotifier::activated,
      [this](QSocketDescriptor socket, QSocketNotifier::Type type) { remoteChannel_->onRead(); });

   socketWriteNotifier_ = new QSocketNotifier(socket, QSocketNotifier::Write, this);
   socketWriteNotifier_->setEnabled(false);
   connect(
      socketWriteNotifier_,
      &QSocketNotifier::activated,
      [this](QSocketDescriptor socket, QSocketNotifier::Type type) { remoteChannel_->onWrite(); });

   socketErrorNotifier_ = new QSocketNotifier(socket, QSocketNotifier::Exception, this);
   socketErrorNotifier_->setEnabled(false);
   connect(socketErrorNotifier_,
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              remoteChannel_->onError();
              quit();
           });

   remoteChannel_.reset(new clap::RemoteChannel(
      [this](const clap::RemoteChannel::Message &msg) { onMessage(msg); }, *this, socket, false));
}

void Application::modifyFd(clap_fd_flags flags) {
   socketReadNotifier_->setEnabled(flags & CLAP_FD_READ);
   socketWriteNotifier_->setEnabled(flags & CLAP_FD_WRITE);
   socketErrorNotifier_->setEnabled(flags & CLAP_FD_ERROR);
}

void Application::onMessage(const clap::RemoteChannel::Message &msg) {
   switch (msg.type) {
   case clap::messages::kDefineParameterRequest: {
      clap::messages::DefineParameterRequest rq;
      msg.get(rq);
      break;
   }
   }
}
