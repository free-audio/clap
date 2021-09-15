#pragma once

#include <QGuiApplication>
#include <QSocketNotifier>
#include <QWindow>

#include "../io/remote-channel.hh"
#include "plugin-proxy.hh"
#include "transport-proxy.hh"

class QQuickView;

class Application : public QGuiApplication, public clap::RemoteChannel::EventControl {
   Q_OBJECT;

public:
   Application(int& argc, char **argv);

   clap::RemoteChannel& remoteChannel() const { return *_remoteChannel; }
   void modifyFd(clap_fd_flags flags) override;
   void removeFd() override;

   static Application& instance() { return *dynamic_cast<Application *>(QGuiApplication::instance()); }

private:
   void onMessage(const clap::RemoteChannel::Message& msg);

   QQuickView *_quickView = nullptr;
   std::unique_ptr<QSocketNotifier> _socketReadNotifier;
   std::unique_ptr<QSocketNotifier> _socketWriteNotifier;
   std::unique_ptr<QSocketNotifier> _socketErrorNotifier;

   std::unique_ptr<QWindow> _hostWindow = nullptr;

   std::unique_ptr<clap::RemoteChannel> _remoteChannel;
   PluginProxy *_pluginProxy = nullptr;
   TransportProxy *_transportProxy = nullptr;
};