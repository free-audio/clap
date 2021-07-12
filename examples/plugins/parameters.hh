#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <clap/all.h>

#include "parameter-interpolator.hh"

namespace clap {
   class Parameter {
   public:
      Parameter(const clap_param_info &info) : info_(info) {}

      const double value() const noexcept { return value_; }
      const double modulation() const noexcept { return modulation_; }
      const double modulatedValue() const noexcept { return value_ + modulation_; }
      const clap_param_info &info() const noexcept { return info_; }

      void setDefaultValue() {
         value_ = info_.default_value;
         modulation_ = 0;
      }

      void setValue(double val) { value_ = val; }

      void setModulation(double mod) { modulation_ = mod; }

   private:
      clap_param_info info_;

      double value_;
      double modulation_;
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
      friend class boost::serialization::access;

      template <class Archive>
      void save(Archive &ar, const unsigned int version) const {
         std::vector<std::pair<clap_id, double>> values;
         for (auto &p : params_)
            values.emplace_back(p->info().id, p->value());

         ar << values;
      }

      template <class Archive>
      void load(Archive &ar, const unsigned int version) {
         std::vector<std::pair<clap_id, double>> values;
         ar >> values;

         for (auto &p : params_)
            p->setDefaultValue();

         for (auto &v : values) {
            auto *p = getById(v.first);
            if (!p)
               continue;
            p->setValue(v.second);
         }
      }

      BOOST_SERIALIZATION_SPLIT_MEMBER()

      std::vector<std::unique_ptr<Parameter>> params_;
      std::unordered_map<clap_id, Parameter *> id2param_;
   };

} // namespace clap

BOOST_CLASS_VERSION(clap::Parameters, 1)