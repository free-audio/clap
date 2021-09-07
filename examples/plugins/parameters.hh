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
      explicit Parameter(const clap_param_info &info) : _info(info) { _info.cookie = this; }

      Parameter(const Parameter &) = delete;
      Parameter(Parameter &&) = delete;
      Parameter &operator=(const Parameter &) = delete;
      Parameter &operator=(Parameter &&) = delete;

      const double value() const noexcept { return _value; }
      const double modulation() const noexcept { return _modulation; }
      const double modulatedValue() const noexcept { return _value + _modulation; }
      const clap_param_info &info() const noexcept { return _info; }

      void setDefaultValue() {
         _value = _info.default_value;
         _modulation = 0;
      }

      void setValueImmediately(double val) {
         _value = val;
         _valueRamp = 0;
         _valueSteps = 0;
      }
      void setModulationImmediately(double mod) {
         _modulation = mod;
         _modulationRamp = 0;
         _modulationSteps = 0;
      }

      void setValueSmoothed(double val, uint16_t steps) {
         assert(steps > 0);
         _valueRamp = (val - _value) / steps;
         _valueSteps = steps;
      }

      void setModulationSmoothed(double mod, uint16_t steps) {
         assert(steps > 0);
         _modulationRamp = (mod - _modulation) / steps;
         _modulationSteps = steps;
      }

      // Advances the value by 1 samples and return the new value + modulation
      double step() {
         if (_valueSteps > 0) [[unlikely]] {
            _value += _valueRamp;
            --_valueSteps;
         }

         if (_modulationSteps > 0) [[unlikely]] {
            _modulation += _modulationRamp;
            --_modulationSteps;
         }

         return _value + _modulation;
      }

      // Advances the value by n samples and return the new value + modulation
      double step(uint32_t n) {
         if (_valueSteps > 0) [[unlikely]] {
            auto k = std::min<uint32_t>(_valueSteps, n);
            _value += k * _valueRamp;
            _valueSteps -= k;
         }

         if (_modulationSteps > 0) [[unlikely]] {
            auto k = std::min<uint32_t>(_valueSteps, n);
            _modulation += k * _modulationRamp;
            _modulationSteps -= k;
         }

         return _value + _modulation;
      }

   private:
      clap_param_info _info;

      double _value = 0;
      double _modulation = 0;

      double _valueRamp = 0;
      double _modulationRamp = 0;

      uint16_t _valueSteps = 0;
      uint16_t _modulationSteps = 0;
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
         for (auto &p : _params)
            values.emplace_back(p->info().id, p->value());

         ar << values;
      }

      template <class Archive>
      void load(Archive &ar, const unsigned int version) {
         std::vector<std::pair<clap_id, double>> values;
         ar >> values;

         for (auto &p : _params)
            p->setDefaultValue();

         for (auto &v : values) {
            auto *p = getById(v.first);
            if (!p)
               continue;
            p->setValueImmediately(v.second);
         }
      }

      BOOST_SERIALIZATION_SPLIT_MEMBER()

      std::vector<std::unique_ptr<Parameter>> _params;
      std::unordered_map<clap_id, Parameter *> _id2param;
   };

} // namespace clap

BOOST_CLASS_VERSION(clap::Parameters, 1)