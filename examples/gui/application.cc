#include <QCommandLineParser>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QWindow>

#include "../io/messages.hh"
#include "application.hh"

Application::Application(int &argc, char **argv)
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

   auto qmlContext = quickView_->engine()->rootContext();
   qmlContext->setContextProperty("plugin", pluginProxy_);

   quickView_->setSource(parser.value(skinOpt) + "/main.qml");

   auto socket = parser.value(socketOpt).toULongLong();

   socketReadNotifier_.reset(new QSocketNotifier(socket, QSocketNotifier::Read, this));
   connect(socketReadNotifier_.get(),
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              remoteChannel_->onRead();
              if (!remoteChannel_->isOpen())
                 quit();
           });

   socketWriteNotifier_.reset(new QSocketNotifier(socket, QSocketNotifier::Write, this));
   connect(socketWriteNotifier_.get(),
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              remoteChannel_->onWrite();
              if (!remoteChannel_->isOpen()) {
                 quit();
              }
           });

   socketErrorNotifier_.reset(new QSocketNotifier(socket, QSocketNotifier::Exception, this));
   connect(socketErrorNotifier_.get(),
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

void Application::removeFd() {
   socketReadNotifier_.reset();
   socketWriteNotifier_.reset();
   socketErrorNotifier_.reset();
   quit();
}

void Application::onMessage(const clap::RemoteChannel::Message &msg) {
   switch (msg.type) {
   case clap::messages::kDestroyRequest:
      clap::messages::DestroyResponse rp;
      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      quit();
      break;

   case clap::messages::kDefineParameterRequest: {
      clap::messages::DefineParameterRequest rq;
      msg.get(rq);
      pluginProxy_->defineParameter(rq.info);
      break;
   }

   case clap::messages::kParameterValueRequest: {
      clap::messages::ParameterValueRequest rq;
      msg.get(rq);
      auto p = pluginProxy_->param(rq.paramId);
      p->setValueFromPlugin(rq.value);
      p->setModulationFromPlugin(rq.modulation);
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
      clap::messages::AttachResponse rp{false};
      msg.get(rq);

#ifdef Q_OS_LINUX
      hostWindow_.reset(QWindow::fromWinId(rq.window));
      quickView_->setParent(hostWindow_.get());
      rp.succeed = true;
#endif

      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kAttachWin32Request: {
      clap::messages::AttachWin32Request rq;
      clap::messages::AttachResponse rp{false};
      msg.get(rq);

#ifdef Q_OS_WIN
      hostWindow_.reset(QWindow::fromWinId(rq.hwnd));
      quickView_->setParent(hostWindow_.get());
      rp.succeed = true;
#endif

      remoteChannel_->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kAttachCocoaRequest: {
      clap::messages::AttachCocoaRequest rq;
      clap::messages::AttachResponse rp{false};

      msg.get(rq);

#ifdef Q_OS_MACOS
      hostWindow_.reset(QWindow::fromWinId(rq.nsView));
      quickView_->setParent(hostWindow_.get());
      rp.succeed = true;
#endif

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
