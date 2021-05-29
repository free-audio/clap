#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <clap/all.h>

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
      double           valueRamp = 0;
      double           modulationRamp = 0;
   };

   class Parameters {
   public:
      Parameters() = default;
      Parameters(const Parameters &parameters);

      void addParameter(const Parameter &param);

      size_t count() const noexcept;

      Parameter *getByIndex(size_t index) const noexcept;

      Parameter *getById(clap_id id) const noexcept;

   private:
      std::vector<std::unique_ptr<Parameter>>  params_;
      std::unordered_map<clap_id, Parameter *> id2param_;
   };
} // namespace clap