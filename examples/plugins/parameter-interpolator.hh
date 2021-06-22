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
         val0_ = val;
         val1_ = val;
         mod0_ = mod;
         mod1_ = mod;
         duration_ = 0;
         offset_ = 0;
      }

      void setInterpolationData(double val0, double val1, double mod0, double mod1, uint32_t duration) noexcept
      {
         val0_ = val0;
         val1_ = val1;
         mod0_ = mod0;
         mod1_ = mod1;
         duration_ = duration;
         offset_ = 0;
      }

      double value() noexcept
      {
         if (offset_ >= duration_)
            return val1_ + mod1_;

         const double x = static_cast<double>(offset_) / static_cast<double>(duration_);
         const double value = (val1_ + mod1_) * x + (val0_ + mod0_) * (1 - x);
         return value;
      }

      double mod() noexcept
      {
         if (offset_ >= duration_)
            return mod1_;

         const double x = static_cast<double>(offset_) / static_cast<double>(duration_);
         const double value = (mod1_) * x + (mod0_) * (1 - x);
         return value;
      }

      void step(uint32_t n) noexcept
      {
         offset_ += n;
      }

   private:
      double val0_ = 0;
      double val1_ = 0;
      double mod0_ = 0;
      double mod1_ = 0;
      uint32_t duration_ = 0;
      uint32_t offset_ = 0;
   };
} // namespace clap