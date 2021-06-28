#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

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
         setValue(info_.default_value, 0);
      }

      void setValue(double val, double mod) {
         value_ = val;
         modulation_ = mod;

         val0_ = val;
         val1_ = val;
         mod0_ = mod;
         mod1_ = mod;
         distance_ = 0;
         offset_ = 0;
      }

      void setInterpolationData(
         double val0, double val1, double mod0, double mod1, uint32_t distance) noexcept {
         val0_ = val0;
         val1_ = val1;
         mod0_ = mod0;
         mod1_ = mod1;
         distance_ = distance;
         offset_ = 0;

         value_ = val0;
         modulation_ = mod0;
      }

      void step(uint32_t n) noexcept {
         offset_ += n;
         if (offset_ >= n) {
            offset_ = n;
            value_ = val1_;
            modulation_ = mod1_;
         } else {
            const double x = static_cast<double>(offset_) / static_cast<double>(distance_);
            value_ = (val1_ + mod1_) * x + (val0_ + mod0_) * (1 - x);
            mod1_ = (mod1_)*x + (mod0_) * (1 - x);
         }
      }

   private:
      clap_param_info info_;

      double value_;
      double modulation_;

      double val0_ = 0;
      double val1_ = 0;
      double mod0_ = 0;
      double mod1_ = 0;
      uint32_t distance_ = 0;
      uint32_t offset_ = 0;
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

         for (auto & p : params_)
            p->setDefaultValue();

         for (auto &v : values)
         {
            auto *p = getById(v.first);
            if (!p)
               continue;
            p->setValue(v.second, 0);
         }
      }

      BOOST_SERIALIZATION_SPLIT_MEMBER()

      std::vector<std::unique_ptr<Parameter>> params_;
      std::unordered_map<clap_id, Parameter *> id2param_;
   };

} // namespace clap

BOOST_CLASS_VERSION(clap::Parameters, 1)