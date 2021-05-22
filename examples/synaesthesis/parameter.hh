#pragma once

#include <clap/all.h>

namespace synaesthesis {
   class Parameter {
   private:
      clap_param_info  info_;
      clap_param_value value_;
      clap_param_value modulated_value_;
   };
} // namespace synaesthesis