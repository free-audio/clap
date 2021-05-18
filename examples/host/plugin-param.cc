#include "plugin-param.hh"
#include "plugin-host.hh"

PluginParam::PluginParam(PluginHost &           pluginHost,
                         const clap_param_info &info,
                         clap_param_value       value)
   : QObject(&pluginHost), info_(info), value_(value), modulated_value_(value) {}

void PluginParam::setValue(clap_param_value v) {
   if (isValueEqualTo(v))
      return;
   value_ = v;
   valueChanged();
}

void PluginParam::setModulatedValue(clap_param_value v) {
   if (areValuesEqual(info_.type, modulated_value_, v))
      return;
   modulated_value_ = v;
   modulatedValueChanged();
}

bool PluginParam::hasRange() const {
   switch (info_.type) {
   case CLAP_PARAM_INT:
   case CLAP_PARAM_FLOAT:
      return true;
   default:
      return false;
   }
}

bool PluginParam::areValuesEqual(clap_param_type type, clap_param_value v1, clap_param_value v2) {
   switch (type) {
   case CLAP_PARAM_BOOL:
      return v1.b == v2.b;
   case CLAP_PARAM_ENUM:
   case CLAP_PARAM_INT:
      return v1.i == v2.i;
   case CLAP_PARAM_FLOAT:
      return v1.d == v2.d;
   default:
      std::terminate();
   }
}

bool PluginParam::isValueEqualTo(const clap_param_value v) const {
   return areValuesEqual(info_.type, value_, v);
}

bool PluginParam::isValueValid(const clap_param_value v) const {
   switch (info_.type) {
   case CLAP_PARAM_BOOL:
      return true;
   case CLAP_PARAM_ENUM:
      return enum_entries_.find(v.i) != enum_entries_.end();
   case CLAP_PARAM_INT:
      return info_.min_value.i <= v.i && v.i <= info_.max_value.i;
   case CLAP_PARAM_FLOAT:
      return info_.min_value.d <= v.d && v.d <= info_.max_value.d;
   default:
      std::terminate();
   }
}

void PluginParam::printInfo(std::ostream &os) const {
   os << "id: " << info_.id << ", name: '" << info_.name << "', module: '" << info_.module << "'";

   if (hasRange()) {
      os << ", min: ";
      printValue(info_.min_value, os);
      os << ", max: ";
      printValue(info_.max_value, os);
   }
}

void PluginParam::printValue(const clap_param_value v, std::ostream &os) const {
   switch (info_.type) {
   case CLAP_PARAM_BOOL:
      os << (v.b ? "true" : "false");
      return;
   case CLAP_PARAM_ENUM: {
      auto it = enum_entries_.find(v.i);
      if (it != enum_entries_.end())
         os << it->second << "=" << v.i;
      else
         os << "(unknown enum entry)=" << v.i;
   }
      return;
   case CLAP_PARAM_INT:
      os << v.i;
      return;
   case CLAP_PARAM_FLOAT:
      os << v.d;
      return;
   default:
      std::terminate();
   }
}

bool PluginParam::isInfoEqualTo(const clap_param_info &info) const {
   return !isInfoCriticallyDifferentTo(info) &&
          !strncmp(info_.name, info.name, sizeof(info.name)) &&
          !strncmp(info_.module, info.module, sizeof(info.module)) &&
          info_.is_used == info.is_used && info_.is_periodic == info.is_periodic &&
          info_.is_hidden == info.is_hidden && info_.is_bypass == info.is_bypass &&
          areValuesEqual(info.type, info_.default_value, info_.default_value);
}

bool PluginParam::isInfoCriticallyDifferentTo(const clap_param_info &info) const {
   return info_.id != info.id || info_.is_per_note != info.is_per_note ||
          info_.is_per_channel != info.is_per_channel || info_.is_locked != info.is_locked ||
          info_.is_automatable != info.is_automatable || info_.type != info.type ||
          ((info.type == CLAP_PARAM_INT || info.type == CLAP_PARAM_FLOAT) &&
              !areValuesEqual(info.type, info_.min_value, info_.min_value) ||
           !areValuesEqual(info.type, info_.max_value, info_.max_value)) ||
          (info.type == CLAP_PARAM_ENUM && info.enum_entry_count != info_.enum_entry_count);
}