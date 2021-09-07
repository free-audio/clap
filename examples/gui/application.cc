#include <QCommandLineParser>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QWindow>

#include "../io/messages.hh"
#include "application.hh"

Application::Application(int &argc, char **argv)
   : QGuiApplication(argc, argv), _quickView(new QQuickView()) {

   bool waitForDebbugger = false;
   while (waitForDebbugger)
      ;

   qmlRegisterType<ParameterProxy>("org.clap", 1, 0, "ParameterProxy");
   qmlRegisterType<PluginProxy>("org.clap", 1, 0, "PluginProxy");

   QCommandLineParser parser;

   QCommandLineOption skinOpt("skin", tr("path to the skin directory"), tr("path"));
   QCommandLineOption socketOpt("socket", tr("path to the QML skin"), tr("path"));
   QCommandLineOption qmlLibOpt("qml-import", tr("QML import path"), tr("path"));

   parser.addOption(skinOpt);
   parser.addOption(socketOpt);
   parser.addOption(qmlLibOpt);
   parser.addHelpOption();

   parser.process(*this);

   _pluginProxy = new PluginProxy(this);

   auto qmlContext = _quickView->engine()->rootContext();
   for (const auto &str : parser.values(qmlLibOpt))
      _quickView->engine()->addImportPath(str);
   qmlContext->setContextProperty("plugin", _pluginProxy);

   _quickView->setSource(parser.value(skinOpt) + "/main.qml");

   auto socket = parser.value(socketOpt).toULongLong();

   _socketReadNotifier.reset(new QSocketNotifier(socket, QSocketNotifier::Read, this));
   connect(_socketReadNotifier.get(),
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              _remoteChannel->onRead();
              if (!_remoteChannel->isOpen())
                 quit();
           });

   _socketWriteNotifier.reset(new QSocketNotifier(socket, QSocketNotifier::Write, this));
   connect(_socketWriteNotifier.get(),
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              _remoteChannel->onWrite();
              if (!_remoteChannel->isOpen()) {
                 quit();
              }
           });

   _socketErrorNotifier.reset(new QSocketNotifier(socket, QSocketNotifier::Exception, this));
   connect(_socketErrorNotifier.get(),
           &QSocketNotifier::activated,
           [this](QSocketDescriptor socket, QSocketNotifier::Type type) {
              _remoteChannel->onError();
              quit();
           });

   _remoteChannel.reset(new clap::RemoteChannel(
      [this](const clap::RemoteChannel::Message &msg) { onMessage(msg); }, *this, socket, false));

   _socketReadNotifier->setEnabled(true);
   _socketWriteNotifier->setEnabled(false);
   _socketErrorNotifier->setEnabled(false);

   QCoreApplication::arguments();
}

void Application::modifyFd(clap_fd_flags flags) {
   _socketReadNotifier->setEnabled(flags & CLAP_FD_READ);
   _socketWriteNotifier->setEnabled(flags & CLAP_FD_WRITE);
   _socketErrorNotifier->setEnabled(flags & CLAP_FD_ERROR);
}

void Application::removeFd() {
   _socketReadNotifier.reset();
   _socketWriteNotifier.reset();
   _socketErrorNotifier.reset();
   quit();
}

void Application::onMessage(const clap::RemoteChannel::Message &msg) {
   switch (msg.type) {
   case clap::messages::kDestroyRequest:
      clap::messages::DestroyResponse rp;
      _remoteChannel->sendResponseAsync(rp, msg.cookie);
      quit();
      break;

   case clap::messages::kDefineParameterRequest: {
      clap::messages::DefineParameterRequest rq;
      msg.get(rq);
      _pluginProxy->defineParameter(rq.info);
      break;
   }

   case clap::messages::kParameterValueRequest: {
      clap::messages::ParameterValueRequest rq;
      msg.get(rq);
      auto p = _pluginProxy->param(rq.paramId);
      p->setValueFromPlugin(rq.value);
      p->setModulationFromPlugin(rq.modulation);
      break;
   }

   case clap::messages::kSizeRequest: {
      clap::messages::SizeResponse rp;
      auto rootItem = _quickView->rootObject();
      rp.width = rootItem ? rootItem->width() : 500;
      rp.height = rootItem ? rootItem->height() : 300;
      _remoteChannel->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kAttachX11Request: {
      clap::messages::AttachX11Request rq;
      clap::messages::AttachResponse rp{false};
      msg.get(rq);

#ifdef Q_OS_LINUX
      _hostWindow.reset(QWindow::fromWinId(rq.window));
      _quickView->setParent(_hostWindow.get());
      sync();
      rp.succeed = true;
#endif

      _remoteChannel->sendResponseAsync(rp, msg.cookie);
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

      _remoteChannel->sendResponseAsync(rp, msg.cookie);
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

      _remoteChannel->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kShowRequest: {
      _quickView->show();
      clap::messages::ShowResponse rp;
      _remoteChannel->sendResponseAsync(rp, msg.cookie);
      break;
   }

   case clap::messages::kHideRequest: {
      _quickView->hide();
      clap::messages::HideResponse rp;
      _remoteChannel->sendResponseAsync(rp, msg.cookie);
      break;
   }
   }
}
