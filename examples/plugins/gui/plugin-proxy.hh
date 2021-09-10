#pragma once

#include <QHash>
#include <QObject>

#include <clap/all.h>

#include "parameter-proxy.hh"

class PluginProxy : public QObject {
   Q_OBJECT

public:
   explicit PluginProxy(QObject *parent = nullptr);

   void defineParameter(const clap_param_info& info);

   Q_INVOKABLE ParameterProxy *param(clap_id paramId);
   Q_INVOKABLE QString toString() const;

private:
   std::unordered_map<clap_id, ParameterProxy *> _parameters;
};