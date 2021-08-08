#pragma once

#include <QGuiApplication>
#include <QSocketNotifier>
#include <QWindow>

#include "../io/remote-channel.hh"
#include "plugin-proxy.hh"

class QQuickView;

class Application : public QGuiApplication, public clap::RemoteChannel::EventControl {
   Q_OBJECT;

public:
   Application(int argc, char **argv);

   clap::RemoteChannel& remoteChannel() const { return *remoteChannel_; }
   void modifyFd(clap_fd_flags flags) override;

   static Application& instance() { return *dynamic_cast<Application *>(QCoreApplication::instance()); }

private:
   void onMessage(const clap::RemoteChannel::Message& msg);

   QQuickView *quickView_ = nullptr;
   QSocketNotifier *socketReadNotifier_ = nullptr;
   QSocketNotifier *socketWriteNotifier_ = nullptr;
   QSocketNotifier *socketErrorNotifier_ = nullptr;

   std::unique_ptr<QWindow> hostWindow_ = nullptr;

   std::unique_ptr<clap::RemoteChannel> remoteChannel_;
   PluginProxy *plugin_ = nullptr;
};