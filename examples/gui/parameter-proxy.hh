#pragma once

#include <QObject>

#include <clap/all.h>

class ParameterProxy : public QObject {
   Q_OBJECT
   Q_PROPERTY(uint32_t id READ getId)
   Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
   Q_PROPERTY(QString module READ getModule)
   Q_PROPERTY(double value READ getValue WRITE setValueFromUI NOTIFY valueChanged)
   Q_PROPERTY(double modulation READ getModulation NOTIFY modulationChanged)
   Q_PROPERTY(double minValue READ getMinValue NOTIFY minValueChanged)
   Q_PROPERTY(double maxValue READ getMaxValue NOTIFY maxValueChanged)
   Q_PROPERTY(double defaultValue READ getDefaultValue NOTIFY defaultValueChanged)
   Q_PROPERTY(bool isAdjusting READ isAdjusting WRITE setIsAdjusting)

public:
   ParameterProxy(const clap_param_info &info, QObject *parent = nullptr);

   uint32_t getId() const { return id_; }
   const QString &getModule() const { return module_; }
   const QString &getName() const { return name_; }

   double getValue() const { return value_; }
   void setValueFromUI(double value);
   void setValueFromPlugin(double value);

   double getModulation() const { return modulation_; }
   void setModulationFromPlugin(double mod);

   bool isAdjusting() const { return isAdjusting_; }
   void setIsAdjusting(bool isAdjusting);

   double getMinValue() const { return minValue_; }
   void setMinValueFromPlugin(double minValue);
   double getMaxValue() const { return maxValue_; }
   void setMaxValueFromPlugin(double maxValue);
   double getDefaultValue() const { return defaultValue_; }
   void setDefaultValueFromPlugin(double defaultValue);

signals:
   void nameChanged();
   void valueChanged();
   void modulationChanged();
   void minValueChanged();
   void maxValueChanged();
   void defaultValueChanged();

private:
   const uint32_t id_;
   QString name_;
   QString module_;
   double value_ = 0;
   double modulation_ = 0;
   double minValue_ = 0;
   double maxValue_ = 0;
   double defaultValue_ = 0;
   bool isAdjusting_ = false;
};