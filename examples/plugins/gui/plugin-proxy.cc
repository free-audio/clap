#include "plugin-proxy.hh"

PluginProxy::PluginProxy(QObject *parent) : QObject(parent) {}

ParameterProxy *PluginProxy::param(clap_id paramId) {
   auto it = _parameters.find(paramId);
   if (it != _parameters.end())
      return it->second;

   auto *p = new ParameterProxy(paramId, this);
   _parameters.insert_or_assign(paramId, p);
   return p;
}

QString PluginProxy::toString() const { return "Plugin"; }

void PluginProxy::defineParameter(const clap_param_info &info)
{
   auto it = _parameters.emplace(info.id, new ParameterProxy(info, this));
   if (it.second)
      it.first->second->redefine(info);
}