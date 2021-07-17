#include "plugin-param.hh"
#include "plugin-host.hh"

PluginParam::PluginParam(PluginHost &pluginHost, const clap_param_info &info, double value)
   : QObject(&pluginHost), info_(info), value_(value) {}

void PluginParam::setValue(double v) {
   if (value_ == v)
      return;

   value_ = v;
   valueChanged();
}

void PluginParam::setModulation(double v) {
   if (modulation_ == v)
      return;

   modulation_ = v;
   modulatedValueChanged();
}

bool PluginParam::isValueValid(const double v) const {
   return info_.min_value <= v && v <= info_.max_value;
}

void PluginParam::printShortInfo(std::ostream &os) const {
   os << "id: " << info_.id << ", name: '" << info_.name << "', module: '" << info_.module << "'";
}

void PluginParam::printInfo(std::ostream &os) const {
   printShortInfo(os);
   os << ", min: " << info_.min_value << ", max: " << info_.max_value;
}

bool PluginParam::isInfoEqualTo(const clap_param_info &info) const {
   return !std::memcmp(&info, &info_, sizeof(clap_param_info));
}

bool PluginParam::isInfoCriticallyDifferentTo(const clap_param_info &info) const {
   assert(info_.id == info.id);
   return (info_.flags & CLAP_PARAM_IS_PER_NOTE) == (info.flags & CLAP_PARAM_IS_PER_NOTE) ||
          (info_.flags & CLAP_PARAM_IS_PER_CHANNEL) == (info.flags & CLAP_PARAM_IS_PER_CHANNEL) ||
          (info_.flags & CLAP_PARAM_IS_READONLY) == (info.flags & CLAP_PARAM_IS_READONLY) ||
          info_.min_value != info_.min_value || info_.max_value != info_.max_value;
}