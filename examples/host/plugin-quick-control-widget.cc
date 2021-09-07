#include <QDial>
#include <QLabel>
#include <QVBoxLayout>

#include "plugin-host.hh"
#include "plugin-param.hh"
#include "plugin-quick-control-widget.hh"

PluginQuickControlWidget::PluginQuickControlWidget(QWidget *parent, PluginHost &pluginHost)
   : QWidget(parent), pluginHost_(pluginHost) {
   _dial = new QDial(this);
   _label = new QLabel(this);

   auto layout = new QVBoxLayout(this);
   layout->addWidget(_dial);
   layout->addWidget(_label);
   setLayout(layout);

   setPluginParam(nullptr);
}

void PluginQuickControlWidget::setPluginParam(PluginParam *param) {
   if (_param == param)
      return;

   if (_param)
      disconnectFromParam();

   Q_ASSERT(!_param);

   connectToParam(param);
}

void PluginQuickControlWidget::connectToParam(PluginParam *param) {
   Q_ASSERT(!_param);

   _param = param;
   connect(_param, &PluginParam::infoChanged, this, &PluginQuickControlWidget::paramInfoChanged);
   connect(_param, &PluginParam::valueChanged, this, &PluginQuickControlWidget::paramValueChanged);
   updateAll();
}

void PluginQuickControlWidget::disconnectFromParam() {
   Q_ASSERT(_param);

   disconnect(_param, &PluginParam::infoChanged, this, &PluginQuickControlWidget::paramInfoChanged);
   disconnect(
      _param, &PluginParam::valueChanged, this, &PluginQuickControlWidget::paramValueChanged);

   updateAll();
}

void PluginQuickControlWidget::paramInfoChanged() { updateParamInfo(); }

void PluginQuickControlWidget::paramValueChanged() { updateParamValue(); }

void PluginQuickControlWidget::dialValueChanged(int newValue) {
   if (!_param)
      return;

   if (!_dial->isSliderDown())
      return;

   auto &info = _param->info();

   double value = newValue * (info.max_value - info.min_value) / DIAL_RANGE + info.min_value;
   pluginHost_.setParamValueByHost(*_param, value);
}
void PluginQuickControlWidget::updateParamValue() {
   if (!_param)
      return;

   if (_dial->isSliderDown())
      return;

   auto info = _param->info();
   auto v = _param->value();
   _dial->setValue(DIAL_RANGE * (v - info.min_value) / (info.max_value - info.min_value));
}

void PluginQuickControlWidget::updateParamInfo() {
   _label->setText(_param ? _param->info().name : "-");
}

void PluginQuickControlWidget::updateAll() {
   updateParamInfo();
   updateParamValue();
}
