#include "plugin-param.hh"
#include "plugin-host.hh"

PluginParam::PluginParam(PluginHost &pluginHost, const clap_param_info &info, double value)
   : QObject(&pluginHost), _info(info), _value(value) {}

void PluginParam::setValue(double v) {
   if (_value == v)
      return;

   _value = v;
   valueChanged();
}

void PluginParam::setModulation(double v) {
   if (_modulation == v)
      return;

   _modulation = v;
   modulatedValueChanged();
}

bool PluginParam::isValueValid(const double v) const {
   return _info.min_value <= v && v <= _info.max_value;
}

void PluginParam::printShortInfo(std::ostream &os) const {
   os << "id: " << _info.id << ", name: '" << _info.name << "', module: '" << _info.module << "'";
}

void PluginParam::printInfo(std::ostream &os) const {
   printShortInfo(os);
   os << ", min: " << _info.min_value << ", max: " << _info.max_value;
}

bool PluginParam::isInfoEqualTo(const clap_param_info &info) const {
   return !std::memcmp(&info, &_info, sizeof(clap_param_info));
}

bool PluginParam::isInfoCriticallyDifferentTo(const clap_param_info &info) const {
   assert(_info.id == info.id);
   return (_info.flags & CLAP_PARAM_IS_PER_NOTE) == (info.flags & CLAP_PARAM_IS_PER_NOTE) ||
          (_info.flags & CLAP_PARAM_IS_PER_CHANNEL) == (info.flags & CLAP_PARAM_IS_PER_CHANNEL) ||
          (_info.flags & CLAP_PARAM_IS_READONLY) == (info.flags & CLAP_PARAM_IS_READONLY) ||
          _info.min_value != _info.min_value || _info.max_value != _info.max_value;
}