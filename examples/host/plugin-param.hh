#pragma once

#include <ostream>
#include <unordered_map>

#include <QObject>

#include <clap/all.h>

class PluginHost;
class PluginParam : public QObject {
   Q_OBJECT;

public:
   PluginParam(PluginHost &pluginHost, const clap_param_info &info, double value);

   double value() const { return value_; }
   void setValue(double v);

   double modulation() const { return modulation_; }
   void setModulation(double v);

   double modulatedValue() const {
      return std::min(info_.max_value, std::max(info_.min_value, value_ + modulation_));
   }

   bool isValueValid(const double v) const;

   void printShortInfo(std::ostream &os) const;
   void printInfo(std::ostream &os) const;

   void setInfo(const clap_param_info &info) noexcept { info_ = info; }
   bool isInfoEqualTo(const clap_param_info &info) const;
   bool isInfoCriticallyDifferentTo(const clap_param_info &info) const;
   clap_param_info &info() noexcept { return info_; }
   const clap_param_info &info() const noexcept { return info_; }

   bool isBeingAdjusted() const noexcept { return is_being_adjusted_; }
   void beginAdjust() {
      Q_ASSERT(!is_being_adjusted_);
      is_being_adjusted_ = true;
      isBeingAdjustedChanged();
   }
   void endAdjust() {
      Q_ASSERT(is_being_adjusted_);
      is_being_adjusted_ = false;
      isBeingAdjustedChanged();
   }

signals:
   void isBeingAdjustedChanged();
   void infoChanged();
   void valueChanged();
   void modulatedValueChanged();

private:
   bool is_being_adjusted_ = false;
   clap_param_info info_;
   double value_ = 0;
   double modulation_ = 0;
   std::unordered_map<int64_t, std::string> enum_entries_;
};