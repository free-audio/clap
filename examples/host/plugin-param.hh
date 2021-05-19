#pragma once

#include <ostream>
#include <unordered_map>

#include <QObject>

#include <clap/all.h>

class PluginHost;
class PluginParam : public QObject {
   Q_OBJECT;

public:
   PluginParam(PluginHost &pluginHost, const clap_param_info &info, clap_param_value value);

   clap_param_value value() const { return value_; }
   void             setValue(clap_param_value v);

   clap_param_value modulatedValue() const { return modulated_value_; }
   void             setModulatedValue(clap_param_value v);

   bool        hasRange() const;
   bool        isValueEqualTo(const clap_param_value v) const;
   bool        isValueValid(const clap_param_value v) const;
   static bool areValuesEqual(clap_param_type type, clap_param_value v1, clap_param_value v2);

   void printShortInfo(std::ostream &os) const;
   void printInfo(std::ostream &os) const;
   void printValue(const clap_param_value v, std::ostream &os) const;

   void                   setInfo(const clap_param_info &info) noexcept { info_ = info; }
   bool                   isInfoEqualTo(const clap_param_info &info) const;
   bool                   isInfoCriticallyDifferentTo(const clap_param_info &info) const;
   clap_param_info &      info() noexcept { return info_; }
   const clap_param_info &info() const noexcept { return info_; }

   bool isBeingAdjusted() const noexcept { return is_being_adjusted_; }
   void beginAdjust() {
      Q_ASSERT(!is_being_adjusted_);
      is_being_adjusted_ = true;
   }
   void endAdjust() {
      Q_ASSERT(is_being_adjusted_);
      is_being_adjusted_ = false;
   }

signals:
   void infoChanged();
   void valueChanged();
   void modulatedValueChanged();

private:
   bool                                     is_being_adjusted_ = false;
   clap_param_info                          info_;
   clap_param_value                         value_;
   clap_param_value                         modulated_value_;
   std::unordered_map<int64_t, std::string> enum_entries_;
};