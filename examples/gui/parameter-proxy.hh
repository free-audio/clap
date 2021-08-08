#pragma once

#include <QObject>

class ParameterProxy : public QObject {
   Q_OBJECT
   Q_PROPERTY(uint32_t id READ getId)
   Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
   Q_PROPERTY(QString module READ getModule)
   Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)
   Q_PROPERTY(double modulation READ getModulation NOTIFY modulationChanged)

public:
   uint32_t getId() const { return id_; }
   const QString &getModule() const { return module_; }
   const QString &getName() const { return name_; }
   double getValue() const { return value_; }
   void setValue(double value);
   double getModulation() const { return modulation_; }
   void setModulation(double mod);

signals:
   void nameChanged();
   void valueChanged();
   void modulationChanged();

private:
   uint32_t id_;
   QString name_;
   QString module_;
   double value_;
   double modulation_;
};