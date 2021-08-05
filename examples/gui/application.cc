#include <QCommandLineParser>
#include <QQuickView>
#include <QWindow>

#include "../io/messages.hh"
#include "application.hh"

Application::Application(int argc, char **argv)
   : QGuiApplication(argc, argv), quickView_(new QQuickView()) {

   QCommandLineParser parser;

   QCommandLineOption qmlOpt("qml", tr("path to the QML skin"), tr("path"));
   QCommandLineOption socketOpt("socket", tr("path to the QML skin"), tr("path"));

   parser.addOption(qmlOpt);
   parser.addOption(socketOpt);
   parser.addHelpOption();

   parser.process(*this);

   quickView_->setSource(parser.value(qmlOpt));

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
      // TODO
      break;
   }

   case clap::messages::kAttachX11Request: {
      clap::messages::AttachX11Request rq;
      msg.get(rq);
      hostWindow_.reset(QWindow::fromWinId(rq.window));
      quickView_->setParent(hostWindow_.get());

      clap::messages::AttachResponse rp;
      remoteChannel_->sendMessageAsync(rp);
      break;
   }

   case clap::messages::kShowRequest: {
      clap::messages::ShowRequest rq;
      msg.get(rq);

      quickView_->show();
      clap::messages::ShowResponse rp;
      remoteChannel_->sendMessageAsync(rp);
      break;
   }

   case clap::messages::kHideRequest: {
      clap::messages::HideRequest rq;
      msg.get(rq);

      quickView_->hide();
      clap::messages::HideResponse rp;
      remoteChannel_->sendMessageAsync(rp);
      break;
   }
   }
}
