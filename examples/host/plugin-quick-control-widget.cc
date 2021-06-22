#include <QDial>
#include <QLabel>
#include <QVBoxLayout>

#include "plugin-host.hh"
#include "plugin-param.hh"
#include "plugin-quick-control-widget.hh"

PluginQuickControlWidget::PluginQuickControlWidget(QWidget *parent, PluginHost &pluginHost)
   : QWidget(parent), pluginHost_(pluginHost) {
   dial_ = new QDial(this);
   label_ = new QLabel(this);

   auto layout = new QVBoxLayout(this);
   layout->addWidget(dial_);
   layout->addWidget(label_);
   setLayout(layout);

   setPluginParam(nullptr);
}

void PluginQuickControlWidget::setPluginParam(PluginParam *param) {
   if (param_ == param)
      return;

   if (param_)
      disconnectFromParam();

   Q_ASSERT(!param_);

   connectToParam(param);
}

void PluginQuickControlWidget::connectToParam(PluginParam *param) {
   Q_ASSERT(!param_);

   param_ = param;
   connect(param_, &PluginParam::infoChanged, this, &PluginQuickControlWidget::paramInfoChanged);
   connect(param_, &PluginParam::valueChanged, this, &PluginQuickControlWidget::paramValueChanged);
   updateAll();
}

void PluginQuickControlWidget::disconnectFromParam() {
   Q_ASSERT(param_);

   disconnect(param_, &PluginParam::infoChanged, this, &PluginQuickControlWidget::paramInfoChanged);
   disconnect(
      param_, &PluginParam::valueChanged, this, &PluginQuickControlWidget::paramValueChanged);

   updateAll();
}

void PluginQuickControlWidget::paramInfoChanged() { updateParamInfo(); }

void PluginQuickControlWidget::paramValueChanged() { updateParamValue(); }

void PluginQuickControlWidget::dialValueChanged(int newValue) {
   if (!param_)
      return;

   if (!dial_->isSliderDown())
      return;

   auto &info = param_->info();

   double value = newValue * (info.max_value - info.min_value) / DIAL_RANGE + info.min_value;
   pluginHost_.setParamValueByHost(*param_, value);
}
void PluginQuickControlWidget::updateParamValue() {
   if (!param_)
      return;

   if (dial_->isSliderDown())
      return;

   auto info = param_->info();
   auto v = param_->value();
   dial_->setValue(DIAL_RANGE * (v - info.min_value) / (info.max_value - info.min_value));
}

void PluginQuickControlWidget::updateParamInfo() {
   label_->setText(param_ ? param_->info().name : "-");
}

void PluginQuickControlWidget::updateAll() {
   updateParamInfo();
   updateParamValue();
}
