#include <QCommandLineParser>
#include <QQuickView>
#include <QWindow>

#include "../io/messages.hh"
#include "application.hh"

Application::Application(int argc, char **argv)
   : QGuiApplication(argc, argv), quickView_(new QQuickView()) {

   bool waitForDebbugger = true;
   while (waitForDebbugger)
      ;

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
   connect(socketReadNotifier_,
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              printf("ON READ\n");
              remoteChannel_->onRead();
           });

   socketWriteNotifier_ = new QSocketNotifier(socket, QSocketNotifier::Write, this);
   connect(
      socketWriteNotifier_,
      &QSocketNotifier::activated,
      [this](QSocketDescriptor socket, QSocketNotifier::Type type) { remoteChannel_->onWrite(); });

   socketErrorNotifier_ = new QSocketNotifier(socket, QSocketNotifier::Exception, this);
   connect(socketErrorNotifier_,
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              remoteChannel_->onError();
              quit();
           });

   remoteChannel_.reset(new clap::RemoteChannel(
      [this](const clap::RemoteChannel::Message &msg) { onMessage(msg); }, *this, socket, false));

   socketReadNotifier_->setEnabled(true);
   socketWriteNotifier_->setEnabled(false);
   socketErrorNotifier_->setEnabled(false);
}

void Application::modifyFd(clap_fd_flags flags) {
   socketReadNotifier_->setEnabled(flags & CLAP_FD_READ);
   socketWriteNotifier_->setEnabled(flags & CLAP_FD_WRITE);
   socketErrorNotifier_->setEnabled(flags & CLAP_FD_ERROR);
}

void Application::onMessage(const clap::RemoteChannel::Message &msg) {
   printf("ON MESSAGE\n");
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
      quickView_->show();
      clap::messages::ShowResponse rp;
      remoteChannel_->sendMessageAsync(rp);
      break;
   }

   case clap::messages::kHideRequest: {
      quickView_->hide();
      clap::messages::HideResponse rp;
      remoteChannel_->sendMessageAsync(rp);
      break;
   }

   case clap::messages::kSizeRequest: {
      clap::messages::SizeResponse rp;
      rp.width = quickView_->width();
      rp.height = quickView_->height();
      remoteChannel_->sendMessageAsync(rp);
      break;
   }
   }
}
