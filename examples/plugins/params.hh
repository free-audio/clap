#include <clap/all.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace clap {
   struct EnumEntry {
      std::string name;
      int64_t     value;
   };

   struct EnumDefinition {
      std::vector<EnumEntry> entries;
   };

   struct Parameter {
      clap_param_info  info;
      EnumDefinition   enumDefinition;
      clap_param_value value;
      clap_param_value modulation;
      double           valueRamp;
      double           modulationRamp;
   };

   struct ParameterBank {
      std::vector<Parameter>                   params;
      std::unordered_map<clap_id, Parameter *> hmap;
   };
} // namespace clap