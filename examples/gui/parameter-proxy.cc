#include "parameter-proxy.hh"
#include "../io/messages.hh"
#include "application.hh"

ParameterProxy::ParameterProxy(const clap_param_info &info, QObject *parent)
   : QObject(parent), id_(info.id), name_(info.name), module_(info.module),
     value_(info.default_value), minValue_(info.min_value), maxValue_(info.max_value),
     defaultValue_(info.default_value) {}

ParameterProxy::ParameterProxy(clap_id param_id, QObject *parent)
   : QObject(parent), id_(param_id) {}

void ParameterProxy::redefine(const clap_param_info &info) {
   Q_ASSERT(id_ == info.id);

   if (name_ != info.name) {
      name_ = info.name;
      nameChanged();
   }

   if (module_ != info.module) {
      module_ = info.module;
      moduleChanged();
   }

   setMinValueFromPlugin(info.min_value);
   setMaxValueFromPlugin(info.max_value);
   setDefaultValueFromPlugin(info.default_value);

   setValueFromPlugin(std::min(maxValue_, std::max(minValue_, value_)));
}

void ParameterProxy::setIsAdjusting(bool isAdjusting) {
   if (isAdjusting == isAdjusting_)
      return;

   isAdjusting_ = isAdjusting;
   if (isAdjusting) {
      clap::messages::BeginAdjustRequest rq{id_};
      Application::instance().remoteChannel().sendRequestAsync(rq);
   } else {
      clap::messages::EndAdjustRequest rq{id_};
      Application::instance().remoteChannel().sendRequestAsync(rq);
   }
}

void ParameterProxy::setValueFromUI(double value) {
   value_ = value;

   clap::messages::AdjustRequest rq{id_, value_};
   Application::instance().remoteChannel().sendRequestAsync(rq);
   valueChanged();
}

void ParameterProxy::setValueFromPlugin(double value) {
   if (value == value_)
      return;

   value_ = value;
   valueChanged();
}

void ParameterProxy::setModulationFromPlugin(double mod) {
   if (mod == modulation_)
      return;

   modulation_ = mod;
   modulationChanged();
}

void ParameterProxy::setMinValueFromPlugin(double minValue) {
   if (minValue == minValue_)
      return;

   minValue_ = minValue;
   minValueChanged();
}

void ParameterProxy::setMaxValueFromPlugin(double maxValue) {
   if (maxValue == maxValue_)
      return;

   maxValue_ = maxValue;
   maxValueChanged();
}

void ParameterProxy::setDefaultValueFromPlugin(double defaultValue) {
   if (defaultValue == defaultValue_)
      return;

   defaultValue_ = defaultValue;
   defaultValueChanged();
}