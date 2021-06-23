#pragma once

#include <QGuiApplication>

class QQmlApplicationEngine;

class Application : public QGuiApplication {
   Q_OBJECT;

public:
   Application(int argc, char **argv);

private:
   QQmlApplicationEngine *qmlEngine_ = nullptr;
};