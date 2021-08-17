#include "plugin-proxy.hh"

PluginProxy::PluginProxy(QObject *parent) : QObject(parent) {}

ParameterProxy *PluginProxy::param(clap_id paramId) {
   auto it = parameters_.find(paramId);
   if (it != parameters_.end())
      return it->second;

   auto *p = new ParameterProxy(paramId, this);
   parameters_.emplace(paramId, p);
   return p;
}

QString PluginProxy::toString() const { return "Plugin"; }

void PluginProxy::defineParameter(const clap_param_info &info)
{
   auto it = parameters_.find(info.id);
   if (it != parameters_.end())
      it->second->redefine(info);
   else
      parameters_.emplace(info.id, new ParameterProxy(info, this));
}