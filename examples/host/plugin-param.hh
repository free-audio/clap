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

   double value() const { return _value; }
   void setValue(double v);

   double modulation() const { return _modulation; }
   void setModulation(double v);

   double modulatedValue() const {
      return std::min(_info.max_value, std::max(_info.min_value, _value + _modulation));
   }

   bool isValueValid(const double v) const;

   void printShortInfo(std::ostream &os) const;
   void printInfo(std::ostream &os) const;

   void setInfo(const clap_param_info &info) noexcept { _info = info; }
   bool isInfoEqualTo(const clap_param_info &info) const;
   bool isInfoCriticallyDifferentTo(const clap_param_info &info) const;
   clap_param_info &info() noexcept { return _info; }
   const clap_param_info &info() const noexcept { return _info; }

   bool isBeingAdjusted() const noexcept { return _isBeingAdjusted; }
   void setIsAdjusting(bool isAdjusting) {
      if (isAdjusting && !_isBeingAdjusted)
         beginAdjust();
      else if (!isAdjusting && _isBeingAdjusted)
         endAdjust();
   }
   void beginAdjust() {
      Q_ASSERT(!_isBeingAdjusted);
      _isBeingAdjusted = true;
      emit isBeingAdjustedChanged();
   }
   void endAdjust() {
      Q_ASSERT(_isBeingAdjusted);
      _isBeingAdjusted = false;
      emit isBeingAdjustedChanged();
   }

signals:
   void isBeingAdjustedChanged();
   void infoChanged();
   void valueChanged();
   void modulatedValueChanged();

private:
   bool _isBeingAdjusted = false;
   clap_param_info _info;
   double _value = 0;
   double _modulation = 0;
   std::unordered_map<int64_t, std::string> _enumEntries;
};