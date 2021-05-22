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

   clap_param_value value;
   switch (info.type) {
   case CLAP_PARAM_FLOAT:
      value.d = newValue * (info.max_value.d - info.min_value.d) / DIAL_RANGE + info.min_value.d;
      pluginHost_.setParamValueByHost(*param_, value);
      break;
   case CLAP_PARAM_INT:
      value.i = (newValue * (info.max_value.i - info.min_value.i)) / DIAL_RANGE + info.min_value.i;
      pluginHost_.setParamValueByHost(*param_, value);
      break;
   }
}
void PluginQuickControlWidget::updateParamValue() {
   if (!param_)
      return;

   if (dial_->isSliderDown())
      return;

   auto info = param_->info();
   auto v = param_->value();
   switch (info.type) {
   case CLAP_PARAM_FLOAT:
      dial_->setValue(DIAL_RANGE * (v.d - info.min_value.d) /
                      (info.max_value.d - info.min_value.d));
      break;
   case CLAP_PARAM_INT:
      dial_->setValue((DIAL_RANGE * (v.i - info.min_value.i)) /
                      (info.max_value.i - info.min_value.i));
      break;
   }
}

void PluginQuickControlWidget::updateParamInfo() {
   label_->setText(param_ ? param_->info().name : "-");
}

void PluginQuickControlWidget::updateAll() {
   updateParamInfo();
   updateParamValue();
}
