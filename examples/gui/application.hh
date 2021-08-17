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
   Application(int& argc, char **argv);

   clap::RemoteChannel& remoteChannel() const { return *remoteChannel_; }
   void modifyFd(clap_fd_flags flags) override;
   void removeFd() override;

   static Application& instance() { return *dynamic_cast<Application *>(QGuiApplication::instance()); }

private:
   void onMessage(const clap::RemoteChannel::Message& msg);

   QQuickView *quickView_ = nullptr;
   std::unique_ptr<QSocketNotifier> socketReadNotifier_;
   std::unique_ptr<QSocketNotifier> socketWriteNotifier_;
   std::unique_ptr<QSocketNotifier> socketErrorNotifier_;

   std::unique_ptr<QWindow> hostWindow_ = nullptr;

   std::unique_ptr<clap::RemoteChannel> remoteChannel_;
   PluginProxy *pluginProxy_ = nullptr;
};