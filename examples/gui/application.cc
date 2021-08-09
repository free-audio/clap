#include <QCommandLineParser>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QQuickItem>
#include <QWindow>

#include "../io/messages.hh"
#include "application.hh"

Application::Application(int argc, char **argv)
   : QGuiApplication(argc, argv), quickView_(new QQuickView()) {

   bool waitForDebbugger = false;
   while (waitForDebbugger)
      ;

   QCommandLineParser parser;

   QCommandLineOption skinOpt("skin", tr("path to the skin directory"), tr("path"));
   QCommandLineOption socketOpt("socket", tr("path to the QML skin"), tr("path"));

   parser.addOption(skinOpt);
   parser.addOption(socketOpt);
   parser.addHelpOption();

   parser.process(*this);

   pluginProxy_ = new PluginProxy(this);

   quickView_->setSource(parser.value(skinOpt) + "/main.qml");
   auto qmlContext = quickView_->engine()->rootContext();
   qmlContext->setContextProperty("plugin", pluginProxy_);

   auto socket = parser.value(socketOpt).toULongLong();

   socketReadNotifier_ = new QSocketNotifier(socket, QSocketNotifier::Read, this);
   connect(
      socketReadNotifier_,
      &QSocketNotifier::activated,
      [this](QSocketDescriptor socket, QSocketNotifier::Type type) { remoteChannel_->onRead(); });

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

   QCoreApplication::arguments();
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
      pluginProxy_->defineParameter(rq.info);
      break;
   }

   case clap::messages::kSizeRequest: {
      clap::messages::SizeResponse rp;
      auto rootItem = quickView_->rootObject();
      rp.width = rootItem ? rootItem->width() : 500;
      rp.height = rootItem ? rootItem->height() : 300;
      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kAttachX11Request: {
      clap::messages::AttachX11Request rq;
      msg.get(rq);
      hostWindow_.reset(QWindow::fromWinId(rq.window));
      quickView_->setParent(hostWindow_.get());

      clap::messages::AttachResponse rp;
      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kShowRequest: {
      quickView_->show();
      clap::messages::ShowResponse rp;
      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kHideRequest: {
      quickView_->hide();
      clap::messages::HideResponse rp;
      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      break;
   }
   }
}
