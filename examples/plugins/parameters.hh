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
      explicit Parameter(const clap_param_info &info) : info_(info) { info_.cookie = this; }

      Parameter(const Parameter &) = delete;
      Parameter(Parameter &&) = delete;
      Parameter &operator=(const Parameter &) = delete;
      Parameter &operator=(Parameter &&) = delete;

      const double value() const noexcept { return value_; }
      const double modulation() const noexcept { return modulation_; }
      const double modulatedValue() const noexcept { return value_ + modulation_; }
      const clap_param_info &info() const noexcept { return info_; }

      void setDefaultValue() {
         value_ = info_.default_value;
         modulation_ = 0;
      }

      void setValueImmediately(double val) {
         value_ = val;
         valueRamp_ = 0;
         valueSteps_ = 0;
      }
      void setModulationImmediately(double mod) {
         modulation_ = mod;
         modulationRamp_ = 0;
         modulationSteps_ = 0;
      }

      void setValueSmoothed(double val, uint16_t steps)
      {
         assert(steps > 0);
         valueRamp_ = (val - value_) / steps;
         valueSteps_ = steps;
      }

      void setModulationSmoothed(double mod, uint16_t steps)
      {
         assert(steps > 0);
         modulationRamp_ = (mod - modulation_) / steps;
         modulationSteps_ = steps;
      }

      // Advances the value by 1 samples and return the new value + modulation
      double step() {
         if (valueSteps_ > 0) [[unlikely]]
         {
            value_ += valueRamp_;
            --valueSteps_;
         }

         if (modulationSteps_ > 0) [[unlikely]]
         {
            modulation_ += modulationRamp_;
            --modulationSteps_;
         }

         return value_ + modulation_;
      }

      // Advances the value by n samples and return the new value + modulation
      double step(uint32_t n) {
         if (valueSteps_ > 0) [[unlikely]]
         {
            auto k = std::min<uint32_t>(valueSteps_, n);
            value_ += k * valueRamp_;
            valueSteps_ -= k;
         }

         if (modulationSteps_ > 0) [[unlikely]]
         {
            auto k = std::min<uint32_t>(valueSteps_, n);
            modulation_ += k * modulationRamp_;
            modulationSteps_ -= k;
         }

         return value_ + modulation_;
      }

   private:
      clap_param_info info_;

      double value_ = 0;
      double modulation_ = 0;

      double valueRamp_ = 0;
      double modulationRamp_ = 0;

      uint16_t valueSteps_ = 0;
      uint16_t modulationSteps_ = 0;
   };

   class Parameters {
   public:
      Parameters() = default;
      Parameters(const Parameters &parameters);

      void addParameter(const clap_param_info &info);
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
            p->setValueImmediately(v.second);
         }
      }

      BOOST_SERIALIZATION_SPLIT_MEMBER()

      std::vector<std::unique_ptr<Parameter>> params_;
      std::unordered_map<clap_id, Parameter *> id2param_;
   };

} // namespace clap

BOOST_CLASS_VERSION(clap::Parameters, 1)