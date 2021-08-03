#pragma once

#include <QGuiApplication>
#include <QSocketNotifier>

#include "../io/remote-channel.hh"

class QQmlApplicationEngine;

class Application : public QGuiApplication, public clap::RemoteChannel::EventControl {
   Q_OBJECT;

public:
   Application(int argc, char **argv);

   void modifyFd(clap_fd_flags flags) override;
   void onMessage(const clap::RemoteChannel::Message& msg);

private:
   QQmlApplicationEngine *qmlEngine_ = nullptr;
   QSocketNotifier *socketReadNotifier_ = nullptr;
   QSocketNotifier *socketWriteNotifier_ = nullptr;
   QSocketNotifier *socketErrorNotifier_ = nullptr;

   std::unique_ptr<clap::RemoteChannel> remoteChannel_;
};