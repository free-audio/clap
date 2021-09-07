#pragma once

#include <cstdint>
#include <algorithm>

namespace clap {
   class ParameterInterpolator {
   public:
      ParameterInterpolator() = default;
      ParameterInterpolator(double val, double mod)
      {
         setValue(val, mod);
      }

      void setValue(double val, double mod)
      {
         _val0 = val;
         _val1 = val;
         _mod0 = mod;
         _mod1 = mod;
         _duration = 0;
         _offset = 0;
      }

      void setInterpolationData(double val0, double val1, double mod0, double mod1, uint32_t duration) noexcept
      {
         _val0 = val0;
         _val1 = val1;
         _mod0 = mod0;
         _mod1 = mod1;
         _duration = duration;
         _offset = 0;
      }

      double value() noexcept
      {
         if (_offset >= _duration)
            return _val1 + _mod1;

         const double x = static_cast<double>(_offset) / static_cast<double>(_duration);
         const double value = (_val1 + _mod1) * x + (_val0 + _mod0) * (1 - x);
         return value;
      }

      double mod() noexcept
      {
         if (_offset >= _duration)
            return _mod1;

         const double x = static_cast<double>(_offset) / static_cast<double>(_duration);
         const double value = (_mod1) * x + (_mod0) * (1 - x);
         return value;
      }

      void step(uint32_t n) noexcept
      {
         _offset += n;
      }

   private:
      double _val0 = 0;
      double _val1 = 0;
      double _mod0 = 0;
      double _mod1 = 0;
      uint32_t _duration = 0;
      uint32_t _offset = 0;
   };
} // namespace clap